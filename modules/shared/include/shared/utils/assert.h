#pragma once

#include <shared/utils/colors.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>
#include <type_traits>

namespace utils {
    // functions inside this namesapce are only visible inside this file
    namespace assert_helpers {
        template <typename T, typename = void>
        struct is_container : std::false_type { };

        // checks if T has begin/end functions to determine whether its a container
        // explicitly excludes std::string and std::string_view, as those already provide '<<' overlaods
        template <typename T>
        struct is_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
            : std::integral_constant<bool, !std::is_same_v<T, std::string> && !std::is_same_v<T, std::string_view>> { };

        template <typename Container>
        std::enable_if_t<is_container<Container>::value, std::ostream&> operator<<(std::ostream& os,
                                                                                    const Container& container)
        {
            os << "[";
            bool first = true;
            for ( const auto& elem : container ) {
                if ( !first )
                    os << ", ";
                os << elem;
                first = false;
            }
            return os << "]";
        }

        template <typename T1, typename T2>
        void log_assertion_failure(const T1& actual, const T2& expected, std::string_view op, std::string_view msg,
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

        template <typename T1, typename T2>
        void custom_assert(const T1& actual, const T2& expected, std::string_view op, std::string_view msg,
                           std::string_view file, int line, std::string_view function)
        {
#ifndef NDEBUG
            if constexpr ( is_container<T1>::value && is_container<T2>::value ) {
                if ( actual.size() != expected.size() ) {
                    log_assertion_failure(actual, expected, op, msg, file, line, function);
                    std::abort();
                }

                auto it1 = actual.begin();
                auto it2 = expected.begin();
                using CommonType = std::common_type_t<typename T1::value_type, typename T2::value_type>;

                bool all_equal = std::equal(it1, actual.end(), it2, expected.end(),
                                            [ ] (const auto& a, const auto& b)
                {
                    return static_cast<CommonType>(a) == static_cast<CommonType>(b);
                });

                if ( !all_equal ) {
                    log_assertion_failure(actual, expected, op, msg, file, line, function);
                    std::abort();
                }
            }
            else {
                if ( !(static_cast<std::common_type_t<T1, T2>>(actual) == static_cast<std::common_type_t<T1, T2>>(expected)) ) {
                    log_assertion_failure(actual, expected, op, msg, file, line, function);
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
#define ASSERT_TRUE(expr, msg)                                                                                         \
    utils::assert_helpers::custom_assert((expr), true, "==", (msg), __FILE__, __LINE__, __func__)

 /**
  * @brief Asserts (!expr)
  */
#define ASSERT_FALSE(expr, msg)                                                                                        \
    utils::assert_helpers::custom_assert((expr), false, "==", (msg), __FILE__, __LINE__, __func__)

  /**
   * @brief Asserts (actual == expected)
   * Also works for containers
   */
#define ASSERT_EQ(actual, expected, msg)                                                                               \
    utils::assert_helpers::custom_assert((actual), (expected), "==", (msg), __FILE__, __LINE__, __func__)

   /**
    * @brief Asserts (actual != expected)
    * Also works for containers
    */
#define ASSERT_NEQ(actual, expected, msg)                                                                              \
    utils::assert_helpers::custom_assert((actual), (expected), "!=", (msg), __FILE__, __LINE__, __func__)

    /**
     * @brief less than (actual < expected)
     */
#define ASSERT_LT(actual, expected, msg)                                                                               \
    utils::assert_helpers::custom_assert((actual), (expected), "<", (msg), __FILE__, __LINE__, __func__);

     /**
      * @brief greater than (actual > expected)
      */
#define ASSERT_GT(actual, expected, msg)                                                                               \
    utils::assert_helpers::custom_assert((actual), (expected), ">", (msg), __FILE__, __LINE__, __func__);

      /**
       * @brief less equal (actual <= expected)
       */
#define ASSERT_LE(actual, expected, msg)                                                                               \
    utils::assert_helpers::custom_assert((actual), (expected), "<=", (msg), __FILE__, __LINE__, __func__)

       /**
        * @brief greater equal (actual >= expected)
        */
#define ASSERT_GE(actual, expected, msg)                                                                               \
    utils::assert_helpers::custom_assert((actual), (expected), ">=", (msg), __FILE__, __LINE__, __func__);
