
#include <shared/game/game_state/game_phase.h>
#include <stdexcept>

namespace shared
{
    std::string gamePhaseToString(GamePhase game_phase)
    {
        switch ( game_phase ) {
            case GamePhase::ACTION_PHASE:
                return "action_phase";
            case GamePhase::BUY_PHASE:
                return "buy_phase";
            case GamePhase::PLAYING_ACTION_CARD:
                return "playing_action_card";
            default:
                throw std::invalid_argument("Invalid game phase");
        }
    }

    GamePhase gamePhaseFromString(const std::string &game_phase)
    {
        if ( game_phase == "action_phase" ) {
            return GamePhase::ACTION_PHASE;
        } else if ( game_phase == "buy_phase" ) {
            return GamePhase::BUY_PHASE;
        } else if ( game_phase == "playing_action_card" ) {
            return GamePhase::PLAYING_ACTION_CARD;
        } else {
            throw std::invalid_argument("Invalid game phase");
        }
    }
} // namespace shared
