
#include <cstddef>
#include <shared/utils/logger.h>
#include <uiElements/image_map.h>
#include <uiElements/image_panel.h>
#include <wx/filename.h>
#include <wx/image.h>
#include "dominion.h"

namespace client
{
    ImagePanel::ImagePanel(wxWindow *parent, wxString asset_name, wxBitmapType /*format*/, wxPoint position,
                           wxSize size, double rotation) :
        wxPanel(parent, wxID_ANY, position, size),
        _rotation(rotation)
    {

        // this either gets the image from the map or loads it from the file system
        // if it hasn't been used before
        this->_image = wxGetApp().getImageMap().getImage(asset_name);

        this->Bind(wxEVT_PAINT, &ImagePanel::paintEvent, this);
        this->Bind(wxEVT_SIZE, &ImagePanel::onSize, this);
    }


    void ImagePanel::paintEvent(const wxPaintEvent & /*event*/)
    {
        // this code is called when the system requests this panel to be redrawn.

        if ( !_image.IsOk() ) {
            LOG(WARN) << "Image is not ok";
            return;
        }

        wxPaintDC deviceContext = wxPaintDC(this);

        int newWidth;
        int newHeight;
        deviceContext.GetSize(&newWidth, &newHeight);

        if ( newWidth != this->_width || newHeight != this->_height ) {

            wxImage transformed;

            if ( this->_rotation == 0.0 ) {
                transformed = _image.Scale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);

            } else {
                wxPoint centerOfRotation = wxPoint(_image.GetWidth() / 2, _image.GetHeight() / 2);
                transformed = _image.Rotate(this->_rotation, centerOfRotation, true);
                transformed = transformed.Scale(newWidth, newHeight, wxIMAGE_QUALITY_BILINEAR);
            }
            this->_bitmap = wxBitmap(transformed);
            this->_width = transformed.GetWidth();
            this->_height = transformed.GetHeight();

            deviceContext.DrawBitmap(this->_bitmap, 0, 0, false);
        } else {
            deviceContext.DrawBitmap(this->_bitmap, 0, 0, false);
        }
    }


    void ImagePanel::onSize(wxSizeEvent &event)
    {
        // when the user resizes this panel, the image should redraw itself
        Refresh();

        // skip any other effects of this event.
        event.Skip();
    }

    void ImagePanel::makeGrey(wxString asset_name)
    {
        _image = wxGetApp().getImageMap().getGreyImage(asset_name);
        Refresh();
    }
} // namespace client
