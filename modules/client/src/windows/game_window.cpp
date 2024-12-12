#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <windows/game_window.h>
#include <wx/dcbuffer.h>
#include <wx/display.h>
#include <wx/wx.h>

namespace client
{

    GameWindow::GameWindow(const wxString &title, const wxPoint &pos, const wxSize &size) :
        wxFrame(nullptr, wxID_ANY, title, pos, size), _mainLayout(new wxBoxSizer(wxVERTICAL)), _currentPanel(nullptr)
    {
        // Set up layout that will contain and center all content
        wxBoxSizer *outerLayout = new wxBoxSizer(wxHORIZONTAL);
        outerLayout->Add(this->_mainLayout, 1, wxCENTER);
        this->SetSizerAndFit(outerLayout);

        // Set up status bar
        this->_statusBar = this->CreateStatusBar(1);

        // Set background style to allow custom drawing
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);

        // Set the minimum size of the window. The user won't be able to resize the window to a size smaller than this
        this->SetMinSize(wxSize(1000, 720));

#ifdef __WXMAC__

#else
        this->ShowFullScreen(true, wxFULLSCREEN_ALL);
#endif

        // Bind paint event to draw background image
        this->Bind(wxEVT_PAINT, &GameWindow::onPaint, this);
    }

    void GameWindow::showPanel(wxPanel *panel)
    {
        // if we are already showing the panel, we don't need to do anything
        if ( this->_currentPanel == panel ) {
            return;
        }

        // idk why, but if we use this then the panel switch actually works on osx...
        this->CallAfter(
                [panel, this]()
                {
                    if ( this->_currentPanel != nullptr ) {
                        this->_mainLayout->Detach(this->_currentPanel);

                        this->_mainLayout->Layout();
                        this->Update();
                        this->Refresh();

                        this->_currentPanel->Hide();
                        this->_currentPanel = nullptr;
                    }

                    if ( panel != nullptr ) {
                        this->_mainLayout->Add(panel, 1, wxALIGN_CENTER, 20); // 20-pixel spacing
                        panel->Show(true);
                        this->_currentPanel = panel;

                        this->_mainLayout->Layout();
                        this->Fit();
                    }
                });
    }

    void GameWindow::setStatus(const std::string &message) { this->_statusBar->SetStatusText(message, 0); }

    void GameWindow::onPaint(wxPaintEvent & /*event*/)
    {
        wxBufferedPaintDC dc(this);
        // Load the background image
        wxImage backgroundImage(formatting_constants::WINDOW_BACKGROUND_PATH);
        if ( backgroundImage.IsOk() ) {
            wxBitmap bitmap(backgroundImage.Scale(GetSize().GetWidth(), GetSize().GetHeight()));
            dc.DrawBitmap(bitmap, 0, 0, false);
        } else {
            // Fallback to a plain color if the image fails to load
            dc.SetBackground(wxBrush(formatting_constants::DEFAULT_WINDOW_BACKGROUND));
            dc.Clear();
        }
    }

} // namespace client