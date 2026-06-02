
configure_file(main.py zappy_ai COPYONLY)

add_custom_target(zappy_ai_exec ALL
    COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/zappy_ai
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/zappy_ai
    COMMENT "Making zappy_ai executable"
)
