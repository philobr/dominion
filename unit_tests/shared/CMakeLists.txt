add_executable(shared_tests
    message_types/constructors.cpp
    message_types/equality.cpp
    message_types/json_conversion.cpp
    game_state.cpp
)

include_gtest(shared_tests)
include_shared_lib(shared_tests)

add_test(NAME SharedTests COMMAND shared_tests)
