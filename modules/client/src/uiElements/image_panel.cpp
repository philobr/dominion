
#include <cstddef>
#include <shared/utils/logger.h>
#include <uiElements/image_panel.h>
#include <wx/filename.h>
#include <wx/image.h>

namespace client
{
    ImagePanel::ImagePanel(wxWindow *parent, wxString asset_name, wxBitmapType format, wxPoint position, wxSize size,
                           double rotation) :
        wxPanel(parent, wxID_ANY, position, size)
    {
        wxString file = wxString("assets") + wxFileName::GetPathSeparator() + "color" + wxFileName::GetPathSeparator() +
                asset_name;
        if ( !wxFileExists(file) ) {
            wxMessageBox("Could not find file: " + file, "File error", wxICON_ERROR);
            LOG(ERROR) << "Could not find file: " << file;
            throw std::runtime_error("Could not find file: " + file);
        }

        if ( !this->_image.LoadFile(file, format) ) {
            wxMessageBox("Could not load file: " + file, "File error", wxICON_ERROR);
            LOG(ERROR) << "Could not load file: " << file;
            throw std::runtime_error("Could not load file: " + file);
        }

        this->_rotation = rotation;

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

    void ImagePanel::makeGrey()
    {
        _image = _image.ConvertToGreyscale();
        Refresh();
    }
} // namespace client
