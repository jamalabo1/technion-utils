import wx
from PIL import Image

class HoverPreviewHandler:
    def __init__(self, parent):
        self.parent = parent
        self.preview_popup = None
        self.hover_btn = None
        self.current_idx = None

    def bind(self, btn):
        """Attach right-click event to a thumbnail button."""
        btn.Bind(wx.EVT_RIGHT_DOWN, self.toggle)

    def toggle(self, evt):
        self.hide_preview()

        btn = evt.GetEventObject()

        self.hover_btn = btn

        if self.current_idx != self.idx(btn):
            self.show_preview()
        else:
            self.clear_preview()


  
    def idx(self, btn):
        return btn.page_num

    def show_preview(self):
        """Display a transient popup with a high‐res thumbnail to the right of the button."""
        self.current_idx = self.idx(self.hover_btn)

        # Prepare image
        img = self.hover_btn.full_img.copy()
        img = img.resize((600, 800), Image.LANCZOS)
        w, h = img.size
        bmp = wx.Bitmap.FromBuffer(w, h, img.convert("RGB").tobytes())

        # Create popup
        popup = wx.PopupTransientWindow(self.parent, wx.BORDER_SIMPLE)
        panel = wx.Panel(popup)
        st = wx.StaticBitmap(panel, bitmap=bmp)
        bs = wx.BoxSizer()
        bs.Add(st, 0, wx.ALL, 1)
        panel.SetSizer(bs)
        panel.Fit()
        popup.SetClientSize(panel.GetSize())

        # Position to the right of the hovered button
        btn = self.hover_btn
        screen_pos = btn.ClientToScreen((0, 0))
        btn_w, btn_h = btn.GetSize()
        new_pos = (screen_pos.x + btn_w + 5, screen_pos.y)
        popup.Position(new_pos, (0, 0))
        popup.Show(True)

        self.preview_popup = popup

    def hide_preview(self):
        """Destroy the popup if it exists."""
        if self.preview_popup:
            self.preview_popup.Destroy()
    def clear_preview(self):
        self.preview_popup = None
        self.current_idx = None
        self.hover_btn = None 