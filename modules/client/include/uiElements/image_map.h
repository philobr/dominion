#pragma once

#include <map>
#include <string>
#include <wx/image.h>
#include <wx/string.h>

namespace client
{
    /**
     * @brief This class only contains a map of image paths and their corresponding imaages.
     * This will allow us to load images only once and then use them multiple times.
     */
    class ImageMap
    {
    public:
        ImageMap() : _images(), _grey_images() {}
        ~ImageMap() = default;

        /**
         * @brief Get the Image object and add it to the map if it hasn't been loaded before
         *
         * @param path
         * @return wxImage
         */
        wxImage getImage(wxString name);

        /**
         * @brief get greyscale image
         */
        wxImage getGreyImage(wxString name);

        /**
         * @brief add an image to the map
         */
        void addImage(wxString path, wxImage image);

    private:
        std::map<wxString, wxImage> _images;
        std::map<wxString, wxImage> _grey_images;
    };

} // namespace client
