from pdf2image import convert_from_path
from PIL import Image

def load_pages(path: str, dpi: int = 100) -> list[Image.Image]:
    try:
        pages = convert_from_path(path, dpi=dpi)
    except Exception as e:
        raise RuntimeError(f"Failed to load PDF pages: {e}")
    return pages