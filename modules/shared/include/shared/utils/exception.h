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

#define NEW_BASE_EXCPETION(class_name)                                                                                 \
    namespace exception                                                                                                \
    {                                                                                                                  \
        class class_name : public BaseExceptionType                                                                    \
        {                                                                                                              \
        public:                                                                                                        \
            explicit class_name(const std::string &error_msg) : BaseExceptionType(error_msg) {}                        \
                                                                                                                       \
            explicit class_name(std::string &&error_msg) : BaseExceptionType(std::move(error_msg)) {}                  \
        };                                                                                                             \
    }

#define NEW_INHERITED_EXCEPTION(class_name, base_class_name)                                                           \
    namespace exception                                                                                                \
    {                                                                                                                  \
        class class_name : public base_class_name                                                                      \
        {                                                                                                              \
        public:                                                                                                        \
            explicit class_name(const std::string &error_msg) : base_class_name(error_msg) {}                          \
                                                                                                                       \
            explicit class_name(std::string &&error_msg) : base_class_name(std::move(error_msg)) {}                    \
        };                                                                                                             \
    }


/*
THIS IS AN EXAMPLE USAGE.

NEW_BASE_EXCPETION(GameState);
NEW_INHERITED_EXCEPTION(GameStateConstructor, GameState);
NEW_INHERITED_EXCEPTION(DuplicateCards, GameStateConstructor);
NEW_INHERITED_EXCEPTION(CardCountMismatch, GameStateConstructor);
 */

NEW_BASE_EXCPETION(Logger);

// for gamestate
NEW_BASE_EXCPETION(GameState);
NEW_INHERITED_EXCEPTION(InsufficientFunds, GameState);
NEW_INHERITED_EXCEPTION(CardNotAvailable, GameState);
NEW_INHERITED_EXCEPTION(PlayerCountMismatch, GameState);
NEW_INHERITED_EXCEPTION(DuplicatePlayer, GameState);
NEW_INHERITED_EXCEPTION(WrongCardCount, GameState);
NEW_INHERITED_EXCEPTION(OutOfPhase, GameState);

// CLI parsing
NEW_BASE_EXCPETION(Cli);
NEW_INHERITED_EXCEPTION(InvalidArgument, Cli);
