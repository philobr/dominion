#pragma once

#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{

    /**
     * @brief The utility panel that displays an image
     *
     */
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

        /**
         * @brief Callback to repaint the image
         *
         * @param event
         */
        void paintEvent(const wxPaintEvent &event);

        /**
         * @brief Callback to resize the image
         *
         * @param event
         */
        void onSize(wxSizeEvent &event);

        /**
         * @brief Make the image grey
         *
         */
        void makeGrey();
    };

} // namespace client
