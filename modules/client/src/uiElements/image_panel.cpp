#include <shared/utils/logger.h>
#include <uiElements/image_panel.h>

namespace client
{

    ImagePanel::ImagePanel(wxWindow *parent, wxString file, wxBitmapType format, wxPoint position, wxSize size,
                           double rotation) :
        wxPanel(parent, wxID_ANY, position, size)
    {
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

        if ( !this->_image.IsOk() ) {
            return;
        }

        wxPaintDC deviceContext = wxPaintDC(this);

        int newWidth;
        int newHeight;
        deviceContext.GetSize(&newWidth, &newHeight);

        if ( newWidth != this->_width || newHeight != this->_height ) {

            wxImage transformed;

            if ( this->_rotation == 0.0 ) {
                transformed = this->_image.Scale(newWidth, newHeight, wxIMAGE_QUALITY_BILINEAR);

            } else {
                wxPoint centerOfRotation = wxPoint(this->_image.GetWidth() / 2, this->_image.GetHeight() / 2);
                transformed = this->_image.Rotate(this->_rotation, centerOfRotation, true);
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

} // namespace client
