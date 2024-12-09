
#pragma once

namespace shared
{

    class PlayerResult
    {
    public:
        PlayerResult(std::string player_name, int score);
        ~PlayerResult() = default;

        /**
         * Get the name of the player who achieved this result.
         */
        std::string playerName() const;
        int score() const;

        bool operator==(const PlayerResult &other) const;
        bool operator!=(const PlayerResult &other) const;

    private:
        std::string _playerName;
        int _score;
    };

} // namespace shared
