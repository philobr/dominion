#include <uiElements/image_map.h>

#include <shared/utils/logger.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

namespace client
{
    wxImage ImageMap::getImage(wxString name)
    {
        wxString path = wxString("assets") + wxFileName::GetPathSeparator() + name + wxString(".png");
        if ( _images.find(path) == _images.end() ) {
            wxImage image;
            if ( !image.LoadFile(path, wxBITMAP_TYPE_PNG) ) {
                wxMessageBox("Could not load file: " + path, "File error", wxICON_ERROR);
                LOG(ERROR) << "Could not load file: " << path;
                throw std::runtime_error("Could not load file: " + path);
            }
            _images[name] = image;
            LOG(DEBUG) << "we have: " << _images.size() << " images loaded";
        }
        return _images[name];
    }

    wxImage ImageMap::getGreyImage(wxString name)
    {
        if ( _grey_images.find(name) == _grey_images.end() ) {
            wxImage image = getImage(name);
            _grey_images[name] = image.ConvertToGreyscale();
        }
        return _grey_images[name];
    }

    void ImageMap::addImage(wxString path, wxImage image) { _images[path] = image; }
} // namespace client
