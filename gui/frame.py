
import wx
from core_utils import load_pages, format_ranges, parse_ranges, run_scaler

from .thumbnail_panel import ThumbnailPanel

class PDFScalerFrame(wx.Frame):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.pdf_path = None
        self.pages = []
        self.selected = set()

        # --- UI layout ---
        panel = wx.Panel(self)
        vbox = wx.BoxSizer(wx.VERTICAL)

        # File picker + Load button
        hfile = wx.BoxSizer(wx.HORIZONTAL)
        self.file_txt = wx.TextCtrl(panel, style=wx.TE_READONLY)
        btn_load = wx.Button(panel, label="Load PDF")
        btn_load.Bind(wx.EVT_BUTTON, self.on_load)
        hfile.Add(self.file_txt, 1, wx.EXPAND|wx.ALL, 5)
        hfile.Add(btn_load, 0, wx.ALL, 5)
        vbox.Add(hfile, 0, wx.EXPAND)

        # Thumbnail area
        self.thumb_panel = ThumbnailPanel(panel, on_selection=self.on_selection_change)
        vbox.Add(self.thumb_panel, 1, wx.EXPAND|wx.ALL, 5)

        # Range text + Scale button
        hrange = wx.BoxSizer(wx.HORIZONTAL)
        self.range_txt = wx.TextCtrl(panel)
        self.range_txt.Bind(wx.EVT_KILL_FOCUS, self.on_range_text_change)
        self.scale_btn = wx.Button(panel, label="Scale")
        self.scale_btn.Bind(wx.EVT_BUTTON, self.on_scale)
        hrange.Add(wx.StaticText(panel, label="Pages:"), 0, wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, 5)
        hrange.Add(self.range_txt, 1, wx.EXPAND|wx.RIGHT, 5)
        hrange.Add(self.scale_btn, 0)
        vbox.Add(hrange, 0, wx.EXPAND|wx.ALL, 5)

        panel.SetSizer(vbox)
        self.SetSize((240*4 + 90, 800))

    def on_load(self, event):
        dlg = wx.FileDialog(self, message="Choose a PDF file",
                            style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST)
        if dlg.ShowModal() == wx.ID_OK:
            self.pdf_path = dlg.GetPath()
            self.file_txt.SetValue(self.pdf_path)
            self.pages = load_pages(self.pdf_path)
            self.thumb_panel.display(self.pages)
            self.selected.clear()
            self.range_txt.SetValue("")
        dlg.Destroy()

    def on_selection_change(self, selected_pages):
        """Callback from ThumbnailPanel when selection changes."""
        self.selected = selected_pages
        self.range_txt.SetValue(format_ranges(self.selected))

    def on_scale(self, event):
        if not self.selected:
            wx.MessageBox("Please select at least one page.", "No Selection", wx.ICON_WARNING)
            return

        # Disable button + show busy
        self.scale_btn.Disable()
        busy = wx.BusyInfo("Scaling PDF, please wait...", self)
        wx.Yield()

        pages_arg = self.range_txt.GetValue().strip().replace(" ", "")
        try:
            # Validate format before running
            parse_ranges(pages_arg)
        except ValueError as e:
            wx.MessageBox(str(e), "Invalid Range", wx.ICON_ERROR)
            busy = None
            self.scale_btn.Enable()
            return

        try:
            run_scaler(self.pdf_path, pages_arg)
        except Exception as e:
            wx.MessageBox(f"Scaler failed:\n{e}", "Error", wx.ICON_ERROR)
            return
        finally:
            # cleanup
            if busy:
                del busy
            self.scale_btn.Enable()

        wx.MessageBox("Scaled PDF", "Done", wx.ICON_INFORMATION)
        # self.Close()
    def on_range_text_change(self, event):
        # Parse the textual ranges and update the thumbnail selection
        pages_arg = self.range_txt.GetValue().strip().replace(" ", "")
        try:
            pages = parse_ranges(pages_arg)
        except ValueError:
        # Invalid input: ignore until valid
            return
        self.selected = set(pages)
        # Programmatically update thumbnails to match the text
        self.thumb_panel.select_pages(self.selected)
        event.Skip()