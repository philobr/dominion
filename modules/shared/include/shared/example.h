#pragma once

namespace shared {

    void hello_from_shared();
    bool test();

    namespace core_types {
        struct Type1 {
            Type1();
        };
    };

} // namespace shared