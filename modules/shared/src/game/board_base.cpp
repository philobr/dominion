#include <algorithm>
#include <shared/game/game_state/board_base.h>

namespace shared
{
    namespace BoardConfig
    {
        static constexpr size_t getCopperCount(size_t num_players) { return TREASURE_COPPER_COUNT - (7 * num_players); }

        static constexpr size_t getVictoryCardCount(size_t num_players)
        {
            return num_players == 2 ? VICTORY_CARDS_SMALL_GAME : VICTORY_CARDS_LARGE_GAME;
        }

        static constexpr bool validatePlayerCount(size_t num_players)
        {
            return MIN_PLAYER_COUNT <= num_players && num_players <= MAX_PLAYER_COUNT;
        }

        static constexpr size_t getCurseCardCount(size_t num_players) { return CURSE_MULTIPLIER * (num_players - 1); }

    } // namespace BoardConfig

    Pile Pile::makeKingdomCard(const shared::CardBase::id_t &kingdom_card_id)
    {
        return Pile(kingdom_card_id, BoardConfig::KINGDOM_CARD_COUNT);
    }

    Pile Pile::make(const shared::CardBase::id_t &card_id, size_t pile_size) { return Pile(card_id, pile_size); }

    Board::Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        _ASSERT_EQ(kingdom_cards.size(), BoardConfig::KINGDOM_CARD_COUNT,
                   std::string_view{"Board must be initialised with 10 kingdom cards, but was initialised with " +
                                    std::to_string(kingdom_cards.size()) + " cards"});

        _ASSERT_TRUE(BoardConfig::validatePlayerCount(player_count),
                     std::string_view{"player_count must be in [" + std::to_string(BoardConfig::MIN_PLAYER_COUNT) +
                                      ", " + std::to_string(BoardConfig::MAX_PLAYER_COUNT) + "], but is " +
                                      std::to_string(player_count)});

        std::transform(kingdom_cards.begin(), kingdom_cards.end(),
                       std::inserter(this->kingdom_cards, this->kingdom_cards.end()),
                       [](const shared::CardBase::id_t &card_id) { return Pile::makeKingdomCard(card_id); });

        initialiseTreasureCards(player_count);
        initialiseVictoryCards(player_count);
    }

    Board::ptr_t Board::make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        return ptr_t(new Board(kingdom_cards, player_count));
    }

    size_t Board::getEmptyPilesCount() const
    {
        auto count_empty = [](const auto &pile_set) -> size_t
        { return std::count_if(pile_set.begin(), pile_set.end(), [](const auto &pile) { return pile.count == 0; }); };

        return count_empty(treasure_cards) + count_empty(victory_cards) + count_empty(kingdom_cards);
    }

    bool Board::isGameOver() const
    {
        auto province_pile = victory_cards.find("Province");
        return ((province_pile != victory_cards.end()) && (province_pile->count == 0)) ||
                (getEmptyPilesCount() >= BoardConfig::MAX_NUM_EMPTY_PILES);
    }

    void Board::initialiseTreasureCards(size_t player_count)
    {
        treasure_cards = {Pile::make("Copper", BoardConfig::getCopperCount(player_count)),
                          Pile::make("Silver", BoardConfig::TREASURE_SILVER_COUNT),
                          Pile::make("Gold", BoardConfig::TREASURE_GOLD_COUNT)};
    }

    void Board::initialiseVictoryCards(size_t player_count)
    {
        const size_t card_count = BoardConfig::getVictoryCardCount(player_count);
        const size_t curse_count = BoardConfig::getCurseCardCount(player_count);

        victory_cards = {Pile::make("Estate", card_count), Pile::make("Duchy", card_count),
                         Pile::make("Province", card_count), Pile::make("Curse", curse_count)};
    }

} // namespace shared
