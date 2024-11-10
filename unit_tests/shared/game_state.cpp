
#include <gtest/gtest.h>
#include <shared/game_state.h>

using namespace shared;
using rapidjson::Document;

#define COMPLETE_TEST(classname, expected, json)                                                                       \
    classname expected_val = expected;                                                                                 \
                                                                                                                       \
    Document parsed_doc;                                                                                               \
    parsed_doc.Parse(json.c_str());                                                                                    \
    EXPECT_FALSE(parsed_doc.HasParseError());                                                                          \
                                                                                                                       \
    std::unique_ptr<classname> actual = classname::from_json(parsed_doc);                                              \
    ASSERT_NE(actual, nullptr);                                                                                        \
    EXPECT_EQ(*actual, expected_val);

TEST(SharedLibraryTest, BoardFromJson)
{
    std::vector<CardBase::id_t> kingdom_cards = {"Witch",    "Moat",    "Village", "Market",      "Woodcutter",
                                                 "Workshop", "Militia", "Smithy",  "CouncilRoom", "Festival"};
    unsigned int num_players = 2;
    Board expected(kingdom_cards, num_players);

    std::string json =
            "{\"kingdom_cards\":[{\"card\":\"Witch\",\"count\":10},{\"card\":\"Moat\",\"count\":10},{\"card\":"
            "\"Village\",\"count\":10},{\"card\":\"Market\",\"count\":10},{\"card\":\"Woodcutter\",\"count\":10},{"
            "\"card\":\"Workshop\",\"count\":10},{\"card\":\"Militia\",\"count\":10},{\"card\":\"Smithy\",\"count\":10}"
            ",{\"card\":\"CouncilRoom\",\"count\":10},{\"card\":\"Festival\",\"count\":10}],\"victory_cards\":[{"
            "\"card\":\"Estate\",\"count\":8},{\"card\":\"Duchy\",\"count\":12},{\"card\":\"Province\",\"count\":12},{"
            "\"card\":\"Curse\",\"count\":10}],\"treasure_cards\":[{\"card\":\"Copper\",\"count\":46},{\"card\":"
            "\"Silver\",\"count\":40},{\"card\":\"Gold\",\"count\":30}],\"trash\":[]}";

    COMPLETE_TEST(Board, expected, json);
}

TEST(SharedLibraryTest, BoardJson2WayConversion)
{
    std::vector<CardBase::id_t> kingdom_cards = {"Cellar",     "Chapel",   "Moat",       "Chancellor", "Village",
                                                 "Woodcutter", "Workshop", "Bureaucrat", "Feast",      "Gardens"};
    unsigned int num_players = 4;
    Board expected(kingdom_cards, num_players);

    Document doc = expected.to_json();
    std::string json = document_to_string(doc);
    COMPLETE_TEST(Board, expected, json);
}

TEST(SharedLibraryTest, PileFromJson)
{
    Pile expected("Witch", 10);
    std::string json = "{\"card\":\"Witch\",\"count\":10}";
    COMPLETE_TEST(Pile, expected, json);
}

TEST(SharedLibraryTest, PileJson2WayConversion)
{
    Pile expected("Moat", 10);
    Document doc = expected.to_json();
    std::string json = document_to_string(doc);
    COMPLETE_TEST(Pile, expected, json);
}
