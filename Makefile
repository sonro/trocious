# Compiler and standard
CC      = clang
CSTD    = c23

# Directories
SRC_DIR     = src
INCLUDE_DIR = include
TEST_DIR    = tests
OBJ_DIR     = out/obj
LIB_DIR     = out/lib
BIN_DIR     = out/bin

# Targets
LIB_NAME    = libtrocious.a
LIB_TARGET  = $(LIB_DIR)/$(LIB_NAME)
TEST_TARGET = $(BIN_DIR)/test

# Flags
CFLAGS  = -std=$(CSTD) -Wall -Wextra -Wpedantic -g \
		  -I$(INCLUDE_DIR) -I$(SRC_DIR) \
		  -O3 
ARFLAGS = rcs

# Files
SRC_FILES   = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES   = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_FILES  = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ    = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.test.o,$(TEST_FILES))

# Default target
.PHONY: all
all: $(LIB_TARGET)

# Build library
$(LIB_TARGET): $(OBJ_FILES) | $(LIB_DIR)
	@echo "Archiving $@"
	@$(AR) $(ARFLAGS) $@ $^

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile test sources
$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Build test binary
.PHONY: test-build
test-build: $(TEST_TARGET)

$(TEST_TARGET): $(OBJ_FILES) $(TEST_OBJ) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@

# Run tests
.PHONY: test
test: test-build
	@$(TEST_TARGET)

# Utility targets
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

.PHONY: rebuild
rebuild: clean all

