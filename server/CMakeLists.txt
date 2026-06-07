add_library(zappy_server_app STATIC
  Cli/ArgsParser.cpp
  Cli/ArgumentCursor.cpp
  Cli/Validators/OptionSchema.cpp
  Cli/Exceptions/ParserException.cpp
  App/World/Resources/ResourceType.cpp
  App/World/Tile/Tile.cpp
  App/World/Map/Map.cpp
  App/World/Exceptions/WorldException.cpp
  App/Scheduler/Scheduler.cpp
  App/Scheduler/ActionTiming.cpp
  App/Scheduler/Exceptions/SchedulerException.cpp
  App/GameServer.cpp
)

target_include_directories(zappy_server_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zappy_server_app PUBLIC zappy_protocol zappy_schema)
target_compile_options(zappy_server_app PRIVATE -Wall -Wextra -Werror)

add_executable(zappy_server
  main.cpp
)

target_link_libraries(zappy_server PRIVATE zappy_server_app)
target_compile_options(zappy_server PRIVATE -Wall -Wextra -Werror)
