# unit_tests/client_tests/CMakeLists.txt

add_executable(client_tests
    test.cpp # placeholder
)

include_gtest(client_tests)
include_gmock(client_tests)
include_client_lib(client_tests)
include_shared_lib(client_tests)

add_test(NAME ClientTests COMMAND server_tests)
