#pragma once

#include <cxxabi.h> // to demangle typeids
#include <memory>
#include <string>

namespace utils
{
    inline std::string demangle(const char *mangled_name)
    {
        int status = 0;
        std::unique_ptr<char, void (*)(void *)> demangled_name(
                abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status), std::free);
        return (status == 0) ? demangled_name.get() : mangled_name;
    }

} // namespace utils