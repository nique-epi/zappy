add_library(zappy_cli STATIC
    ArgumentCursor.cpp
    Exceptions/ParserException.cpp
)

target_include_directories(zappy_cli PUBLIC
    ${ZAPPY_COMMON_INCLUDE_DIR}
)

target_compile_options(zappy_cli PRIVATE -Wall -Wextra -Werror)
