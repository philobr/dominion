#pragma once

#include "image_panel.h"

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>


namespace client
{
    /**
     * @brief A utility panel that displays a pile of cards
     */
    class PilePanel : public wxPanel
    {
    public:
        PilePanel(wxWindow *parent, shared::Pile pile, wxSize size = wxDefaultSize, int padding = 0);

        shared::Pile getPile() const;

        /**
         * @brief make the pile clickable
         *
         * @tparam Functor the function to call when the pile is clicked
         *
         */
        template <typename Functor>
        void makeClickable(const wxEventTypeTag<wxMouseEvent> click_type, Functor f);

        /**
         * @brief Make the pile grey
         *
         */
        void makeGrey();

    private:
        shared::Pile Pile;
        ImagePanel *Image;
    };


    template <typename Functor>
    void PilePanel::makeClickable(const wxEventTypeTag<wxMouseEvent> click_type, Functor f)
    {
        Image->Bind(click_type, f);
    }
} // namespace client
