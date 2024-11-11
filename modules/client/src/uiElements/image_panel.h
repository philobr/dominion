#pragma once

#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{

    // This class can be used to display an image. It can be scaled with parameter <size> and rotated with <rotation>
    // (in radian)
    class ImagePanel : public wxPanel
    {
        wxImage _image;
        wxBitmap _bitmap;

        double _rotation;

        int _width;
        int _height;

    public:
        ImagePanel(wxWindow *parent, wxString file, wxBitmapType format, wxPoint position = wxDefaultPosition,
                   wxSize size = wxDefaultSize, double rotation = 0.0);

        void paintEvent(const wxPaintEvent &event);
        void onSize(wxSizeEvent &event);
    };

} // namespace client
