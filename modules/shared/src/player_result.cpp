
#include <string>

#include <shared/player_result.h>

namespace shared
{

    PlayerResult::PlayerResult(std::string player_name, int score) : _playerName(player_name), _score(score) {}

    std::string PlayerResult::playerName() const { return _playerName; }

    int PlayerResult::score() const { return _score; }

    bool PlayerResult::operator==(const PlayerResult &other) const
    {
        return _playerName == other._playerName && _score == other._score;
    }

    bool PlayerResult::operator!=(const PlayerResult &other) const { return !(*this == other); }

} // namespace shared
