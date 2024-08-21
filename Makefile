TEST_CFLAGS= \
		-std=c23\
		-Wall\
		-Werror\
		-Wextra\
		-Wpedantic\
		-Wno-gnu\
		-Wno-missing-braces\
		-Iinclude\
		-g

TEST_LDFLAGS= \
		-Lout/lib\
	 	-ltrocious

LIB_CFLAGS= \
		-std=c23\
		-Wall\
		-Werror\
		-Wextra\
		-Wpedantic\
		-Wno-gnu\
		-Wno-missing-braces\
		-Iinclude\
		-g

MKDIR=mkdir -p
RM=rm -f

SRC_DIR=src
TEST_DIR=tests
INC_DIR=include
OUT_DIR=out
OBJ_DIR=out/obj
BIN_DIR=out/bin
LIB_DIR=out/lib

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_MAIN = $(TEST_DIR)/run.c

# Include files
INC_FILES = $(wildcard $(INC_DIR)/*.h)

# Object files
SRC_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_FILES))

# Targets
LIB_TARGET = $(LIB_DIR)/libtrocious.a
TEST_TARGET = $(BIN_DIR)/test

.PHONY: build
build: $(LIB_TARGET)

# Ensure directories exist
$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)

# Build the library
$(LIB_TARGET): $(SRC_OBJ_FILES) | $(LIB_DIR)
	$(AR) -cvrs $(LIB_TARGET) $(SRC_OBJ_FILES)

# Build the test binary
$(TEST_TARGET): $(LIB_TARGET) $(TEST_OBJ_FILES) | $(BIN_DIR)
	$(CC) $(TEST_OBJ_FILES) $(TEST_LDFLAGS) -o $@

# Compile all source files from src into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES) | $(OBJ_DIR)
	$(CC) $(LIB_CFLAGS) -c $< -o $@

# Compile all source files from tests into object files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(INC_FILES) | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Clean up build artifacts
.PHONY: clean
clean:
	$(RM) -r $(OUT_DIR)

.PHONY: build-test
build-test: $(TEST_TARGET)

.PHONY: test
test: build-test
	$(TEST_TARGET)

