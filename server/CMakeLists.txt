add_executable(zappy_server
  main.cpp
  App/ArgsParser.cpp
  App/GameServer.cpp
)

target_include_directories(zappy_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zappy_server PRIVATE zappy_protocol)
target_compile_options(zappy_server PRIVATE -Wall -Wextra -Werror)
