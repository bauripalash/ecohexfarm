PROJECT:=hexgame
BUILD_DIR:=build

.PHONY: all
all: run

.PHONY: run
run: build
	./$(BUILD_DIR)/$(PROJECT)/$(PROJECT)

.PHONY: build
build:
	cmake --build $(BUILD_DIR) --parallel

.PHONY: setup
setup:
	cmake -S . -B $(BUILD_DIR)

