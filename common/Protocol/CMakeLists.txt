add_library(zappy_protocol STATIC
  GuiProtocol.cpp
  AiProtocol.cpp
)

target_include_directories(zappy_protocol PUBLIC ${ZAPPY_COMMON_INCLUDE_DIR})
target_link_libraries(zappy_protocol PUBLIC zappy_rpc)
target_compile_options(zappy_protocol PRIVATE -Wall -Wextra -Werror)
