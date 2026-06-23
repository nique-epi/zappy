include(FetchContent)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

set(RAYGUI_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/Render/raygui.h)
if(NOT EXISTS ${RAYGUI_HEADER})
    message(STATUS "Downloading raygui.h...")
    file(DOWNLOAD
        https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h
        ${RAYGUI_HEADER}
        STATUS RAYGUI_DOWNLOAD_STATUS
    )
    list(GET RAYGUI_DOWNLOAD_STATUS 0 RAYGUI_DOWNLOAD_CODE)
    if(NOT RAYGUI_DOWNLOAD_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download raygui.h — place it manually in gui/Render/raygui.h")
    endif()
endif()

add_library(zappy_gui_cli STATIC
    Exceptions/GuiException.cpp
    Cli/ArgsParser.cpp
)

target_include_directories(zappy_gui_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zappy_gui_cli PUBLIC zappy_cli zappy_schema)

target_compile_options(zappy_gui_cli PRIVATE -Wall -Wextra -Werror)

add_library(zappy_gui_handshake STATIC
    Network/ServerHandshake.cpp
    Exceptions/HandshakeException.cpp
)

target_include_directories(zappy_gui_handshake PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zappy_gui_handshake PUBLIC zappy_gui_cli)

target_compile_options(zappy_gui_handshake PRIVATE -Wall -Wextra -Werror)

add_library(zappy_gui_commands STATIC
    Network/CommandSender.cpp
)

target_include_directories(zappy_gui_commands PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zappy_gui_commands PUBLIC zappy_gui_cli)
target_link_libraries(zappy_gui_commands PRIVATE zappy_protocol)

target_compile_options(zappy_gui_commands PRIVATE -Wall -Wextra -Werror)

add_library(zappy_gui_parser STATIC
    Network/Parsing/MessageParser.cpp
    Network/Parsing/ParsingUtils.cpp
    Network/Parsing/MapHandlers.cpp
    Network/Parsing/PlayerHandlers.cpp
    Network/Parsing/EggHandlers.cpp
    Network/Parsing/ServerHandlers.cpp
)

target_include_directories(zappy_gui_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zappy_gui_parser PUBLIC zappy_gui_cli)

target_compile_options(zappy_gui_parser PRIVATE -Wall -Wextra -Werror)

add_library(zappy_gui_world_init STATIC
    World/WorldInitializer.cpp
    Exceptions/WorldInitException.cpp
)

target_include_directories(zappy_gui_world_init PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(zappy_gui_world_init PUBLIC zappy_gui_cli zappy_gui_parser)
target_link_libraries(zappy_gui_world_init PRIVATE zappy_protocol)

target_compile_options(zappy_gui_world_init PRIVATE -Wall -Wextra -Werror)

add_executable(zappy_gui
    main.cpp
    Network/NetworkManager.cpp
    World/MockWorld.cpp
    Render/Raygui.cpp
    Render/RenderUtils.cpp
    Render/Entity/EggRenderer.cpp
    Render/Entity/PlayerRenderer.cpp
    Render/SpeedControl.cpp
    Render/WorldCamera.cpp
    Render/TileGridRenderer.cpp
    Render/Panel/HudPanel.cpp
    Render/Panel/InfoPanel.cpp
)

set_source_files_properties(Render/Raygui.cpp PROPERTIES
    COMPILE_OPTIONS "-w"
)

set_source_files_properties(Render/WorldCamera.cpp PROPERTIES
    COMPILE_OPTIONS "-Wno-missing-field-initializers"
)

target_link_libraries(zappy_gui PRIVATE zappy_gui_cli zappy_gui_handshake zappy_gui_commands zappy_gui_parser zappy_gui_world_init zappy_network raylib)

target_compile_options(zappy_gui PRIVATE -Wall -Wextra -Werror)
