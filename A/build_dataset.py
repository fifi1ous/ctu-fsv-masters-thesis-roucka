import csv
import io
import json
import os
import re
import sys
import unicodedata
from pathlib import Path

import fitz  # PyMuPDF
import numpy as np
import pytesseract
import torch
from PIL import Image
from tqdm import tqdm
from ultralytics import YOLO


# --- 0. Configuration ---
# path to the raw dataset
RAW_ROOT = Path(r"C:\Users\phill\Downloads\GP_test")
# path to the output dataset
OUT_ROOT = Path(r"C:\Users\phill\Downloads\Dataset")

# maximum number of pages per (author, class) for classification
CLS_CAP = 100         
# rasterization DPI for PNG output
DPI = 300                 
# Tesseract language code (Czech)
OCR_LANG = "ces"          
# path to the YOLO weights
YOLO_WEIGHTS = "yolo11n-cls.pt"
# A4 page geometry
A4_W = 595.0  
A4_H = 842.0
# buffer for the A4 page
BUFFER = 10.0 


# class names
CLASS_NAMES = [
    "dsps", "gnss", "gp", "nacrt", "popispole",
    "prot", "sezvlast", "vymery", "vytyc", "zadost", "zap",
]

# markers for GNSS class from the page text
GNSS_MARKERS = [
    "protokol určení bodů pomocí technologií gnss",
    "protokol určení bodů technologií gnss",
    "gnss protokol o rtk observacích",
    "informace o observacích",
    "zápisník měření technologií gnss",
    "výška antény",
    "příloha v.1",
    "příloha v.5",
    "příloha v.6",
    "fazového",
    "vrs3",
    "czepos",
    "fixovano",
    "rtk",
    "gnss (rtk)",
    "pdop",
    "etrs89",
]

# list of stop-words
CZECH_STOPWORDS = {
    "a", "aby", "aj", "ale", "anebo", "ani", "ano", "asi", "az", "až",
    "bez", "bude", "budem", "budes", "byl", "byla", "byli", "bylo", "byly",
    "bys", "co", "coz", "což", "cz", "dal", "další", "do", "ho", "i",
    "ja", "já", "jak", "jako", "jeho", "jej", "její", "jejich", "jen",
    "jenž", "jeste", "ještě", "jestli", "jestliže", "ji", "jí", "jiz", "již",
    "jsem", "jses", "jseš", "jsi", "jsme", "jsou", "jste", "k", "kam", "kde",
    "kdo", "kdy", "kdyz", "když", "ke", "ktera", "která", "ktere", "které",
    "kteri", "kteří", "ktery", "který", "ku", "ma", "má", "mate", "máte",
    "me", "mě", "mezi", "mi", "mit", "mít", "mne", "mně", "mnou", "muj",
    "můj", "muze", "může", "my", "nad", "nam", "nám", "napiste",
    "naše", "naši", "ne", "nebo", "nebot", "neboť", "nec", "neco", "něco",
    "nedavno", "neg", "nejsi", "nejsou", "nemam", "nemám",
    "nemate", "nemáte", "neni", "není", "nez", "než", "ni", "nic", "nove",
    "nové", "novy", "nový", "o", "od", "ode", "on", "oni", "ono", "ony",
    "pak", "po", "pod", "podle", "pokud", "pouze", "prave", "právě",
    "pred", "před", "pres", "přes", "pri", "při", "proc", "proč",
    "proto", "protoze", "protože", "prvni", "první", "pta", "re", "s", "se",
    "si", "sice", "ste", "sve", "své", "svuj", "svůj", "svych", "svých",
    "svym", "svým", "svymi", "svými", "ta", "tady", "tak", "take", "také",
    "takze", "takže", "tam", "tato", "te", "tě", "tebe", "tebou", "ted",
    "teď", "tedy", "tema", "téma", "ten", "tento", "teto", "této", "tim",
    "tím", "timto", "tímto", "to", "tobě", "tohle", "toho", "tohoto", "tom",
    "tomto", "tomuto", "toto", "tu", "tuto", "tvuj", "tvůj", "ty", "tyto",
    "u", "uz", "už", "v", "vam", "vám", "vas", "vás", "vase", "vaše",
    "vaši", "ve", "vedle", "vice", "více", "vsak", "však", "vsechen",
    "všechen", "vsechno", "všechno", "vy", "z", "zda", "zde", "ze",
    "že", "ží",
}

# --- Helper functions ---
# helper function to decode the class from the filename
def decode_class_from_filename(filename: str):
    # convert the filename to lowercase and drop the extension
    name = Path(filename).stem.lower()
    # split into tokens on common separators
    tokens = set(re.split(r"[\s_\-.]+", name))
    # check if the filename contains both 'zadost' and 'gp'
    if "zadost" in tokens and "gp" in tokens:
        # return the class 'zadost'
        return "zadost"
    # loop through the class names
    for cls in CLASS_NAMES:
        # check if the class name is a token in the filename
        if cls in tokens:
            # return the class name
            return cls
    # return None if no class is found
    return None

# helper function to check if the file is a pure GP file
def is_pure_gp_file(filename: str) -> bool:
    # convert the filename to lowercase
    name = filename.lower()
    # check if the filename contains 'gp'
    if "gp" not in name:
        # return False if the filename does not contain 'gp'
        return False
    # loop through the class names
    for cls in CLASS_NAMES:
        # check if the class name is 'gp' or 'gnss'
        if cls in ("gp", "gnss"):
            # continue if the class name is 'gp' or 'gnss'
            continue
        # check if the class name is in the filename
        if cls in name:
            # return False if the class name is in the filename
            return False
    # return True if the filename is a pure GP file
    return True

# helper function to check if the page is a GNSS page
def page_is_gnss(page_text: str) -> bool:
    # check if the page text is empty
    if not page_text:
        # return False if the page text is empty
        return False
    # convert the page text to lowercase
    txt = page_text.lower()
    # check if any of the markers are in the page text
    return any(marker in txt for marker in GNSS_MARKERS)
    # return True if any of the markers are in the page text


# helper function to normalize the text
def normalize_text(text: str) -> str:
    # check if the text is empty
    if not text:
        # return empty string if the text is empty
        return ""
    # convert the text to lowercase
    text = text.lower()
    # create a list to store the normalized text
    out = []
    for ch in text:
        # get the category of the character
        cat = unicodedata.category(ch)
        # check if the character is a letter, digit, or whitespace
        if cat.startswith("L") or cat.startswith("N") or ch.isspace():
            out.append(ch)
        else:
            out.append(" ")
    # join the normalized text
    text = "".join(out)
    # split the text into tokens
    tokens = text.split()
    # filter the tokens
    tokens = [t for t in tokens if t not in CZECH_STOPWORDS and len(t) > 1]
    # join the tokens
    return " ".join(tokens)


# helper function to check if the page needs to be split
def page_needs_split(page) -> bool:
    # get the width and height of the page
    w, h = page.rect.width, page.rect.height
    # check if the page fits in portrait orientation
    fits_portrait = w <= A4_W + BUFFER and h <= A4_H + BUFFER
    # check if the page fits in landscape orientation
    fits_landscape = w <= A4_H + BUFFER and h <= A4_W + BUFFER
    # return True if the page needs to be split
    return not (fits_portrait or fits_landscape)


# helper function to split the page recursively 
def split_page_recursively(out_doc: fitz.Document, src_doc: fitz.Document, page_number: int, clip_rect: fitz.Rect):
    # get the width and height of the clip rectangle
    w = clip_rect.width
    # get the height of the clip rectangle
    h = clip_rect.height
    # check if the page fits in portrait orientation
    if (w <= A4_W + BUFFER and h <= A4_H + BUFFER) or (w <= A4_H + BUFFER and h <= A4_W + BUFFER):
        # create a new page
        new_page = out_doc.new_page(width=w, height=h)
        # show the page
        new_page.show_pdf_page(new_page.rect, src_doc, page_number, clip=clip_rect)
        # return
        return

    # check if the page fits in landscape orientation
    if w > A4_W + BUFFER and w >= h:
        # get the middle x coordinate
        mid_x = clip_rect.x0 + w / 2
        # create a left rectangle
        left = fitz.Rect(clip_rect.x0, clip_rect.y0, mid_x, clip_rect.y1)
        # create a right rectangle
        right = fitz.Rect(mid_x, clip_rect.y0, clip_rect.x1, clip_rect.y1)
        # split the left page recursively
        split_page_recursively(out_doc, src_doc, page_number, left)
        # split the right page recursively
        split_page_recursively(out_doc, src_doc, page_number, right)
    else:
        # get the middle y coordinate
        mid_y = clip_rect.y0 + h / 2
        # create a top rectangle
        top = fitz.Rect(clip_rect.x0, clip_rect.y0, clip_rect.x1, mid_y)
        # create a bottom rectangle
        bottom = fitz.Rect(clip_rect.x0, mid_y, clip_rect.x1, clip_rect.y1)
        # split the top page recursively
        split_page_recursively(out_doc, src_doc, page_number, top)
        # split the bottom page recursively
        split_page_recursively(out_doc, src_doc, page_number, bottom)

# helper function to extract the words from the PDF page
def extract_words_from_pdf_page(page) -> list:
    # get the words from the page
    words = page.get_text("words")
    # check if the page has no words
    if not words:
        return []
    # get the width and height of the page
    w, h = page.rect.width, page.rect.height
    # return the words with the bounding boxes
    return [(wd[4], wd[0], wd[1], wd[2], wd[3], w, h) for wd in words]


# helper function to OCR the words from the image
def ocr_words_from_image(pil_image, lang: str = "ces") -> list:
    # get the size of the image
    width, height = pil_image.size
    # OCR the words from the image
    data = pytesseract.image_to_data(pil_image, lang=lang, output_type=pytesseract.Output.DICT)
    # create a list to store the OCR words
    out = []
    # loop through the words
    # // this part of code was generated by Claude 4.7 opus model //
    for i, word in enumerate(data["text"]):
        # strip the word
        word = word.strip()
        # check if the word is empty
        if not word:
            continue
        # try to get the confidence of the word
        try:
            conf = int(data["conf"][i])
        except (ValueError, TypeError):
            conf = -1
        # check if the confidence is less than 0
        if conf < 0:
            # continue if the confidence is less than 0
            continue
        # get the coordinates of the word
        x, y = data["left"][i], data["top"][i]
        # get the width and height of the word
        w, h = data["width"][i], data["height"][i]
        # add the word to the list
        out.append((word, x, y, x + w, y + h, width, height))
    # // end of code generated by Claude 4.7 opus model //
    # return the list of OCR words
    return out


# helper function to normalize the box
def normalize_box_0_1000(x0, y0, x1, y1, width, height):
    # normalize the box
    return [
        max(0, min(1000, int(1000 * x0 / width))),
        max(0, min(1000, int(1000 * y0 / height))),
        max(0, min(1000, int(1000 * x1 / width))),
        max(0, min(1000, int(1000 * y1 / height))),
    ]

# class to embed the image
class YoloEmbedder:
    def __init__(self, weights: str = "yolo11n-cls.pt", device: str = None):
        # get the device
        self.device = device or ("cuda" if torch.cuda.is_available() else "cpu")
        # load the model
        self.model = YOLO(weights)
    
    # method to embed the image
    def embed(self, pil_image) -> np.ndarray:
        # embed the image
        results = self.model.embed(source=pil_image, device=self.device, verbose=False)
        # check if the results are empty
        if not results:
            # raise an error if the results are empty
            raise RuntimeError("model.embed returned empty result")
        # return the results
        return results[0].cpu().numpy()


# helper function to render the page to a PIL image
def render_page_to_pil(page, dpi: int = DPI) -> Image.Image:
    # get the zoom factor
    zoom = dpi / 72
    # create a matrix
    mat = fitz.Matrix(zoom, zoom)
    # get the pixmap
    pix = page.get_pixmap(matrix=mat)
    # return the PIL image
    return Image.frombytes("RGB", (pix.width, pix.height), pix.samples)

# helper function to split the PDF to A4 pages
def split_pdf_to_a4(pdf_path: Path) -> fitz.Document:
    # open the PDF
    src = fitz.open(str(pdf_path))
    # create a new PDF
    out = fitz.open()
    # loop through the pages
    for i, page in enumerate(src):
        # check if the page is rotated
        if page.rotation != 0:
            # set the page rotation to 0
            page.set_rotation(0)
        # check if the page needs to be split
        if not page_needs_split(page):
            # create a new page
            new_page = out.new_page(width=page.rect.width, height=page.rect.height)
            # show the page
            new_page.show_pdf_page(new_page.rect, src, i, clip=page.rect)
        else:
            # split the page recursively
            split_page_recursively(out, src, i, page.rect)
    # close the source PDF
    src.close()
    # return the new PDF
    return out


# helper function to process one PDF
def process_one_pdf(
    pdf_path: Path,
    author: str,
    project: str,
    out_root: Path,
    embedder: YoloEmbedder,
    cls_counter: dict,
    cls_cap: int,
    dpi: int,
    ocr_lang: str,
):
    # get the filename
    filename = pdf_path.name
    # get the file stem
    file_stem = pdf_path.stem
    # decode the class from the filename
    base_class = decode_class_from_filename(filename)
    # check if the class is None
    if base_class is None:
        # unrecognized filename; skip
        return []

    # check if the file is a pure GP file
    is_segmentation = is_pure_gp_file(filename)
    
    # create the output directories
    cls_img_dir = out_root / "classification" / "images"
    seg_img_dir = out_root / "segmentation" / "images"
    json_dir = out_root / "processed" / "jsons"
    cls_img_dir.mkdir(parents=True, exist_ok=True)
    seg_img_dir.mkdir(parents=True, exist_ok=True)
    json_dir.mkdir(parents=True, exist_ok=True)

    # Split into A4
    try:
        a4_doc = split_pdf_to_a4(pdf_path)
    except Exception as e:
        print(f"  [split error] {pdf_path}: {e}")
        return []

    rows = []
    # loop through the pages
    try:
        for page_idx, page in enumerate(a4_doc, start=1):
            # get the page number
            page_num = page_idx
            # get the image name
            img_name = f"{file_stem}_{page_num}.png"

            # extract the words from the PDF page
            pdf_words = extract_words_from_pdf_page(page)
            # check if the page has text objects
            had_text_object = len(pdf_words) > 0

            # render the page to a PIL image
            pil_img = render_page_to_pil(page, dpi=dpi)

            # get the words and words bounding boxes
            if had_text_object:
                # get the words from the PDF page
                words = [w[0] for w in pdf_words]
                # get the boxes from the PDF page
                boxes = [
                    normalize_box_0_1000(w[1], w[2], w[3], w[4], w[5], w[6])
                    for w in pdf_words
                ]
                # get the page text
                page_text = " ".join(words) 
                # get the width and height for the JSON
                width_for_json = int(page.rect.width)
                # get the height for the JSON
                height_for_json = int(page.rect.height)
            else:
                # OCR the words from the image
                ocr = ocr_words_from_image(pil_img, lang=ocr_lang)
                # get the words from the OCR
                words = [w[0] for w in ocr]
                # get the boxes from the OCR
                boxes = [
                    normalize_box_0_1000(w[1], w[2], w[3], w[4], w[5], w[6])
                    for w in ocr
                ]
                # get the page text
                page_text = " ".join(words)
                # get the width for the JSON
                width_for_json = pil_img.size[0]
                # get the height for the JSON
                height_for_json = pil_img.size[1]

            # get the page class
            page_class = base_class
            # only if the base class is 'prot' or 'zap' and the page text is GNSS, set the page class to 'gnss'
            if base_class in ("prot", "zap") and page_is_gnss(page_text):
                page_class = "gnss"

            # classification cap
            key = (author, page_class)
            under_cap = cls_counter.get(key, 0) < cls_cap

            if under_cap:
                # save the JSON
                # // this part of code was modified by Claude 4.7 opus model //
                json_path = json_dir / f"{file_stem}_{page_num}.json"
                with json_path.open("w", encoding="utf-8") as f:
                    json.dump({
                        "image_path": img_name,
                        "label": page_class,
                        "words": words,
                        "boxes": boxes,
                        "width": width_for_json,
                        "height": height_for_json,
                        "source_pdf": str(pdf_path),
                        "author": author,
                        "project": project,
                        "had_text_object": had_text_object,
                    }, f, ensure_ascii=False)


                # save the image to the classification folder
                pil_img.save(cls_img_dir / img_name, format="PNG")
                # embed the image
                try:
                    emb = embedder.embed(pil_img)
                    # convert the embedding to a JSON string
                    emb_str = json.dumps(emb.tolist())
                except Exception as e:
                    # embed error
                    print(f"  [embed error] {pdf_path}#{page_num}: {e}")
                    emb_str = ""
                # add the row to the list
                rows.append({
                    "name": img_name,
                    "class": page_class,
                    "text": normalize_text(page_text),
                    "embedding": emb_str,
                    "author": author,
                    "project": project,
                })
                cls_counter[key] = cls_counter.get(key, 0) + 1
                # // end of code modified by Claude 4.7 opus model //

            # Save image to segmentation folder if pure-gp file
            if is_segmentation:
                pil_img.save(seg_img_dir / img_name, format="PNG")
    # close the A4 document
    finally:
        a4_doc.close()

    # return the rows
    return rows

# // this part of code was modified by Claude 4.7 opus model //
# helper function to load the checkpoint
def load_checkpoint(ckpt_path: Path) -> set:
    # check if the checkpoint file exists
    if not ckpt_path.exists():
        # return an empty set if the checkpoint file does not exist
        return set()
    # load the checkpoint
    with ckpt_path.open("r", encoding="utf-8") as f:
        # return the set of checkpoint keys
        return {line.strip() for line in f if line.strip()}


# helper function to append the checkpoint
def append_checkpoint(ckpt_path: Path, key: str):
    # append the checkpoint key to the checkpoint file
    with ckpt_path.open("a", encoding="utf-8") as f:
        # write the checkpoint key to the checkpoint file
        f.write(key + "\n")
# // end of code modified by Claude 4.7 opus model //


# helper function to append the CSV rows
def append_csv_rows(csv_path: Path, rows: list, fieldnames: list):
    # check if the CSV file exists
    write_header = not csv_path.exists()
    # open the CSV file
    with csv_path.open("a", encoding="utf-8", newline="") as f:
        # create a CSV writer
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        # check if the header should be written
        if write_header:
            # write the header
            writer.writeheader()
        # loop through the rows
        for r in rows:
            writer.writerow(r)


# --- 1. Create output directories ---
# create the output directories
(OUT_ROOT / "classification" / "images").mkdir(parents=True, exist_ok=True)
(OUT_ROOT / "segmentation" / "images").mkdir(parents=True, exist_ok=True)
(OUT_ROOT / "processed" / "jsons").mkdir(parents=True, exist_ok=True)
(OUT_ROOT / "checkpoints").mkdir(parents=True, exist_ok=True)

csv_path = OUT_ROOT / "classification" / "dataset.csv"
ckpt_path = OUT_ROOT / "checkpoints" / "processed_files.txt"
label_map_path = OUT_ROOT / "processed" / "label_map.json"

# --- 2. Write label map ---
# create the label map
label2id = {name: i for i, name in enumerate(CLASS_NAMES)}
# write the label map to a JSON file
with label_map_path.open("w", encoding="utf-8") as f:
    json.dump({"label2id": label2id,
               "id2label": {i: name for name, i in label2id.items()}},
              f, ensure_ascii=False, indent=2)

# --- 3. Load the checkpoint ---
# load the checkpoint
processed = load_checkpoint(ckpt_path)
# print the number of processed files
print(f"Resuming: {len(processed)} files already processed")

# create the embedder
embedder = YoloEmbedder(weights=YOLO_WEIGHTS)

# rebuild cls_counter from existing CSV so the cap persists across resumes
cls_counter = {}
# load the counter from the CSV file
if csv_path.exists():
    # open the CSV file
    with csv_path.open("r", encoding="utf-8", newline="") as f:
        # create a CSV reader
        reader = csv.DictReader(f)
        # loop through the rows
        for row in reader:
            # get the key
            key = (row["author"], row["class"])
            # increment the counter
            cls_counter[key] = cls_counter.get(key, 0) + 1
    # print the number of loaded counter keys
    print(f"Loaded counter from CSV: {len(cls_counter)} (author, class) keys")

fieldnames = ["name", "class", "text", "embedding", "author", "project"]

# --- 4. Process the PDFs ---
# get the authors
authors = sorted([p for p in RAW_ROOT.iterdir() if p.is_dir()])
# create a list to store the PDF tasks
pdf_tasks = []
# loop through the authors
for author_dir in authors:
    # loop through the project directories
    for project_dir in sorted(author_dir.iterdir()):
        # check if the project directory is a directory
        if not project_dir.is_dir():
            continue
        # loop through the PDFs
        for pdf in sorted(project_dir.glob("*.pdf")):
            # add the PDF task to the list
            pdf_tasks.append((author_dir.name, project_dir.name, pdf))

# print the total number of PDFs found
print(f"Total PDFs found: {len(pdf_tasks)}")

# loop through the PDF tasks
for author, project, pdf in tqdm(pdf_tasks, desc="PDFs"):
    # get the checkpoint key
    ckpt_key = f"{author}/{project}/{pdf.name}"
    if ckpt_key in processed:
        # continue if the checkpoint key is in the processed set
        continue
    # try to process the PDF
    try:
        # process the PDF
        rows = process_one_pdf(
            pdf_path=pdf,
            author=author,
            project=project,
            out_root=OUT_ROOT,
            embedder=embedder,
            cls_counter=cls_counter,
            cls_cap=CLS_CAP,
            dpi=DPI,
            ocr_lang=OCR_LANG,
        )
        # check if the rows are not empty
        if rows:
            # append the rows to the CSV file
            append_csv_rows(csv_path, rows, fieldnames)
        # append the checkpoint key to the checkpoint file
        append_checkpoint(ckpt_path, ckpt_key)
    # except exception
    except Exception as e:
        print(f"[error] {ckpt_key}: {e}")

# print a message that the processing is done
print("\nDone.")
# print the CSV path
print(f"CSV: {csv_path}")
# print a message that the class counts are being printed
print("Class counts:")
# create a dictionary to store the class counts
counts = {}
# loop through the class counts
for (_, cls), n in cls_counter.items():
    counts[cls] = counts.get(cls, 0) + n
# loop through the class counts
for cls, n in sorted(counts.items()):
    print(f"  {cls}: {n}")