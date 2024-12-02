# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -Iinclude/internal # For source files (includes 'include' and 'internal')
TEST_CFLAGS = -Wall -Iinclude# For test files (includes only 'include', not 'internal')

# Directories
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

# Static library name
LIB_NAME = libclpyto.a

# Find all .c files in src directory
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Generate object files from source files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Find all .c files in tests directory
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Generate base names for test executables (without .c extension)
TEST_BASENAMES = $(notdir $(TEST_FILES:.c=))

# Default target to build the library and tests
all: $(LIB_NAME) tests

# Rule to create the static library
$(LIB_NAME): $(OBJ_FILES)
	ar rcs $@ $^

# Rule to compile .c files to .o object files (for source files)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Ensure obj/ directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile and link each test file directly into an executable (no object files for tests)
tests: $(TEST_DIR)/bin/$(TEST_BASENAMES)

# Rule to compile and link each test file into its own executable
$(TEST_DIR)/bin/%: $(TEST_DIR)/%.c $(LIB_NAME)
	@mkdir -p $(TEST_DIR)/bin  # Ensure the bin directory exists for test executables
	$(CC) $(TEST_CFLAGS) $< -L. -lclpyto -o $@

# Clean up object files, library, and test executables
clean:
	rm -rf $(OBJ_DIR) $(LIB_NAME) $(TEST_DIR)/bin/*

# Phony targets
.PHONY: all clean tests
