import wx
from PIL import Image
from .hover_preview import HoverPreviewHandler

class ThumbnailPanel(wx.ScrolledWindow):
    def __init__(self, parent, on_selection):
        super().__init__(parent)
        self.on_selection = on_selection
        self.selected = set()
        self.btns = []

        # WrapSizer so thumbnails flow to fit available width
        self.sizer = wx.WrapSizer(wx.HORIZONTAL)
        self.SetSizer(self.sizer)
        self.SetScrollRate(10, 10)

        # Hover-preview logic is delegated to HoverPreviewHandler
        self.hover_handler = HoverPreviewHandler(self)

    def display(self, pil_pages: list[Image]):
        # Clear out old thumbnails
        for btn in self.btns:
            self.sizer.Detach(btn)
            btn.Destroy()
        self.btns.clear()
        self.selected.clear()

        # Create new thumbnail buttons
        for idx, img in enumerate(pil_pages, start=1):
            # High‐quality thumbnail
            thumb = img.copy()
            thumb.thumbnail((240, 320), Image.LANCZOS)
            w, h = thumb.size
            bmp = wx.Bitmap.FromBuffer(w, h, thumb.tobytes())

            btn = wx.BitmapToggleButton(self, idx, bmp)
            btn.Bind(wx.EVT_TOGGLEBUTTON, self._on_toggle)
            btn.full_img = img                            # store full page
            self.hover_handler.bind(btn)                  # attach hover logic
            # btn.Bind(wx.EVT_RIGHT_DOWN, self.hover_handler.on_btn_right)
            btn.page_num = idx

            self.btns.append(btn)
            self.sizer.Add(btn, 0, wx.ALL, 5)

        self.Layout()
        self.FitInside()

    def _on_toggle(self, evt):
        btn = evt.GetEventObject()
        if btn.GetValue():
            self.selected.add(btn.page_num)
            btn.SetBackgroundColour(wx.Colour(0, 0, 255))
        else:
            self.selected.discard(btn.page_num)
            btn.SetBackgroundColour(wx.NullColour)
        btn.Refresh()
        self.on_selection(self.selected)