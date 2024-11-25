#pragma once

#include <wx/wx.h>

namespace client
{

    /**
     * @brief The main application class
     *
     */
    class Dominion : public wxApp
    {
    public:
        bool OnInit() final;
    };

} // namespace client
