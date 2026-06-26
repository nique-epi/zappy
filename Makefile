##
## EPITECH PROJECT, 2026
## zappy
## File description:
## Root Makefile — thin wrapper driving the CMake build so that `make`
## produces the zappy_server, zappy_gui and zappy_ai binaries at the repo root.
##

CMAKE		?= cmake
BUILD_DIR	?= build
JOBS		?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

BINARIES	:= zappy_server zappy_gui zappy_ai

CACHE		:= $(BUILD_DIR)/CMakeCache.txt

all: configure
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)

configure: $(CACHE)

$(CACHE):
	$(CMAKE) -S . -B $(BUILD_DIR) -DBUILD_BONUS=OFF -DBUILD_TESTS=OFF

zappy_server zappy_gui: configure
	$(CMAKE) --build $(BUILD_DIR) --target $@ -j $(JOBS)

zappy_ai: configure
	$(CMAKE) --build $(BUILD_DIR) --target zappy_ai_exec -j $(JOBS)

bonus:
	$(CMAKE) -S . -B $(BUILD_DIR) -DBUILD_BONUS=ON -DBUILD_TESTS=OFF
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)

tests_run:
	$(CMAKE) -S . -B $(BUILD_DIR) -DBUILD_TESTS=ON
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)
	ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	@if [ -d $(BUILD_DIR) ]; then $(CMAKE) --build $(BUILD_DIR) --target clean; fi

fclean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(BINARIES)

re: fclean all

.PHONY: all configure $(BINARIES) bonus tests_run clean fclean re
