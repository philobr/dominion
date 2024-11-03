#include <shared/example.h>

#include <iostream>

namespace shared {
    void hello_from_shared()
    {
        std::cout << "hello from shared, used in " << __FILE__ << std::endl;
    }

    bool test()
    {
        return true;
    }

    namespace core_types {
        Type1::Type1()
        {
            std::cout << "u get a type1\n";
        }
    };

} // namespace shared