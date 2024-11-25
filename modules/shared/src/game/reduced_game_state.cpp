
#include <algorithm>

#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/json.h>
#include <shared/utils/logger.h>

namespace reduced
{
    bool GameState::operator==(const GameState &other) const
    {
        return *board == *other.board && *reduced_player == *other.reduced_player &&
                std::equal(reduced_enemies.begin(), reduced_enemies.end(), other.reduced_enemies.begin(),
                           other.reduced_enemies.end(),
                           [](const reduced::Enemy::ptr_t &a, const reduced::Enemy::ptr_t &b) { return *a == *b; }) &&
                active_player == other.active_player;
    }

    bool GameState::operator!=(const GameState &other) const { return !(*this == other); }

    rapidjson::Document GameState::toJson() const
    {
        rapidjson::Document doc;
        doc.SetObject();

        rapidjson::Document board_doc = board->toJson();
        rapidjson::Value board_value;
        board_value.CopyFrom(board_doc, doc.GetAllocator());
        doc.AddMember("board", board_value, doc.GetAllocator());

        rapidjson::Document reduced_player_doc = reduced_player->toJson();
        rapidjson::Value reduced_player_value;
        reduced_player_value.CopyFrom(reduced_player_doc, doc.GetAllocator());
        doc.AddMember("reduced_player", reduced_player_value, doc.GetAllocator());

        rapidjson::Value reduced_enemies_value(rapidjson::kArrayType);
        for ( const auto &reduced_enemy : reduced_enemies ) {
            rapidjson::Document reduced_enemy_doc = reduced_enemy->toJson();
            rapidjson::Value reduced_enemy_value;
            reduced_enemy_value.CopyFrom(reduced_enemy_doc, doc.GetAllocator());
            reduced_enemies_value.PushBack(reduced_enemy_value, doc.GetAllocator());
        }
        doc.AddMember("reduced_enemies", reduced_enemies_value, doc.GetAllocator());

        ADD_STRING_MEMBER(this->active_player.c_str(), active_player);

        return doc;
    }

    std::unique_ptr<GameState> GameState::fromJson(const rapidjson::Value &json)
    {
        if ( !json.IsObject() ) {
            LOG(WARN) << "GameState::fromJson: JSON is not an object";
            return nullptr;
        }

        shared::Board::ptr_t board;
        if ( json.HasMember("board") ) {
            board = shared::Board::fromJson(json["board"]);
            if ( board == nullptr ) {
                LOG(WARN) << "GameState::fromJson: Failed to parse board";
                return nullptr;
            }
        } else {
            LOG(WARN) << "GameState::fromJson: JSON does not have 'board' member";
            return nullptr;
        }

        reduced::Player::ptr_t reduced_player;
        if ( json.HasMember("reduced_player") ) {
            reduced_player = reduced::Player::fromJson(json["reduced_player"]);
            if ( reduced_player == nullptr ) {
                LOG(WARN) << "GameState::fromJson: Failed to parse reduced_player";
                return nullptr;
            }
        } else {
            LOG(WARN) << "GameState::fromJson: JSON does not have 'reduced_player' member";
            return nullptr;
        }

        std::vector<reduced::Enemy::ptr_t> reduced_enemies;
        if ( json.HasMember("reduced_enemies") ) {
            if ( !json["reduced_enemies"].IsArray() ) {
                LOG(WARN) << "GameState::fromJson: 'reduced_enemies' is not an array";
                return nullptr;
            }

            for ( const auto &reduced_enemy_json : json["reduced_enemies"].GetArray() ) {
                reduced::Enemy::ptr_t reduced_enemy = reduced::Enemy::fromJson(reduced_enemy_json);
                if ( reduced_enemy == nullptr ) {
                    LOG(WARN) << "GameState::fromJson: Failed to parse reduced_enemy";
                    return nullptr;
                }
                reduced_enemies.push_back(std::move(reduced_enemy));
            }
        } else {
            LOG(WARN) << "GameState::fromJson: JSON does not have 'reduced_enemies' member";
            return nullptr;
        }

        shared::PlayerBase::id_t active_player;
        GET_STRING_MEMBER(active_player, json, "active_player");

        return std::make_unique<GameState>(std::move(board), std::move(reduced_player), std::move(reduced_enemies),
                                           active_player);
    }
} // namespace reduced
