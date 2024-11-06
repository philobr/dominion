#pragma once

#include <wx/wx.h>

namespace client
{

    class MainGamePanel : public wxPanel
    {

    public:
        explicit MainGamePanel(wxWindow *parent);


    private:
        // define key constant layout values
        wxSize const panelSize = wxSize(960, 960); // also set in the constructor implementation
        wxPoint const tableCenter = wxPoint(480, 300);
        wxSize const cardSize = wxSize(80, 124);

        double const otherPlayerHandSize = 160.0;
        double const otherPlayerHandDistanceFromCenter = 180.0;
        double const otherPlayerLabelDistanceFromCenter = 275.0;

        wxPoint const discardPileOffset = wxPoint(-84, -42);
        wxPoint const drawPileOffset = wxPoint(4, -42);
        wxPoint const turnIndicatorOffset = wxPoint(-100, 98);

        double const twoPi = 6.28318530718;
    };

} // namespace client
