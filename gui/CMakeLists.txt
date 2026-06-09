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

add_executable(zappy_gui
    main.cpp
    Network/NetworkManager.cpp
    World/MockWorld.cpp
    Render/Raygui.cpp
)

set_source_files_properties(Render/Raygui.cpp PROPERTIES
    COMPILE_OPTIONS "-w"
)

target_link_libraries(zappy_gui PRIVATE zappy_gui_cli zappy_gui_handshake zappy_network raylib)

target_compile_options(zappy_gui PRIVATE -Wall -Wextra -Werror)
