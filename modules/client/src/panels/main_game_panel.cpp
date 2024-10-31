#include "main_game_panel.h"
#include <wx/wx.h>


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
  wxBoxSizer* mainLayout = new wxBoxSizer(wxVERTICAL);
  auto * textbox = buildStaticText("Welcome to this beautiful game of dominion",
                                   wxPoint(480, 200), wxSize(300, 20),
                                   wxALIGN_CENTER, true);
  this->SetSizer(mainLayout);
}


wxStaticText* MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* staticText = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if(bold) {
        wxFont font = staticText->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        staticText->SetFont(font);
    }
    return staticText;
}


wxSize MainGamePanel::getBoundsOfRotatedSquare(double edgeLength, double rotationAngle) {
    double newEdgeLength = this->getEdgeLengthOfRotatedSquare(edgeLength, rotationAngle);
    return wxSize(newEdgeLength, newEdgeLength);
}


double MainGamePanel::getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle) {
    return originalEdgeLength * (abs(sin(rotationAngle)) + abs(cos(rotationAngle)));
}


wxPoint MainGamePanel::getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle) {
    return wxPoint((int) (sin(angle) * horizontalRadius), (int) (cos(angle) * verticalRadius));
}
