find_package(SFML 2.5 COMPONENTS graphics window system QUIET)

add_executable(zappy_gui main.cpp)

if(SFML_FOUND)
    target_link_libraries(zappy_gui PRIVATE sfml-graphics sfml-window sfml-system)
endif()

target_compile_options(zappy_gui PRIVATE -Wall -Wextra -Werror)
