import wx
from .frame import PDFScalerFrame

def main():
    app = wx.App(False)
    frame = PDFScalerFrame(None, title="PDF Scaler")
    frame.Show()
    app.MainLoop()

if __name__ == "__main__":
    main()