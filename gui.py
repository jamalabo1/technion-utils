import tkinter as tk
from tkinter import filedialog, messagebox, ttk

from scaler import scale_doc
from pypdf import PdfReader


def main():
    # Initialize hidden root
    root = tk.Tk()
    root.withdraw()

    # Select PDF file
    filepath = filedialog.askopenfilename(
        title="Select PDF",
        filetypes=[("PDF files", "*.pdf")]
    )
    if not filepath:
        return

    # Read PDF to determine number of pages
    try:
        reader = PdfReader(filepath)
    except Exception as e:
        messagebox.showerror("Error", f"Failed to read PDF:\n{e}")
        return
    num_pages = len(reader.pages)

    # Create selection window
    sel_win = tk.Toplevel()
    sel_win.title("Select Pages to Scale to A4")

    # Scrollable frame for checkboxes
    canvas = tk.Canvas(sel_win)
    frame = ttk.Frame(canvas)
    vsb = ttk.Scrollbar(sel_win, orient="vertical", command=canvas.yview)
    canvas.configure(yscrollcommand=vsb.set)
    vsb.pack(side="right", fill="y")
    canvas.pack(side="left", fill="both", expand=True)
    canvas.create_window((0,0), window=frame, anchor="nw")
    frame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

    # Checkbuttons for each page
    vars = []
    for i in range(1, num_pages+1):
        var = tk.IntVar(value=0)
        cb = ttk.Checkbutton(frame, text=f"Page {i}", variable=var)
        cb.pack(anchor="w", padx=5, pady=2)
        vars.append(var)

    # Button to scale selected pages
    def on_submit():
        selected = [str(i+1) for i, v in enumerate(vars) if v.get()]
        if not selected:
            messagebox.showwarning("No Pages", "Please select at least one page.")
            return
        page_range = ",".join(selected)
        try:
            scale_doc(filepath, page_range)
            messagebox.showinfo(
                "Success",
                f"Scaled PDF saved as: {filepath.rsplit('.',1)[0]}.scaled.pdf"
            )
            sel_win.destroy()
            root.quit()
        except Exception as e:
            messagebox.showerror("Error", str(e))

    btn = ttk.Button(sel_win, text="Scale Selected Pages", command=on_submit)
    btn.pack(pady=10)

    sel_win.mainloop()


if __name__ == "__main__":
    main()