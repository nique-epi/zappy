
configure_file(main.py ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_ai COPYONLY)

add_custom_target(zappy_ai_exec ALL
    COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_ai
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_ai
    COMMENT "Making zappy_ai executable"
)
