import argparse
import os
from pypdf import PdfReader, PdfWriter, Transformation,PaperSize
from pypdf.generic import RectangleObject

from pdf2image import convert_from_path
from PIL import Image
import numpy as np

from core_utils.range_utils import parse_ranges

import numpy as np

from pathlib import Path

def make_linear_transform(A: np.ndarray, B: np.ndarray):
    """
    Build a linear operator T such that  T(A) = B  and
        ||T(X)||_F = (||B||_F / ||A||_F) · ||X||_F   for every 2×2 matrix X.
    
    Returns
    -------
    T : callable
        A function T(X) → 2×2 numpy array.
    """
    # ---------- 1. housekeeping ----------
    A = np.asarray(A, dtype=float).reshape(2, 2)
    B = np.asarray(B, dtype=float).reshape(2, 2)

    norm_A = np.linalg.norm(A, "fro")
    norm_B = np.linalg.norm(B, "fro")
    if norm_A == 0 or norm_B == 0:
        raise ValueError("Both A and B must be non-zero.")

    lam = norm_B / norm_A            # global scale factor λ

    # Helper to stack entries column-wise: vec[[a b],[c d]] = [a, c, b, d]
    def vec(M: np.ndarray) -> np.ndarray:
        return M.T.flatten()[[0, 2, 1, 3]]  # (a, c, b, d)

    def unvec(v: np.ndarray) -> np.ndarray:
        return np.array([[v[0], v[2]],
                         [v[1], v[3]]])

    # ---------- 2. orthogonal part (Householder reflection) ----------
    u = vec(A) / norm_A
    v = vec(B) / norm_B

    # Householder that maps u ↦ v
    if np.allclose(u, -v):
        # Choose any unit q ⟂ u
        q = np.eye(4)[np.argmax(np.abs(u))]   # basis vector farthest from u
        q -= q @ u * u
        q /= np.linalg.norm(q)
        R = np.eye(4) - 2 * np.outer(q, q)
    else:
        w = u - v
        R = np.eye(4) - 2 * np.outer(w, w) / (w @ w)

    # ---------- 3. the linear map ----------
    def T(X: np.ndarray) -> np.ndarray:
        x = vec(np.asarray(X, dtype=float).reshape(2, 2))
        tx = lam * R @ x
        return unvec(tx)

    return T

def scale_page(page, image):
    screen = image.convert("L")

    # Apply threshold
    threshold = 128

    # Convert to NumPy array
    arr = np.array(screen)

    # Apply threshold using NumPy (much faster on large images)
    binary_arr = np.where(arr < threshold, 255, 0).astype(np.uint8)

    ys, xs = np.nonzero(binary_arr)

    CropVector = [[ys.min(), ys.max()], [xs.min(), xs.max()]]

    A = [[0, arr.shape[0]], [0, arr.shape[1]]]

    mb = np.array(page.mediabox).reshape(2,2).transpose()

    scale_transformer = make_linear_transform(A, mb)

 
    scaled_rec = scale_transformer(CropVector).transpose()

    scaled_cropbox = scaled_rec.flatten().round()

    (top, right, left, bottom) = scaled_cropbox
    
    mb = page.mediabox

    cropBx = (left, mb.top-bottom, right, mb.top-top)


    padding = (-30, -30, 30, 30) 

    page.cropbox = RectangleObject(np.add(cropBx, padding))


    return page


def scale_doc(path, page_range=None):
    reader = PdfReader(path)
    writer = PdfWriter()

    images = convert_from_path(path)

    pages_to_process = range(0, len(reader.pages)) if page_range is None else sorted(parse_ranges(page_range))

    print(pages_to_process)

    for i in pages_to_process:
        print(i)
        page = reader.pages[i-1]
        srcpage = scale_page(page, images[i])
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
    
    parser.add_argument('path')           # positional argument
    parser.add_argument('-r', '--range')      # option that takes a value
    
    args = parser.parse_args()

    scale_doc(args.path, args.range)
    


if __name__ == "__main__":
    main()