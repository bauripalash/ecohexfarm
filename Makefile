PROJECT:=ecohex
BUILD_DIR:=build
HEADERS=$(shell find src/ -path 'src/gen' -prune -o -path 'src/external' -prune -o -path 'src/tmpl' -prune -o -name '*.h' -print)
SOURCES=$(shell find src/ -path 'src/gen' -prune -o -path 'src/external' -prune -o -path 'src/tmpl' -prune -o -name '*.c' -print)

.PHONY: all
all: run

.PHONY: run
run: build
	cd $(BUILD_DIR)/$(PROJECT) && ./$(PROJECT)

.PHONY: build
build:
	cmake --build $(BUILD_DIR) --parallel

.PHONY: setup
setup:
	cmake -S . -B $(BUILD_DIR)

.PHONY: fmt
fmt:
	@clang-format -i -style=file --verbose $(SOURCES) $(HEADERS)

