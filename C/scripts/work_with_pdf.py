import os

from process_pdf import ProcessPDF
from ultralytics import YOLO


def _annotation_filename(page_index: int) -> str:
    """Return the annotation file name for a 0-based page index."""
    return f"page_{page_index + 1}.txt"


def _page_index_from_annotation_filename(filename: str) -> int:
    """Parse a 0-based page index from an annotation file name produced by
    :func:`_annotation_filename`."""
    stem = os.path.splitext(os.path.basename(filename))[0]
    return int(stem.split('_', 1)[1]) - 1


def segment_geometric_plan(pdf_path, path_GP_folder, model, path_to_annotations_folder):

    pdf_processor = ProcessPDF(pdf_path)

    pages = pdf_processor.convert_pdf_to_images(save_path=path_GP_folder)

    os.makedirs(path_to_annotations_folder, exist_ok=True)

    for i, page_img_path in enumerate(pages):
        results = model(page_img_path)

        annot_file = os.path.join(path_to_annotations_folder, _annotation_filename(i))
        with open(annot_file, "w") as f:
            for result in results:
                # YOLO stores the source image shape as (height, width)
                height_image, width_image = result.orig_shape
                for box in result.boxes:
                    # Extract coordinates from YOLO
                    x1, y1, x2, y2 = [v.item() for v in box.xyxy[0]]
                    class_id = int(box.cls[0].item())

                    # Convert to PDF points
                    nx1, ny1, nx2, ny2 = convert_image_coordinates_to_normalized(
                        x1, y1, x2, y2, (width_image, height_image)
                    )

                    # Save annotation
                    f.write(f"{class_id} {nx1} {ny1} {nx2} {ny2}\n")

def convert_image_coordinates_to_normalized(x1, y1, x2, y2, size_of_image):
    width_image, height_image = size_of_image

    # Divide the absolute pixel coordinate by the total image dimensions
    nx1 = x1 / width_image
    ny1 = y1 / height_image
    nx2 = x2 / width_image
    ny2 = y2 / height_image

    # We use 6 decimal places because percentages need high precision
    return round(nx1, 6), round(ny1, 6), round(nx2, 6), round(ny2, 6)

# Divide PDF into separate pages based on annotations as pdf and images
def divide_pdf_into_separate_pages(pdf_path, path_to_annotations_folder, output_folder, class_id, name_of_file=None):
    # initialize PDF processor
    pdf_processor = ProcessPDF(pdf_path)

    # page dimensions in PDF points (rotation-aware), indexed 0-based
    page_dims = pdf_processor.get_pdf_page_dimensions()

    # get files in annotations folder
    annotation_files = [f for f in os.listdir(path_to_annotations_folder) if f.endswith('.txt')]

    # (page_index, x0, y0, x1, y1) tuples ready for ProcessPDF.cropping_pdf
    pages_with_class = []

    for annot_file in annotation_files:
        page_index = _page_index_from_annotation_filename(annot_file)
        page_width, page_height = page_dims[page_index]

        with open(os.path.join(path_to_annotations_folder, annot_file), 'r') as f:
            lines = f.readlines()
            for line in lines:
                parts = line.strip().split()
                class_id_in_file, nx1, ny1, nx2, ny2 = parts
                if class_id_in_file != str(class_id):
                    continue

                # convert normalized image-space bbox to PDF points
                x0, y0, x1, y1 = ProcessPDF.normalized_to_pdf_points(
                    page_width, page_height,
                    float(nx1), float(ny1), float(nx2), float(ny2),
                )

                pages_with_class.append((page_index, x0, y0, x1, y1))

    # crop PDF based on collected pages_with_class
    pdf_processor.cropping_pdf(save_path=output_folder, crop_box=pages_with_class, name_of_file=name_of_file)

def convert_pdf_to_images(pdf_path, output_folder, binarize=False):
    pdf_processor = ProcessPDF(pdf_path)

    pages = pdf_processor.convert_pdf_to_images(save_path=output_folder, binarize=binarize, dpi=300)

    return pages

def classify_pdf(pdf_path, save_path, model):
    pdf_processor = ProcessPDF(pdf_path)

    pages = pdf_processor.convert_pdf_to_images(save_path=save_path)

    classifications = []
    for page_img_path in pages:
        results = model(page_img_path)
        top_class_id = results[0].probs.top1
        classifications.append(results[0].names[top_class_id])

    classes, gnss_indices = classification_rules(classifications)

    if gnss_indices:
        pdf_processor.extract_pdf_pages(page_index=gnss_indices, save_path=save_path, name_of_file="gnss.pdf")

    return classes


def classification_rules(classifications):
    if not classifications:
        return None, []

    gnss_indices = [i for i, cls in enumerate(classifications) if cls == 'gnss']

    has_gnss = len(gnss_indices) > 0
    has_prot = 'prot' in classifications
    has_zap = 'zap' in classifications

    # Priority rule: if any of gnss/prot/zap are present, the document is
    # one of the official protocol types. Pick prot vs zap by majority and
    # ignore the weighted scoring below — those classes outrank 'nacrt'
    # regardless of how many 'nacrt' pages were detected.
    if has_gnss or has_prot or has_zap:
        prot_count = classifications.count('prot')
        zap_count = classifications.count('zap')

        main_class = 'prot' if prot_count >= zap_count else 'zap'
        return main_class, gnss_indices


    scores = {}
    for cls in classifications:
        if cls not in scores:
            scores[cls] = 0


        # 'nacrt' (drawing) pages are common filler and would dominate a
        # plain page-count vote, so they're down-weighted to let any other
        # class win when present in roughly equal numbers.
        if cls == 'nacrt':
            scores[cls] += 0.75
        else:
            scores[cls] += 1.0

    main_class = max(scores, key=scores.get)

    return main_class, gnss_indices
