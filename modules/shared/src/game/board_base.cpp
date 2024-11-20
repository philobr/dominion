
#include <algorithm>
#include <rapidjson/document.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/json.h>

namespace shared
{
    Pile Pile::makeKingdomCard(const shared::CardBase::id_t &kingdom_card_id)
    {
        return Pile(kingdom_card_id, BoardConfig::KINGDOM_CARD_COUNT);
    }

    std::unique_ptr<Pile> Pile::fromJson(const rapidjson::Value &json)
    {
        CardBase::id_t card_id;
        GET_STRING_MEMBER(card_id, json, "card_id");
        size_t count;
        GET_UINT_MEMBER(count, json, "count");
        return std::make_unique<Pile>(card_id, count);
    }

    Board::Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        victory_cards(initialiseVictoryCards(player_count)), treasure_cards(initialiseTreasureCards(player_count)),
        curse_card_pile(initialiseCursePile(player_count))
    {
        _ASSERT_TRUE(BoardConfig::validatePlayerCount(player_count),
                     std::string_view{"player_count must be in [" + std::to_string(BoardConfig::MIN_PLAYER_COUNT) +
                                      ", " + std::to_string(BoardConfig::MAX_PLAYER_COUNT) + "], but is " +
                                      std::to_string(player_count)});

        _ASSERT_EQ(kingdom_cards.size(), BoardConfig::KINGDOM_CARD_COUNT,
                   std::string_view{"Board must be initialised with 10 kingdom cards, but was initialised with " +
                                    std::to_string(kingdom_cards.size()) + " cards"});

        std::transform(kingdom_cards.begin(), kingdom_cards.end(),
                       std::inserter(this->kingdom_cards, this->kingdom_cards.end()),
                       [](const shared::CardBase::id_t &card_id) { return Pile::makeKingdomCard(card_id); });

        _ASSERT_EQ(this->kingdom_cards.size(), BoardConfig::KINGDOM_CARD_COUNT,
                   std::string_view{"Board received duplicate kingdom card"});
    }

    Board::ptr_t Board::make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        return ptr_t(new Board(kingdom_cards, player_count));
    }

    size_t Board::getEmptyPilesCount() const
    {
        auto count_empty = [](const auto &pile_set) -> size_t
        { return std::count_if(pile_set.begin(), pile_set.end(), [](const auto &pile) { return pile.count == 0; }); };

        return count_empty(treasure_cards) + count_empty(victory_cards) + count_empty(kingdom_cards) +
                static_cast<size_t>(curse_card_pile.empty());
    }

    bool Board::isGameOver() const
    {
        auto is_province_pile_empty = [&]() -> bool
        {
            if ( auto province_pile = victory_cards.find("Province"); province_pile != victory_cards.end() ) {
                return province_pile->empty();
            }
            return false;
        }();

        return is_province_pile_empty || (getEmptyPilesCount() >= BoardConfig::MAX_NUM_EMPTY_PILES);
    }

    Board::pile_container_t Board::initialiseTreasureCards(size_t player_count)
    {
        return {Pile("Copper", BoardConfig::getCopperCount(player_count)),
                Pile("Silver", BoardConfig::TREASURE_SILVER_COUNT), Pile("Gold", BoardConfig::TREASURE_GOLD_COUNT)};
    }

    Board::pile_container_t Board::initialiseVictoryCards(size_t player_count)
    {
        const size_t card_count = BoardConfig::getVictoryCardCount(player_count);
        return {Pile("Estate", card_count), Pile("Duchy", card_count), Pile("Province", card_count)};
    }

    Pile Board::initialiseCursePile(size_t player_count)
    {
        const size_t curse_count = BoardConfig::getCurseCardCount(player_count);
        return Pile("Curse", curse_count);
    }

} // namespace shared
