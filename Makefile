# Project-specific defaults
target ?= Thor
compiler ?=
file ?=

# Build script (installed system-wide)
BUILD_SCRIPT := cbuild

# Include master makefile
include $(HOME)/.dotfiles/scripts/cmake.mk

# Add project-specific targets below

# Run the program
.PHONY: run
run:
	@echo "▶ Running $(target)..."
	@if [ -n "$(file)" ]; then \
		./build/$(target) "$(file)"; \
	else \
		./build/$(target); \
	fi
