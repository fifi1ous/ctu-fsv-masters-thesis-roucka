import re
import os
import copy
import struct
from pypdf import PdfReader, PdfWriter
from pypdf.generic import RectangleObject
from pdf2image import convert_from_path


class ProcessPDF:

    def __init__(self, file_path: str):
        self._file_path = file_path
        self._min_version = (1, 0)
        self._features_found = []


    def convert_pdf_to_images(self, path_to_pdf: str | None = None, save_path: str | None = None, dpi: int = 400, binarize: bool = False) -> list[str]:
        """Convert PDF pages to PNG images and return a list of saved file paths."""
        target_path = path_to_pdf or self._file_path
        save_dir = save_path or os.path.dirname(target_path) or "."

        # Create directories if they don't exist
        if save_path:
            os.makedirs(save_path, exist_ok=True)

        try:
            pages = convert_from_path(target_path, dpi=dpi, use_cropbox=True)
        except Exception as e:
            # Fallback: high DPI can exhaust Poppler's memory on large/A0
            # cadastral plans. Retry at 200 DPI so we still produce output.
            pages = convert_from_path(target_path, dpi=200, use_cropbox=True)

        base_name = os.path.basename(target_path)
        name_only = os.path.splitext(base_name)[0]

        generated_files = []
        for i, page in enumerate(pages):
            output_file = os.path.join(save_dir, f"{name_only}_{i+1}.png")

            if binarize:
                gray = page.convert("L")
                threshold = self._otsu_threshold(gray)
                page = gray.point(lambda x, t=threshold: 255 if x > t else 0, mode='1')

            page.save(output_file, format="PNG")
            generated_files.append(output_file)

        return generated_files

    
    def get_page_rotations(self, path_to_pdf: str | None = None) -> list[int]:
        """Return a list of effective rotation angles (0, 90, 180, 270) for each page."""
        target_path = path_to_pdf or self._file_path
        reader = PdfReader(target_path)
        return [int(page.get('/Rotate') or 0) % 360 for page in reader.pages]

    def get_pdf_page_dimensions(self, path_to_pdf: str | None = None) -> list[tuple[float, float]]:
        """Return (width, height) in PDF points for each page, accounting for rotation."""
        target_path = path_to_pdf or self._file_path
        reader = PdfReader(target_path)
        dims = []
        for page in reader.pages:
            w, h = float(page.mediabox.width), float(page.mediabox.height)
            rotation = int(page.get('/Rotate') or 0) % 360
            if rotation in (90, 270):
                w, h = h, w
            dims.append((w, h))
        return dims

    def get_size_of_pdf_pages(self, path_to_pdf: str | None = None) -> list[str | None]:
        """Return a list of page sizes (e.g., 'A4', 'A3') for each page in the PDF."""
        sizes = []
        target_path = path_to_pdf or self._file_path
        reader = PdfReader(target_path)

        for page in reader.pages:
            w, h = float(page.mediabox.width), float(page.mediabox.height)
            sizes.append(self._pdf_size_name(w, h))

        return sizes

    @staticmethod
    def get_size_of_image(image_path: str) -> tuple[int, int]:
        """Return (width, height) of a PNG image by reading its header bytes."""
        with open(image_path, "rb") as f:
            f.read(16)  # Skip PNG signature (8 bytes) + IHDR chunk header (8 bytes)
            width = struct.unpack(">I", f.read(4))[0]
            height = struct.unpack(">I", f.read(4))[0]
        return width, height
    
    @staticmethod
    def normalized_to_pdf_points(
        page_width: float,
        page_height: float,
        nx1: float,
        ny1: float,
        nx2: float,
        ny2: float,
    ) -> tuple[float, float, float, float]:
        # Image coords are y-down (origin at top-left); PDF points are y-up
        # (origin at bottom-left), so the y axis must be flipped.
        x0 = nx1 * page_width
        x1 = nx2 * page_width
        y0 = (1 - ny2) * page_height
        y1 = (1 - ny1) * page_height
        return x0, y0, x1, y1

    @staticmethod
    def _visual_rect_to_unrotated(
        x0: float, y0: float, x1: float, y1: float,
        mediabox_w: float, mediabox_h: float, rotation: int,
    ) -> tuple[float, float, float, float]:
        # pypdf writes mediabox/cropbox in the page's *unrotated* coordinate
        # space, but bboxes from images/YOLO are in the *visual* (rotated)
        # space the user sees. Invert the page rotation so the rect lands
        # in the right place after the viewer re-applies /Rotate.
        rotation = rotation % 360
        if rotation == 0:
            return x0, y0, x1, y1
        if rotation == 90:
            return mediabox_w - y1, x0, mediabox_w - y0, x1
        if rotation == 180:
            return (
                mediabox_w - x1, mediabox_h - y1,
                mediabox_w - x0, mediabox_h - y0,
            )
        if rotation == 270:
            return y0, mediabox_h - x1, y1, mediabox_h - x0
        raise ValueError(f"Unsupported page rotation: {rotation}")

    def cropping_pdf(self, path_to_pdf: str | None = None, save_path: str | None = None, crop_box: list[tuple[int, float, float, float, float]] | None = None, name_of_file: str | None = None) -> PdfWriter:
        """Build a new PDF where each entry in ``crop_box`` produces one cropped page.

        """
        if not crop_box:
            return PdfWriter()

        target_path = path_to_pdf or self._file_path
        reader = PdfReader(target_path)
        writer = PdfWriter()

        for page_num, x0, y0, x1, y1 in crop_box:
            source_page = reader.pages[page_num]

            # Defensive: normalize ordering in case the caller passed an
            # inverted rect (e.g. YOLO emitting x1 < x0).
            x0, x1 = (x0, x1) if x0 <= x1 else (x1, x0)
            y0, y1 = (y0, y1) if y0 <= y1 else (y1, y0)

            mediabox_w = float(source_page.mediabox.width)
            mediabox_h = float(source_page.mediabox.height)
            rotation = int(source_page.get('/Rotate') or 0) % 360

            # Convert from visual (rotation-aware) coords to the page's
            # unrotated mediabox coordinate system, which is what pypdf
            # writes into ``mediabox``/``cropbox``.
            ux0, uy0, ux1, uy1 = self._visual_rect_to_unrotated(
                x0, y0, x1, y1, mediabox_w, mediabox_h, rotation,
            )

            # Clamp to the unrotated mediabox so a slightly out-of-range
            # bbox can't produce an oversized crop.
            ux0 = max(0.0, min(ux0, mediabox_w))
            ux1 = max(0.0, min(ux1, mediabox_w))
            uy0 = max(0.0, min(uy0, mediabox_h))
            uy1 = max(0.0, min(uy1, mediabox_h))

            # Re-order after clamping (no-op unless clamping collapsed the rect).
            ux0, ux1 = (ux0, ux1) if ux0 <= ux1 else (ux1, ux0)
            uy0, uy1 = (uy0, uy1) if uy0 <= uy1 else (uy1, uy0)

            new_page = copy.copy(source_page)
            rect = RectangleObject((ux0, uy0, ux1, uy1))
            new_page.mediabox = rect
            new_page.cropbox = rect
            writer.add_page(new_page)

        if save_path:
            base_name = os.path.basename(target_path)
            name, ext = os.path.splitext(base_name)
            output_file = os.path.join(save_path, f"{name_of_file or name}{ext}")

            # Create directories if they don't exist
            os.makedirs(save_path, exist_ok=True)
            
            with open(output_file, "wb") as f:
                writer.write(f)

        return writer
    
    def extract_pdf_pages(self, page_index: list[int], save_path: str | None = None, name_of_file: str | None = None) -> PdfWriter:
        reader = PdfReader(self._file_path)
        writer = PdfWriter()
        page_count = len(reader.pages)

        for i in page_index:
            if 0 <= i < page_count:
                writer.add_page(reader.pages[i])
            else:
                print(f"Warning: Page index {i} is out of bounds and was skipped.")

        if save_path:
            os.makedirs(save_path, exist_ok=True)
            base = name_of_file or os.path.splitext(os.path.basename(self._file_path))[0]
            if not base.lower().endswith('.pdf'):
                base += '.pdf'
            with open(os.path.join(save_path, base), "wb") as output_pdf:
                writer.write(output_pdf)

        return writer
