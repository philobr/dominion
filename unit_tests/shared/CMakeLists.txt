add_executable(shared_tests
    test.cpp
    # add testfiles here
)

include_gtest(shared_tests)
include_shared_lib(shared_tests)

add_test(NAME SharedTests COMMAND shared_tests)
