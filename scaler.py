import argparse
import os

import numpy as np
from pdf2image import convert_from_path
from pypdf import PdfReader, PdfWriter, PaperSize
from pypdf.generic import RectangleObject

from core_utils.range_utils import parse_ranges


def scale_page(page, image):
    # Convert page image to grayscale
    gray = image.convert("L")
    # Convert to NumPy array
    arr = np.array(gray)
    # Threshold to identify content pixels
    threshold = 128
    # Create binary mask where True indicates content (dark) pixels
    mask = arr < threshold
    coords = np.argwhere(mask)
    if coords.size:
        # Separate x (column) and y (row) indices
        ys = coords[:, 1]
        xs = coords[:, 0]
        min_x, max_x = ys.min(), ys.max()
        min_y, max_y = xs.min(), xs.max()

        # PDF page dimensions (points at 72 DPI)
        page_height = float(page.mediabox.height)

        left = min_x
        right = max_x
        lower = page_height - max_y
        upper = page_height - min_y

        padding = (-20, -20, 20, 20)
        page.cropbox = RectangleObject(np.add([left, lower, right, upper], padding))
    return page


def scale_doc(path, page_range=None):
    reader = PdfReader(path)
    writer = PdfWriter()

    images = convert_from_path(path, dpi=72)

    pages_to_process = range(0, len(reader.pages)) if page_range is None else sorted(parse_ranges(page_range))

    print(pages_to_process)

    for i in pages_to_process:
        print(i)
        page = reader.pages[i - 1]
        srcpage = scale_page(page, images[i - 1])
        srcpage.scale_to(PaperSize.A4.width, PaperSize.A4.height)
        writer.add_page(srcpage)

    filename = os.path.basename(path)

    print(filename)

    new_filename = "{0}.scaled.pdf".format('.'.join(filename.split('.')[:-1]))

    writer.write(os.path.join(os.path.dirname(path), new_filename))


def main():
    parser = argparse.ArgumentParser(
        prog='Scaler',
        description='scaled pdf documents for printing')

    parser.add_argument('path')  # positional argument
    parser.add_argument('-r', '--range')  # option that takes a value

    args = parser.parse_args()

    scale_doc(args.path, args.range)


if __name__ == "__main__":
    main()
