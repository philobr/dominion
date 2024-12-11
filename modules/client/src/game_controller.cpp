
#include <controller_event.h>
#include <game_controller.h>
#include <memory>
#include <shared/action_order.h>
#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>
#include <vector>
#include "shared/action_decision.h"

using namespace shared;

namespace client
{
    void GameController::showStatus(std::string message)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showStatus(std::move(message)));
    }

    void GameController::showError(std::string title, std::string message)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showError(std::move(title), std::move(message)));
    }

    void GameController::showLobbyScreen(std::vector<reduced::Player::id_t> players, bool is_game_master)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showLobbyScreen(std::move(players), is_game_master));
    }

    void GameController::showGameScreen(std::unique_ptr<reduced::GameState> game_state)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showGameScreen(std::move(game_state)));
    }

    void GameController::showVictoryScreen(std::vector<shared::PlayerResult> results)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showVictoryScreen(std::move(results)));
    }

    void GameController::showCardSelectionScreen()
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showCardSelectionScreen());
    }

    void GameController::showGainFromBoard(std::unique_ptr<reduced::GameState> game_state,
                                           shared::GainFromBoardOrder order)
    {
        wxQueueEvent(_guiEventReceiver.get(),
                     ControllerEvent::showGainFromBoardScreen(std::move(game_state), std::move(order)));
    }
    void GameController::showChooseFromHandOrder(std::unique_ptr<reduced::GameState> game_state,
                                                 shared::ChooseFromHandOrder order)
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showChooseFromHandScreen(std::move(game_state), order));
    }

    GameController::GameController(GuiEventReceiver *event_receiver) :
        _guiEventReceiver(event_receiver), _clientState(ClientState::LOGIN_SCREEN)
    {}

    void GameController::createLobby()
    {
        Gui &gui = _guiEventReceiver->getGui();
        if ( _clientState != ClientState::LOGIN_SCREEN ) {
            showError("Error", "Tried to create lobby while not in login screen");
            return;
        }

        ConnectionForm input;
        if ( !gui.getConnectionForm(input) ) {
            return;
        }

        LOG(DEBUG) << "Creating lobby " << input.lobby_name;

        _clientNetworkManager->init(input.host, input.port);
        if ( _clientNetworkManager->failedToConnect() ) {
            _clientState = ClientState::LOGIN_SCREEN;
            LOG(INFO) << "Reverted to ClientState::LOGIN_SCREEN";
        } else {

            _gameName = input.lobby_name;
            _playerName = input.player_name;
            _clientState = ClientState::CREATING_LOBBY;

            // send request to join game
            std::unique_ptr<shared::CreateLobbyRequestMessage> request =
                    std::make_unique<shared::CreateLobbyRequestMessage>(input.lobby_name, input.player_name);
            sendRequest(std::move(request));
        }
    }

    void GameController::joinLobby()
    {
        Gui &gui = _guiEventReceiver->getGui();
        if ( _clientState != ClientState::LOGIN_SCREEN ) {
            showError("Error", "Tried to join lobby while not in login screen");
            return;
        }

        ConnectionForm input;
        if ( !gui.getConnectionForm(input) ) {
            return;
        }

        LOG(DEBUG) << "Joining lobby " << input.lobby_name;

        _clientNetworkManager->init(input.host, input.port);

        if ( _clientNetworkManager->failedToConnect() ) {
            _clientState = ClientState::LOGIN_SCREEN;
            LOG(INFO) << "Reverted to ClientState::LOGIN_SCREEN";
        } else {
            _gameName = input.lobby_name;
            _playerName = input.player_name;
            _clientState = ClientState::JOINING_LOBBY;

            std::unique_ptr<shared::JoinLobbyRequestMessage> request =
                    std::make_unique<shared::JoinLobbyRequestMessage>(input.lobby_name, input.player_name);
            sendRequest(std::move(request));
        }
    }

    void GameController::proceedToCardSelection()
    {
        if ( !isLobbyValid() ) {
            return;
        }
        LOG(DEBUG) << "Proceeding to card selection";
        showCardSelectionScreen();
    }

    void GameController::startGame(std::unordered_map<shared::CardBase::id_t, bool> selected_cards)
    {
        if ( !isLobbyValid() ) {
            return;
        }
        // making a vector out of the selected cards
        std::vector<shared::CardBase::id_t> selectedCards;
        for ( const auto &card : selected_cards ) {
            if ( card.second ) {
                selectedCards.push_back(card.first);
            }
        }

        if ( selectedCards.size() != shared::board_config::KINGDOM_CARD_COUNT ) {
            LOG(DEBUG) << "Invalid number of selected cards: " << selectedCards.size();
            showError("Error",
                      "You need to select exactly " + std::to_string(shared::board_config::KINGDOM_CARD_COUNT) +
                              " cards");
            return;
        }
        LOG(DEBUG) << "Requesting to start game";
        std::unique_ptr<shared::StartGameRequestMessage> msg =
                std::make_unique<shared::StartGameRequestMessage>(_gameName, _playerName, selectedCards);
        sendRequest(std::move(msg));
        _clientState = ClientState::STARTING_GAME;
    }


    void GameController::buyCard(const std::string &card_id)
    {
        LOG(DEBUG) << "Buying card " << card_id << std::endl;

        std::unique_ptr<shared::ActionDecision> decision(new shared::BuyCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }

    void GameController::playCard(const std::string &card_id)
    {
        LOG(DEBUG) << "Playing card " << card_id;

        std::unique_ptr<shared::ActionDecision> decision(new shared::PlayActionCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }

    void GameController::gainCardFromBoard(const std::string &card_id)
    {
        LOG(DEBUG) << "Gaining card from board " << card_id;

        std::unique_ptr<shared::ActionDecision> decision(new shared::GainFromBoardDecision(card_id));

        // TODO (#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);
        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }
    void GameController::confirmSelectionFromHand(std::vector<shared::CardBase::id_t> selected_cards,
                                                  std::vector<shared::ChooseFromOrder::AllowedChoice> choices)
    {
        LOG(DEBUG) << "Confirming selection";
        std::unique_ptr<shared::ActionDecision> decision(new shared::DeckChoiceDecision(selected_cards, choices));

        // TODO (#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);
        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }

    void GameController::endActionPhase()
    {
        LOG(INFO) << "Ending action phase";

        std::unique_ptr<shared::ActionDecision> decision(new shared::EndActionPhaseDecision());

        // TODO (#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }

    void GameController::endTurn()
    {
        LOG(DEBUG) << "Ending turn";

        std::unique_ptr<shared::ActionDecision> decision(new shared::EndTurnDecision());

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, _playerName, std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(std::move(action_decision_message));
    }

    void GameController::sendRequest(std::unique_ptr<shared::ClientToServerMessage> req)
    {
        _clientNetworkManager->sendRequest(std::move(req));
    }

    void GameController::receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> msg)
    {
        if ( _clientState != ClientState::IN_GAME ) {
            LOG(WARN) << "Received unexpected ActionOrderMessage while in state " << _clientState;
            return;
        }

        ActionOrder &action_order = *msg->order;
        if ( typeid(action_order) == typeid(ActionPhaseOrder) ) {
            // TODO(#194) This will be combined with BuyPhaseOrder
            LOG(WARN) << "Received ActionPhaseOrder, but this does not do anything yet";
            showGameScreen(std::move(msg->game_state));
        } else if ( typeid(action_order) == typeid(BuyPhaseOrder) ) {
            // TODO(#194) This will be combined with ActionPhaseOrder
            LOG(WARN) << "Received BuyPhaseOrder, but this does not do anything yet";
            showGameScreen(std::move(msg->game_state));
        } else if ( typeid(action_order) == typeid(EndTurnOrder) ) {
            // TODO(#194) Remove this
            LOG(ERROR) << "Received EndTurnOrder, this is deprecated (see #194)";
            return;
        } else if ( typeid(action_order) == typeid(GainFromBoardOrder) ) {
            LOG(WARN) << "Received GainFromBoardOrder";
            showGainFromBoard(std::move(msg->game_state),
                              std::move(*dynamic_cast<GainFromBoardOrder *>(&action_order)));
            return;
        } else if ( typeid(action_order) == typeid(ChooseFromOrder) ) {
            // TODO(#195): Implement
            LOG(WARN) << "Received ChooseFromOrder, but this does not do anything yet";
            return;
        } else if ( typeid(action_order) == typeid(ChooseFromStagedOrder) ) {
            // TODO(#195): Implement
            LOG(WARN) << "Received ChooseFromStagedOrder, but this does not do anything yet";
            return;
        } else if ( typeid(action_order) == typeid(ChooseFromHandOrder) ) {
            // TODO(#195): Implement
            LOG(WARN) << "Received ChooseFromHandOrder, but this does not do anything yet";
            return;
        } else {
            LOG(ERROR) << "Received unknown ActionOrderMessage: " << typeid(action_order).name();
        }
    }

    void GameController::receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> /*msg*/)
    {
        if ( _clientState != ClientState::CREATING_LOBBY ) {
            LOG(ERROR) << "Received unexpected CreateLobbyResponseMessage";
            return;
        }
        LOG(DEBUG) << "Successfully created lobby";
        std::vector<reduced::Player::id_t> players = {_playerName};
        _numPlayers = 1;
        showLobbyScreen(players, true);
    }

    void GameController::receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg)
    {
        LOG(DEBUG) << "Successfully joined lobby";
        LOG(DEBUG) << "Player " << msg->players.back() << " joined the lobby";
        showLobbyScreen(msg->players, false);
        _numPlayers = msg->players.size();
        _clientState = ClientState::IN_LOBBY;
    }

    void GameController::receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg)
    {
        switch ( _clientState ) {
            case ClientState::LOGIN_SCREEN:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in login screen";
                break;
            case ClientState::JOINING_LOBBY:
                if ( msg->success ) {
                    // Nothing to do here, since everyhing is handled in JoinLobbyBroadcastMessage
                } else {
                    LOG(DEBUG) << "Failed to join lobby";
                    if ( msg->additional_information.has_value() ) {
                        showError("Failed to join lobby", msg->additional_information.value());
                    } else {
                        showError("Failed to join lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    _clientState = ClientState::LOGIN_SCREEN;
                }
                break;
            case ClientState::CREATING_LOBBY:
                if ( msg->success ) {
                    LOG(DEBUG) << "Successfully created lobby";
                    showLobbyScreen({_playerName}, true);
                    _clientState = ClientState::IN_LOBBY;
                } else {
                    LOG(DEBUG) << "Failed to create lobby";
                    if ( msg->additional_information.has_value() ) {
                        showError("Failed to create lobby", msg->additional_information.value());
                    } else {
                        showError("Failed to create lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    _clientState = ClientState::LOGIN_SCREEN;
                }
                break;
            case ClientState::STARTING_GAME:
                if ( msg->success ) {
                    LOG(ERROR) << "Received ResultResponseMessage(success) while starting game";
                    return;
                } else {
                    LOG(DEBUG) << "Failed to start game";
                    // TODO(#175) This should be cleaned up, returning "Unknown problem" is not very helpful
                    showError("Failed to start game, error: ", msg->additional_information.value_or("Unknown problem"));
                    LOG(INFO) << "Returning to lobby screen";
                    _clientState = ClientState::IN_LOBBY;
                }
                break;
            case ClientState::IN_LOBBY:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in lobby screen";
                break;
            case ClientState::IN_GAME:
                if ( msg->success ) {
                    LOG(WARN) << "Received unexpected ResultResponseMessage while in running game";
                } else {
                    showError("Error", msg->additional_information.value_or("Unknown error"));
                }
                break;
            case ClientState::VICTORY_SCREEN:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in victory screen";
                break;
            default:
                LOG(WARN) << "Received ResultResponseMessage, but client is in unknown state";
                break;
        }
    }

    void GameController::receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg)
    {
        // TODO(#125): Unfortunately, this is currently still used to update
        // the game state of the players that are not the active player.
        LOG(WARN) << "Received GameStateMessage, this is deprecated";
        showGameScreen(std::move(msg->game_state));
    }

    void GameController::receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> /*msg*/)
    {
        LOG(DEBUG) << "Game starting soon (StartGameBroadcastMessage)";
        _clientState = ClientState::IN_GAME;
    }

    void GameController::receiveEndGameBroadcastMessage(std::unique_ptr<shared::EndGameBroadcastMessage> msg)
    {
        if ( _clientState != ClientState::IN_GAME ) {
            LOG(WARN) << "Received unexpected EndGameBroadcastMessage while not in game";
            return;
        }
        LOG(DEBUG) << "Game ended (EndGameBroadcastMessage)";
        showVictoryScreen(std::move(msg->results));
        _clientState = ClientState::VICTORY_SCREEN;
    }

    void GameController::receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
// NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE_MESSAGE(type)                                                                                           \
    if ( typeid(msgRef) == typeid(type) ) {                                                                            \
        LOG(INFO) << "Received message of type " << #type;                                                             \
        std::unique_ptr<type> casted(static_cast<type *>(msg.release()));                                              \
        receive##type(std::move(casted));                                                                              \
        return;                                                                                                        \
    }
        // NOLINTEND(bugprone-macro-parentheses)
        ServerToClientMessage &msgRef = *msg;
        HANDLE_MESSAGE(ActionOrderMessage);
        HANDLE_MESSAGE(CreateLobbyResponseMessage);
        HANDLE_MESSAGE(JoinLobbyBroadcastMessage);
        HANDLE_MESSAGE(ResultResponseMessage);
        HANDLE_MESSAGE(GameStateMessage);
        HANDLE_MESSAGE(StartGameBroadcastMessage);
        HANDLE_MESSAGE(EndGameBroadcastMessage);
#undef HANDLE_MESSAGE

        LOG(ERROR) << "Unknown message type";
        throw exception::UnreachableCode("Unknown message type");
    }

    void GameController::skipToVictoryScreen()
    {
        LOG(WARN) << "Skipping to victory screen, this is debug functionality";
        LOG(ERROR) << "Skipping to victory screen is not implemented";
    }

    void GameController::skipToGamePanel()
    {
        LOG(WARN) << "Skipping to game panel, this is debug functionality";
        // We need a game state here, but we don't have one
        // We just pass nullptr for now
        showGameScreen(nullptr);
    }

    void GameController::skipToCardSelectionPanel()
    {
        LOG(WARN) << "Skipping to game panel, this is debug functionality";
        // We need a game state here, but we don't have one
        // We just pass nullptr for now
        showCardSelectionScreen();
    }

    bool GameController::isLobbyValid()
    {
        if ( _numPlayers < shared::board_config::MIN_PLAYER_COUNT ||
             _numPlayers > shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Invalid number of players ( " << _numPlayers << " ) to start game";
            _guiEventReceiver->getGui().showError("Error", "Invalid number of players");
            return false;
        }
        return true;
    }
} // namespace client
