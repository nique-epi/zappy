add_library(zappy_rpc STATIC
  Message/IMessage.cpp
  Message/WireFormat.cpp
  Message/RPCMessage.cpp
  Message/MessageParser.cpp
  Client/ResponseDispatcher.cpp
  Client/RPCClient.cpp
)

target_include_directories(zappy_rpc PUBLIC ${ZAPPY_COMMON_INCLUDE_DIR})
target_link_libraries(zappy_rpc PUBLIC zappy_schema zappy_network)
target_compile_options(zappy_rpc PRIVATE -Wall -Wextra -Werror)
