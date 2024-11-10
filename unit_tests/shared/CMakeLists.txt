add_executable(shared_tests
    json_conversion.cpp
    # Add more test files here
)

include_gtest(shared_tests)
include_shared_lib(shared_tests)

add_test(NAME SharedTests COMMAND shared_tests)
