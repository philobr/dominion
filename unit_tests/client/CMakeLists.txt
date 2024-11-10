add_executable(client_tests
    test.cpp
)

include_gtest(client_tests)
include_shared_lib(client_tests)
include_client_lib(client_tests)

add_test(NAME ClientTests COMMAND client_tests)
