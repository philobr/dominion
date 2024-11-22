#include "game_window.h"
#include <wx/wx.h>
#include <shared/utils/logger.h>

namespace client
{

    GameWindow::GameWindow(const wxString &title, const wxPoint &pos, const wxSize &size) :
        wxFrame(nullptr, wxID_ANY, title, pos, size), _mainLayout(new wxBoxSizer(wxVERTICAL)), _currentPanel(nullptr)
    {
        LOG(INFO) << "Called GameWindow::GameWindow()";
        // Set up layout that will contain and center all content
        wxBoxSizer *outerLayout = new wxBoxSizer(wxHORIZONTAL);
        outerLayout->Add(this->_mainLayout, 1, wxCENTER);
        this->SetSizerAndFit(outerLayout);

        // Set up status bar
        this->_statusBar = this->CreateStatusBar(1);

        // Set background
        wxColor lightBlue = wxColor(213, 231, 239);
        this->SetBackgroundColour(lightBlue);

        // Set the minimum size of the window. The user won't be able to resize the window to a size smaller than this
        this->SetMinSize(wxSize(1000, 720));
        LOG(INFO) << "Done with GameWindow::GameWindow()";
    }


    void GameWindow::showPanel(wxPanel *panel)
    {
        LOG(INFO) << "Called GameWindow::showPanel()";

        // if we are already showing the panel, we don't need to do anything
        if ( this->_currentPanel == panel ) {
            LOG(INFO) << "Was same panel";
            return;
        }
        LOG(INFO) << "Wasn't the same panel";

        // remove previous panel
        if ( this->_currentPanel != nullptr ) {
            LOG(INFO) << "Current panel is not nullptr";
            this->_mainLayout->Detach(this->_currentPanel);
            LOG(INFO) << "Detached current panel";
            this->_currentPanel->Show(false);
            LOG(INFO) << "Not showing current panel any more";
            this->_currentPanel = nullptr;
            LOG(INFO) << "Changed current panel to nullptr";
        }
        LOG(INFO) << "Removed previous panel";

        // add new panel
        this->_mainLayout->Add(panel, 1, wxALIGN_CENTER, 20); // 20 pixel spacing
        panel->Show(true);
        this->_currentPanel = panel;
        LOG(INFO) << "Added new panel";

        // update layout
        this->_mainLayout->Layout();
        LOG(INFO) << "Updated Layout";

        // update window size
        this->Fit();
        LOG(INFO) << "Done with GameWindow::showPanel()";
    }


    void GameWindow::setStatus(const std::string &message) { this->_statusBar->SetStatusText(message, 0); }

} // namespace client
