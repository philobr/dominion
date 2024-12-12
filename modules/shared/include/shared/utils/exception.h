#pragma once

#include <exception>
#include <string>

namespace exception
{
    /**
     * @brief Base class for custom exceptions
     * @warning PLEASE DONT USE THIS, JUST IMPLEMENT A NEW EXCEPTION!!
     */
    class BaseExceptionType : public std::exception
    {
    private:
        /**
         * @brief Error message stored by the exception
         */
        std::string what_m;

    public:
        /**
         * @brief Construct a new BaseException with a custom error message
         */
        explicit BaseExceptionType(const std::string &error_msg) : what_m(error_msg) {}

        /**
         * @brief Construct a new BaseException with a custom error message
         */
        explicit BaseExceptionType(std::string &&error_msg) : what_m(std::move(error_msg)) {}

        /**
         * @brief Returns the error message
         */
        const char *what() const noexcept override { return what_m.c_str(); }

        /**
         * @brief Virtual destructor for proper cleanup in derived classes
         */
        ~BaseExceptionType() noexcept override = default;
    };

} // namespace exception

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define NEW_BASE_EXCEPTION(class_name, default_error)                                                                  \
    namespace exception                                                                                                \
    {                                                                                                                  \
        class class_name : public BaseExceptionType                                                                    \
        {                                                                                                              \
        public:                                                                                                        \
            explicit class_name(const std::string &error_msg) : BaseExceptionType(error_msg) {}                        \
            explicit class_name() : BaseExceptionType((default_error)) {}                                              \
            explicit class_name(std::string &&error_msg) : BaseExceptionType(std::move(error_msg)) {}                  \
        };                                                                                                             \
    }
// NOLINTEND(bugprone-macro-parentheses)

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define NEW_INHERITED_EXCEPTION(class_name, base_class_name, default_error)                                            \
    namespace exception                                                                                                \
    {                                                                                                                  \
        class class_name : public base_class_name                                                                      \
        {                                                                                                              \
        public:                                                                                                        \
            explicit class_name(const std::string &error_msg) : base_class_name(error_msg) {}                          \
            explicit class_name() : base_class_name((default_error)) {}                                                \
            explicit class_name(std::string &&error_msg) : base_class_name(std::move(error_msg)) {}                    \
        };                                                                                                             \
    }
// NOLINTEND(bugprone-macro-parentheses)

/*
THIS IS AN EXAMPLE USAGE.

NEW_BASE_EXCEPTION(GameState);
NEW_INHERITED_EXCEPTION(GameStateConstructor, GameState);
NEW_INHERITED_EXCEPTION(DuplicateCards, GameStateConstructor);
NEW_INHERITED_EXCEPTION(CardCountMismatch, GameStateConstructor);
 */

NEW_BASE_EXCEPTION(Logger, "");

// for gamestate
NEW_BASE_EXCEPTION(GameState, "GameStateError");
NEW_INHERITED_EXCEPTION(PlayerCountMismatch, GameState, "Wrong number of players.");
NEW_INHERITED_EXCEPTION(InsufficientFunds, GameState, "You don not have enough funds.");
NEW_INHERITED_EXCEPTION(CardNotAvailable, GameState, "Card is not available.");
NEW_INHERITED_EXCEPTION(WrongCardCount, GameState, "Received wrong number of cards.");
NEW_INHERITED_EXCEPTION(OutOfPhase, GameState, "");
NEW_INHERITED_EXCEPTION(OutOfActions, GameState, "You do not have enough actions.");
NEW_INHERITED_EXCEPTION(InvalidCardAccess, GameState, "");
NEW_INHERITED_EXCEPTION(InvalidCardType, GameState, "");
NEW_INHERITED_EXCEPTION(InvalidRequest, GameState, "");
NEW_INHERITED_EXCEPTION(NotYourTurn, GameState, "");

NEW_BASE_EXCEPTION(SevereError, "Severe Error!");
NEW_INHERITED_EXCEPTION(UnreachableCode, SevereError, "This should NEVER happen!");
NEW_INHERITED_EXCEPTION(UnrecoverableError, SevereError, "This is not recoverable, shutting down!");