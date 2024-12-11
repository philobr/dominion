#include <uiElements/image_map.h>

#include <shared/utils/logger.h>
#include <wx/msgdlg.h>

namespace client
{
    wxImage ImageMap::getImage(wxString path)
    {
        if ( _images.find(path) == _images.end() ) {
            wxImage image;
            if ( !image.LoadFile(path, wxBITMAP_TYPE_PNG) ) {
                wxMessageBox("Could not load file: " + path, "File error", wxICON_ERROR);
                LOG(ERROR) << "Could not load file: " << path;
                throw std::runtime_error("Could not load file: " + path);
            }
            _images[path] = image;
            LOG(DEBUG) << "we have: " << _images.size() << " images loaded";
        }
        return _images[path];
    }

    void ImageMap::addImage(wxString path, wxImage image) { _images[path] = image; }
} // namespace client
