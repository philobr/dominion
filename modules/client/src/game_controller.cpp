
#include <controller_event.h>
#include <game_controller.h>
#include <memory>
#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>
#include <vector>
#include "shared/action_order.h"

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

    void GameController::showVictoryScreen()
    {
        wxQueueEvent(_guiEventReceiver.get(), ControllerEvent::showVictoryScreen());
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

            // send request to join game
            std::unique_ptr<shared::CreateLobbyRequestMessage> request =
                    std::make_unique<shared::CreateLobbyRequestMessage>(input.lobby_name, input.player_name);
            sendRequest(std::move(request));

            _gameName = input.lobby_name;
            _playerName = input.player_name;
            _clientState = ClientState::CREATING_LOBBY;
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

        std::unique_ptr<shared::JoinLobbyRequestMessage> request =
                std::make_unique<shared::JoinLobbyRequestMessage>(input.lobby_name, input.player_name);
        sendRequest(std::move(request));

        _gameName = input.lobby_name;
        _playerName = input.player_name;
        _clientState = ClientState::JOINING_LOBBY;
    }

    void GameController::startGame()
    {
        if ( _numPlayers < shared::board_config::MIN_PLAYER_COUNT ||
             _numPlayers > shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Invalid number of players ( " << _numPlayers << " ) to start game";
            _guiEventReceiver->getGui().showError("Error", "Invalid number of players");
            return;
        }
        LOG(DEBUG) << "Starting game";
        // TODO Implement card selection
        std::vector<shared::CardBase::id_t> selectedCards{"Moat",         "Smithy",      "Village",      "Laboratory",
                                                          "Festival",     "Market",      "Placeholder1", "Placeholder2",
                                                          "Placeholder3", "Placeholder4"};
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
        // TODO(#125) This is not implemented, and will probably be removed with #125
        if ( _clientState != ClientState::IN_GAME ) {
            LOG(ERROR) << "Received unexpected ActionOrderMessage";
            return;
        }
        showGameScreen(std::move(msg->game_state));

        if ( typeid(msg) == typeid(ActionPhaseOrder) ) {
            // TODO
            LOG(WARN) << "Received ActionPhaseOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(BuyPhaseOrder) ) {
            // TODO
            LOG(WARN) << "Received BuyPhaseOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(EndTurnOrder) ) {
            // TODO
            LOG(WARN) << "Received EndTurnOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(GainFromBoardOrder) ) {
            // TODO
            LOG(WARN) << "Received GainFromBoardOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(ChooseFromOrder) ) {
            // TODO
            LOG(WARN) << "Received ChooseFromOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(ChooseFromStagedOrder) ) {
            // TODO
            LOG(WARN) << "Received ChooseFromStagedOrder, but this does not do anything yet";

        } else if ( typeid(msg) == typeid(ChooseFromHandOrder) ) {
            // TODO
            LOG(WARN) << "Received ChooseFromHandOrder, but this does not do anything yet";
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
            default:
                LOG(WARN) << "Received ResultResponseMessage, but client is in unknown state";
                break;
        }
    }

    void GameController::receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg)
    {
        LOG(INFO) << "Received GameStateMessage, updating game state";
        showGameScreen(std::move(msg->game_state));
    }

    void GameController::receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> /*msg*/)
    {
        LOG(DEBUG) << "Starting game";
        _clientState = ClientState::IN_GAME;
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
#undef HANDLE_MESSAGE

        LOG(ERROR) << "Unknown message type";
        throw exception::UnreachableCode("Unknown message type");
    }

    void GameController::skipToVictoryScreen()
    {
        LOG(WARN) << "Skipping to victory screen, this is debug functionality";
        showVictoryScreen();
    }

    void GameController::skipToGamePanel()
    {
        LOG(WARN) << "Skipping to game panel, this is debug functionality";
        // We need a game state here, but we don't have one
        // We just pass nullptr for now
        showGameScreen(nullptr);
    }
} // namespace client
