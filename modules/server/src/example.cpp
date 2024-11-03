#include <server/example.h>
#include <iostream>

namespace server {
    bool test() { return true; }

    void hello()
    {
        std::cout << "Hello from server!\n";
    }
}
