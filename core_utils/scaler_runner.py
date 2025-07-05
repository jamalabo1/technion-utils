import os
import sys
import subprocess

def run_scaler(pdf_path: str, pages_arg: str) -> None:
    # Determine absolute path to scaler.py
    script_dir = os.path.dirname(__file__)
    scaler_path = os.path.join(script_dir, os.pardir, "scaler.py")
    scaler_path = os.path.abspath(scaler_path)

    cmd = [
        sys.executable,
        scaler_path,
        pdf_path,
        "-r",
        pages_arg
    ]
    print(cmd)
    # Force working directory so scaler.py can do relative imports if needed
    subprocess.run(cmd, check=True, cwd=os.path.dirname(scaler_path))