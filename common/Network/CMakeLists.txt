add_library(zappy_network STATIC
  Exceptions.cpp
  Socket/RealSocket.cpp
  Socket/TcpSocket.cpp
  Server/Client.cpp
  Server/PollLoop.cpp
  Client/Connection.cpp
  Client/ClientLoop.cpp
)

target_include_directories(zappy_network PUBLIC ${ZAPPY_COMMON_INCLUDE_DIR})
target_compile_options(zappy_network PRIVATE -Wall -Wextra -Werror)
