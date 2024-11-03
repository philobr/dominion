#pragma once

#include <string_view>

namespace utils {
    namespace colors {
        inline constexpr std::string_view RED = "\033[31m";
        inline constexpr std::string_view GREEN = "\033[32m";
        inline constexpr std::string_view BLUE = "\033[34m";
        inline constexpr std::string_view RESET = "\033[0m";
    } // namespace colors

} // namespace utils