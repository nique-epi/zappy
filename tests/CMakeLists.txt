add_executable(zappy_network_tests
  Network/mocks/MockSocket.cpp
  Network/TestClient.cpp
  Network/TestPollLoop.cpp
  Network/TestRealSocket.cpp
  Network/TestExceptions.cpp
  Network/TestConnection.cpp
  Network/TestClientLoop.cpp
)

target_include_directories(zappy_network_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/mocks
)

target_link_libraries(zappy_network_tests
  PRIVATE
    zappy_network
    GTest::gtest_main
)

add_executable(zappy_rpc_tests
  Network/mocks/MockSocket.cpp
  Rpc/mocks/LoopbackClient.cpp
  Rpc/TestTcpSocket.cpp
  Rpc/TestRPCServer.cpp
  Rpc/TestServerPreDispatch.cpp
  Rpc/TestWireFormat.cpp
  Rpc/TestResponseDispatcher.cpp
  Rpc/TestRPCClient.cpp
)

target_include_directories(zappy_rpc_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Rpc/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/mocks
)

target_link_libraries(zappy_rpc_tests
  PRIVATE
    zappy_rpc
    GTest::gtest_main
)

add_executable(zappy_schema_tests
  Schema/TestBoundedNumberFieldType.cpp
)

target_link_libraries(zappy_schema_tests
  PRIVATE
    zappy_schema
    GTest::gtest_main
)

add_executable(zappy_server_tests
  Cli/TestParserException.cpp
  Cli/TestArgsParser.cpp
  World/TestResourceType.cpp
  World/TestTile.cpp
  World/TestMap.cpp
  World/TestTeam.cpp
  World/TestTeamRegistry.cpp
  World/TestInventory.cpp
  World/TestPlayer.cpp
  World/TestPlayerRegistry.cpp
  World/TestHungerConstants.cpp
  World/TestDirection.cpp
  World/TestEjection.cpp
  Loot/TestResourceDensity.cpp
  Loot/TestLootService.cpp
  Scheduler/TestScheduler.cpp
  Scheduler/TestActionTiming.cpp
  Net/TestActionQueue.cpp
  Net/TestAiActionPipeline.cpp
  Net/TestGuiQueryHandlers.cpp
  App/TestActionCosts.cpp
  App/TestGameServerHandshake.cpp
  Rpc/mocks/LoopbackClient.cpp
  Network/mocks/MockSocket.cpp
)

target_include_directories(zappy_server_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Rpc/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/Network/mocks
)

target_link_libraries(zappy_server_tests
  PRIVATE
    zappy_server_app
    zappy_cli
    GTest::gtest_main
)

add_executable(zappy_gui_tests
  Gui/TestParserException.cpp
  Gui/TestArgsParser.cpp
)

target_link_libraries(zappy_gui_tests
  PRIVATE
    zappy_gui_cli
    zappy_cli
    GTest::gtest_main
)

add_executable(zappy_gui_handshake_tests
  Gui/TestServerHandshake.cpp
  Gui/mocks/FakeNetwork.cpp
)

target_include_directories(zappy_gui_handshake_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/mocks
)

target_link_libraries(zappy_gui_handshake_tests
  PRIVATE
    zappy_gui_handshake
    GTest::gtest_main
)

add_executable(zappy_gui_parser_tests
  Gui/TestMessageParser.cpp
)

target_link_libraries(zappy_gui_parser_tests
  PRIVATE
    zappy_gui_parser
    GTest::gtest_main
)

add_executable(zappy_gui_commands_tests
  Gui/TestCommandSender.cpp
  Gui/mocks/FakeNetwork.cpp
)

target_include_directories(zappy_gui_commands_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/mocks
)

target_link_libraries(zappy_gui_commands_tests
  PRIVATE
    zappy_gui_commands
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(zappy_network_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_rpc_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_schema_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_server_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_gui_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_gui_handshake_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_gui_parser_tests
  DISCOVERY_MODE PRE_TEST
)
gtest_discover_tests(zappy_gui_commands_tests
  DISCOVERY_MODE PRE_TEST
)

if(COVERAGE)
  add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E env bash
      ${CMAKE_CURRENT_SOURCE_DIR}/coverage.sh
    DEPENDS zappy_network_tests zappy_rpc_tests zappy_schema_tests zappy_server_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    USES_TERMINAL
    COMMENT "Running tests and generating llvm-cov coverage report..."
  )
endif()
