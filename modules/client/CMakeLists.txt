project(${CMAKE_PROJECT_NAME}/Client)

# Add external libraries if any
find_package(wxWidgets COMPONENTS core base net REQUIRED)

include(${wxWidgets_USE_FILE})
# add_subdirectory(external_libraries)

# Add the executable for the client
add_executable(client 
               src/main.cpp 
               src/dominion.cpp
               src/game_controller.cpp
               src/windows/game_window.cpp
               src/panels/connection_panel.cpp
               src/panels/main_game_panel.cpp
               src/uiElements/image_panel.cpp
               src/uiElements/input_field.cpp
                )

target_link_libraries(
  client
  ${wxWidgets_LIBRARIES}
)

file(INSTALL ../../assets DESTINATION ${CMAKE_BINARY_DIR})
# Link the client executable to the shared client_lib
#target_link_libraries(client client_lib shared)
