#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/pile_panel.h>
#include <uiElements/popup.h>
#include <uiElements/single_card_panel.h>
#include <uiElements/text_panel.h>

#include <wx/dcbuffer.h>
#include <wx/gbsizer.h>
#include "shared/game/cards/card_base.h"

namespace client
{
    EnemyPanel::EnemyPanel(wxWindow *parent, reduced::Enemy &enemy, const bool is_active) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 100)), _is_active(is_active)
    {
        // Set a light red background color
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);
        this->Bind(wxEVT_PAINT, &EnemyPanel::onPaint, this);

        // Outer sizer to add horizontal padding
        wxBoxSizer *outerSizer = new wxBoxSizer(wxHORIZONTAL);

        // Inner sizer for content
        wxBoxSizer *innerSizer = new wxBoxSizer(wxHORIZONTAL);


        /* ===========display draw pile=========== */
        wxBoxSizer *drawPileSizer = new wxBoxSizer(wxVERTICAL);

        // Text for the title
        TextPanel *DrawPileText = new TextPanel(this, wxID_ANY, "Draw Pile", TextFormat::PLAIN);

        // The pile itself
        shared::CardBase::id_t top_draw_card = "empty_panel";
        if ( enemy.getDrawPileSize() > 0 ) {
            top_draw_card = "Card_back";
        }
        shared::Pile Draw_Pile(top_draw_card, enemy.getDrawPileSize());
        auto height = 30;
        PilePanel *Draw_Pile_panel = new PilePanel(this, Draw_Pile, wxSize(height / 3 * 2, height));

        // add new items to the sizer
        drawPileSizer->Add(DrawPileText, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        drawPileSizer->Add(Draw_Pile_panel, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        innerSizer->Add(drawPileSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));


        /* ===========display player id=========== */
        // new sizer for displaying the name and the hand cards vertically
        auto *centerSizer = new wxGridSizer(2, 1, 1, 2);

        // TODO: display name in a bigger and bold font
        TextPanel *PlayerId = new TextPanel(this, wxID_ANY, enemy.getId(), TextFormat::BOLD);

        // new sizer for the hand cards
        auto *handCardSizer = new wxGridSizer(
                1, enemy.getHandSize() > formatting_constants::MAX_ENEMY_HAND_CARDS ? 1 : enemy.getHandSize(), 1, 1);
        auto hand_card_heigth = 30;
        auto hand_card_width = hand_card_heigth / 3 * 2;
        if ( enemy.getHandSize() > formatting_constants::MAX_ENEMY_HAND_CARDS ) {
            PilePanel *Hand_Pile_panel = new PilePanel(this, shared::Pile("Card_back", enemy.getHandSize()),
                                                       wxSize(hand_card_width, hand_card_heigth));
            handCardSizer->Add(Hand_Pile_panel, 0, wxALL, 2);
        } else {
            for ( unsigned int i = 0; i < enemy.getHandSize(); i++ ) {
                SingleCardPanel *Card =
                        new SingleCardPanel(this, "Card_back", wxSize(hand_card_width, hand_card_heigth));
                handCardSizer->Add(Card, 0, wxALL, 2);
            }
        }

        // add new items to the sizer
        centerSizer->Add(PlayerId, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        centerSizer->Add(handCardSizer, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxBOTTOM, 5));
        innerSizer->Add(centerSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));

        /* ===========display discard pile=========== */
        wxBoxSizer *discardPileSizer = new wxBoxSizer(wxVERTICAL);

        // Text for the title
        TextPanel *discardPileText = new TextPanel(this, wxID_ANY, "Discard Pile", TextFormat::PLAIN);

        shared::CardBase::id_t top_discard_card = "empty_panel";
        if ( enemy.getDiscardPileSize() > 0 ) {
            top_discard_card = enemy.getTopDiscardCard();
        }
        // The pile itself
        shared::Pile Discard_Pile(top_discard_card, enemy.getDiscardPileSize());
        PilePanel *Discard_Pile_panel = new PilePanel(this, Discard_Pile, wxSize(height / 3 * 2, height));

        // Make top card preview if discard pile is not empty
        if ( enemy.getDiscardPileSize() > 0 ) {
            Discard_Pile_panel->makeClickable(
                    wxEVT_RIGHT_UP,
                    [Discard_Pile_panel](wxMouseEvent & /*event*/)
                    { showCardPopup(Discard_Pile_panel, Discard_Pile_panel->getPile().card_id); });
        }

        // add new items to the sizer
        discardPileSizer->Add(discardPileText, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        discardPileSizer->Add(Discard_Pile_panel, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        innerSizer->Add(discardPileSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));


        // finally add the inner sizer to the outer sizer
        outerSizer->Add(innerSizer, wxSizerFlags(1).Expand().Border(wxALL, 1));

        this->SetSizer(outerSizer);
    }

    // generated by OpenAI
    void EnemyPanel::onPaint(wxPaintEvent &event)
    {
        wxAutoBufferedPaintDC dc(this); // Use double-buffered drawing to prevent flickering

        // Get the panel dimensions
        wxSize size = this->GetSize();

        // Create a rounded rectangle
        wxBrush brush(_is_active ? formatting_constants::ACTIVE_ENEMY_BACKGROUND
                                 : formatting_constants::ENEMY_BACKGROUND);
        dc.SetBrush(brush);
        dc.SetPen(*wxTRANSPARENT_PEN); // No border
        dc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 10); // Radius of 10 for rounded corners

        // Continue normal event processing
        event.Skip();
    }
} // namespace client
