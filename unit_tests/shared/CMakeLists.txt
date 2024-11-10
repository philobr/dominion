add_executable(shared_tests
    game_state.cpp
    message_types/constructors.cpp
    message_types/equality.cpp
    message_types/json_conversion.cpp
)

include_gtest(shared_tests)
include_shared_lib(shared_tests)

add_test(NAME SharedTests COMMAND shared_tests)
