.PHONY: all clean build run rebuild test

# Project configuration
PROJECT_NAME = Krypton
BUILD_DIR = build

# Default target
all: build

# Create build directory and generate CMake files
cmake_init:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..

# Build the project
build: cmake_init
	@echo "Building $(PROJECT_NAME)..."
	@cd $(BUILD_DIR) && make -j$(shell nproc)
	@echo "Build complete!"

# Clean build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(PROJECT_NAME)
	@echo "Clean complete!"

# Rebuild the project
rebuild: clean build

# Run the tests
test:
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)_test
	@echo "Tests complete!"

# Run the program
run:
	@echo "Running $(PROJECT_NAME)..."
	@if [ -n "$(file)" ]; then \
		./$(PROJECT_NAME) "$(file)"; \
	else \
		./$(PROJECT_NAME); \
	fi

# Show help
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make clean    - Clean build files"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make test     - Run tests"
	@echo "  make run      - Run the interpreter in interactive mode"
	@echo "  make run file=<path> - Run the interpreter with a specific file"
	@echo "  make help     - Show this help message"
