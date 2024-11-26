#pragma once

#include <shared/utils/colors.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>
#include <type_traits>

namespace utils
{
    // Functions inside this namespace are only visible inside this file
    namespace assert_helpers
    {
        template <typename T, typename = void>
        struct IsContainer : std::false_type
        {};

        // Checks if T has begin/end functions to determine whether it's a container
        // Explicitly excludes std::string and std::string_view, as those already provide '<<' overloads
        template <typename T>
        struct IsContainer<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
            : std::integral_constant<bool, !std::is_same_v<T, std::string> && !std::is_same_v<T, std::string_view>>
        {};

        // Overload operator<< for containers
        template <typename Container>
        std::enable_if_t<IsContainer<Container>::value, std::ostream &> operator<<(std::ostream &os,
                                                                                    const Container &container)
        {
            os << "[";
            bool first = true;
            for ( const auto &elem : container ) {
                if ( !first ) {
                    os << ", ";
                }
                os << elem;
                first = false;
            }
            return os << "]";
        }

        template <typename T1, typename T2>
        void logAssertionFailure(const T1 &actual, const T2 &expected, std::string_view op, std::string_view msg,
                                   std::string_view file, int line, std::string_view function)
        {
            std::cerr << utils::colors::RED << "Assertion failed (" << op << "): " << utils::colors::RESET << std::endl
                      << utils::colors::BLUE << "Message:   " << utils::colors::RESET << msg << std::endl
                      << utils::colors::BLUE << "Expected:  " << utils::colors::RESET << expected << std::endl
                      << utils::colors::BLUE << "Actual:    " << utils::colors::RESET << actual << std::endl
                      << utils::colors::GREEN << "File:      " << utils::colors::RESET << file << std::endl
                      << utils::colors::GREEN << "Line:      " << utils::colors::RESET << line << std::endl
                      << utils::colors::GREEN << "Function:  " << utils::colors::RESET << function << std::endl;
        }

        template <typename T1, typename T2, typename Compare>
        void customAssert(const T1 &actual, const T2 &expected, Compare comp, std::string_view op,
                           std::string_view msg, std::string_view file, int line, std::string_view function)
        {
#ifndef NDEBUG
            if constexpr ( IsContainer<T1>::value && IsContainer<T2>::value ) {
                if ( op == "==" || op == "!=" ) {
                    bool result = std::equal(actual.begin(), actual.end(), expected.begin(), expected.end());
                    if ( (op == "==" && !result) || (op == "!=" && result) ) {
                        log_assertion_failure(actual, expected, op, msg, file, line, function);
                        std::abort();
                    }
                } else {
                    using SizeType = typename T1::size_type;
                    SizeType actual_size = actual.size();
                    SizeType expected_size = expected.size();
                    if ( !comp(actual_size, expected_size) ) {
                        log_assertion_failure(actual_size, expected_size, op, msg, file, line, function);
                        std::abort();
                    }
                }
            } else {
                using CommonType = std::common_type_t<T1, T2>;
                if ( !comp(static_cast<CommonType>(actual), static_cast<CommonType>(expected)) ) {
                    logAssertionFailure(actual, expected, op, msg, file, line, function);
                    std::abort();
                }
            }
#endif
        }
    } // namespace assert_helpers

} // namespace utils

/**
 * @brief Asserts (expr)
 */
#define _ASSERT_TRUE(expr, msg)                                                                                        \
    utils::assert_helpers::customAssert((expr), true, [](const auto &a, const auto &b) { return a == b; },            \
                                         "==", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (!expr)
 */
#define _ASSERT_FALSE(expr, msg)                                                                                       \
    utils::assert_helpers::customAssert((expr), false, [](const auto &a, const auto &b) { return a == b; },           \
                                         "==", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual == expected)
 * Also works for containers
 */
#define _ASSERT_EQ(actual, expected, msg)                                                                              \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a == b; },    \
                                         "==", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual != expected)
 * Also works for containers
 */
#define _ASSERT_NEQ(actual, expected, msg)                                                                             \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a != b; },    \
                                         "!=", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual < expected)
 */
#define _ASSERT_LT(actual, expected, msg)                                                                              \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a < b; },     \
                                         "<", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual > expected)
 */
#define _ASSERT_GT(actual, expected, msg)                                                                              \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a > b; },     \
                                         ">", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual <= expected)
 */
#define _ASSERT_LE(actual, expected, msg)                                                                              \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a <= b; },    \
                                         "<=", (msg), __FILE__, __LINE__, __func__)

/**
 * @brief Asserts (actual >= expected)
 */
#define _ASSERT_GE(actual, expected, msg)                                                                              \
    utils::assert_helpers::customAssert((actual), (expected), [](const auto &a, const auto &b) { return a >= b; },    \
                                         ">=", (msg), __FILE__, __LINE__, __func__)
