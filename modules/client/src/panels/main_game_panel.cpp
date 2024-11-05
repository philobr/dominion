#include "main_game_panel.h"
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
  wxBoxSizer* mainLayout = new wxBoxSizer(wxVERTICAL);
  auto * textbox = buildStaticText("Welcome to this beautiful game of dominion",
                                   wxPoint(480, 200), wxSize(300, 20),
                                   wxALIGN_CENTER, true);

  wxBoxSizer* playerLayout = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* enemiesLayout = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* playingFieldLayout = new wxBoxSizer(wxHORIZONTAL);
  wxPanel* playingField = new wxPanel(this, wxID_ANY, wxDefaultPosition);
  playingField->SetBackgroundColour(wxColour(0, 255, 0));
  wxPanel* enemies = new wxPanel(this, wxID_ANY, wxDefaultPosition);
  enemies->SetBackgroundColour(wxColour(255, 0, 0));

  mainLayout->Add(enemiesLayout, 1, wxEXPAND, 10);
  mainLayout->Add(playingFieldLayout, 4, wxEXPAND, 10);
  mainLayout->Add(playerLayout, 2, wxEXPAND, 10);
  playerLayout->Add(textbox, 0, wxALIGN_CENTER | wxALL, 10);
  enemiesLayout->Add(enemies, 1, wxEXPAND, 10);
  playingFieldLayout->Add(playingField, 1, wxEXPAND, 10);
  this->SetSizer(mainLayout, wxEXPAND);
}


wxStaticText* MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* staticText = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    staticText->SetForegroundColour(wxColour(0, 0, 0));
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
