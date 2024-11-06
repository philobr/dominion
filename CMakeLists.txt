# root/cmakelists.txt

cmake_minimum_required(VERSION 3.22)
project(doMINION)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXTERNAL_LIBS ${CMAKE_SOURCE_DIR}/external)

enable_testing()

# always provide test output on failure
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

add_compile_options(-Wall -Wpedantic)

add_subdirectory(${EXTERNAL_LIBS}/sockpp)
add_subdirectory(${EXTERNAL_LIBS}/googletest) # warnings because of old cmake version, cant supress them

include_directories(external/sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
if (wxWidgets_FOUND)
    message(STATUS "Including wxWidgets for ${PROJECT_NAME}")
    include(${wxWidgets_USE_FILE})  
else()
    message(FATAL_ERROR "wxWidgets not found; ensure it is installed! (use './install_wx_widgets.sh')")
endif()

################################
# MACROS
################################
macro(include_rapidjson target)
    message(STATUS "Including rapidjson in ${target}")
    target_include_directories(${target} PRIVATE ${EXTERNAL_LIBS}/rapidjson/include)
endmacro()

macro(include_sockpp target)
    message(STATUS "Including sockpp in ${target}")
    target_link_libraries(${target} PRIVATE sockpp)

    if (WIN32)
        message(WARNING "why would you use windows??")
        target_link_libraries(${target} PRIVATE wsock32 ws2_32)
    else()
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
        target_link_libraries(${target} PRIVATE Threads::Threads)
    endif()
endmacro()

macro(include_gtest target)
    message(STATUS "Including gtest in ${target}")
    target_link_libraries(${target} PRIVATE gtest gtest_main)
    target_include_directories(${target} PRIVATE
        ${EXTERNAL_LIBS}/googletest/include
        ${EXTERNAL_LIBS}/googletest/lib
    )
endmacro()

macro(include_wxwidgets target)
    if (wxWidgets_FOUND)
        message(STATUS "Including wxWidgets in ${target}")
        target_link_libraries(${target} PRIVATE ${wxWidgets_LIBRARIES})
        target_include_directories(${target} PRIVATE ${wxWidgets_INCLUDE_DIRS})
        target_compile_options(${target} PRIVATE ${wxWidgets_CXX_FLAGS})  # Add wx-config flags
    else()
        message(FATAL_ERROR "wxWidgets not found; skipping ${target}")
    endif()
endmacro()

macro(include_library target library_name)
    message(STATUS "Including ${library_name} in ${target}")
    target_link_libraries(${target} PRIVATE ${library_name})
    string(REPLACE "_lib" "" library_dir ${library_name})
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/modules/${library_dir}/include)
endmacro()

macro(include_server_lib target)
    include_library(${target} server_lib)
endmacro()

macro(include_client_lib target)
    include_library(${target} client_lib)
endmacro()

macro(include_shared_lib target)
    include_library(${target} shared_lib)
endmacro()

################################
# MODULES
################################
add_subdirectory(modules/shared)
add_subdirectory(modules/client)
add_subdirectory(modules/server)
add_subdirectory(unit_tests)

################################
# EXECUTABLES
################################
add_executable(client_exe ${CLIENT_EXECUTABLE_SOURCES})
include_client_lib(client_exe)
include_shared_lib(client_exe)
include_sockpp(client_exe)
include_wxwidgets(client_exe)
include_rapidjson(client_exe)

add_executable(server_exe ${SERVER_EXECUTABLE_SOURCES})
include_shared_lib(server_exe)
include_server_lib(server_exe)
include_sockpp(server_exe)
include_wxwidgets(server_exe)
include_rapidjson(server_exe)

################################
# HELPERS
################################

add_custom_target(run_server
    COMMAND ${CMAKE_COMMAND} -E echo "Starting server..."
    COMMAND ${CMAKE_BINARY_DIR}/server_exe
    COMMENT "Running server_exe"
    USES_TERMINAL
)

add_custom_target(run_client
    COMMAND ${CMAKE_COMMAND} -E echo "Starting client..."
    COMMAND ${CMAKE_BINARY_DIR}/client_exe
    COMMENT "Running client_exe"
    USES_TERMINAL
)

add_custom_target(format
    COMMENT "Running clang-format and checking for formatting issues..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND find modules unit_tests -name '*.cpp' -o -name '*.h' | xargs clang-format -i
    COMMAND git diff --exit-code || (echo "Formatting issues found!" && exit 1)
)

add_custom_target(check
    COMMENT "Running cppcheck..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND cppcheck --version
    COMMAND cppcheck
        --enable=all
        --error-exitcode=1
        # The --max-configs option limits the number of paths (branches) that
        # cppcheck will analyze in complex functions.
        # Increasing it might give cppcheck more flexibility in analyzing complex code paths.
        --max-configs=100
        --suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck_suppressions
        modules unit_tests
)
