#include <shared/example.h>

#include <iostream>

namespace shared
{

    bool test() { return true; }

    namespace core_types
    {
        Type1::Type1() { std::cout << "u get a type1\n"; }
    }; // namespace core_types

} // namespace shared
