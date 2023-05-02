# ===================
# Variable definition
# ===================
MAKEFILE := $(lastword $(MAKEFILE_LIST))
DOCUMENTATION := README.md CONTRIBUTING.md

# ----------------
# Folder variables
# ----------------
SRC_DIR := src
HEADER_DIR := include
TEST_DIR := tests

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_BUILD_DIR := $(BUILD_DIR)/tests
TEST_OBJ_DIR := $(TEST_BUILD_DIR)/obj

# ---------------
# Target variable
# ---------------
TARGET := $(BUILD_DIR)/creator $(BUILD_DIR)/producer $(BUILD_DIR)/consumer $(BUILD_DIR)/finalizer

# ---------------------
# Source file variables
# ---------------------
TARGET_SRC := $(TARGET:$(BUILD_DIR)/%=$(SRC_DIR)/%.c)
SRCS := $(filter-out $(TARGET_SRC), $(shell find $(SRC_DIR) -name '*.c' 2> /dev/null))
HEADERS := $(shell find $(HEADER_DIR) -name '*.h' 2> /dev/null)
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.c' 2> /dev/null)

# -------------------
# Byproduct variables
# -------------------
TARGET_OBJS := $(TARGET:$(BUILD_DIR)/%=$(OBJ_DIR)/%.o)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d) $(TARGET_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

.SECONDARY: $(OBJS) $(TEST_OBJS) $(TARGET_OBJS) $(DEPS)

# --------------
# Test variables
# --------------
TEST_TARGET := $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%)

# ----------------------
# Distribution variables
# ----------------------
DIST := Abreu-Guevara-Ortiz-Yip.tgz

# ---------------------
# Compilation variables
# ---------------------
CC := clang
CFLAGS += -Wall -Wextra -Wpedantic \
					-Wformat=2 -Wno-unused-parameter -Wshadow \
					-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
					-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -std=gnu11
CPPFLAGS += -I$(HEADER_DIR) -MMD -MP
LDLIBS += -lm -pthread
LDFLAGS +=

# =================
# Compilation rules
# =================
.PHONY: all
all: $(TARGET)

.PHONY: all_tests
all_tests: $(TEST_TARGET)

.PHONY: dist
dist: $(DIST)

$(BUILD_DIR)/%: $(OBJ_DIR)/%.o $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(TEST_BUILD_DIR)/%: LDLIBS += -lcriterion
$(TEST_BUILD_DIR)/%: $(TEST_OBJ_DIR)/%.o $(OBJS) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)/%.d | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(TEST_OBJ_DIR)/%.d | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# See https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
$(DEPS):

# =================
# Distribution rule
# =================
$(DIST): $(SRC_DIR) $(HEADER_DIR) $(TEST_DIR) $(MAKEFILE) $(DOCUMENTATION)
	mkdir $(basename $@)
	cp -r $^ $(basename $@)
	tar -zcvf $@ $(basename $@)
	rm -r $(basename $@)

# ========================
# Directory creation rules
# ========================
$(SRC_DIR) $(HEADER_DIR) $(TEST_DIR) $(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR) $(TEST_BUILD_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

# ========================
# Pseudo-target definition
# ========================
.PHONY: test
test: $(TEST_TARGET)
	for test_file in $^; do ./$$test_file; done

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) -f $(DIST)

.PHONY: install-hooks
install-hooks:
	pre-commit install
	pre-commit install --hook-type commit-msg

.PHONY: run-hooks
run-hooks:
	pre-commit run --all-files

.PHONY: lint
lint:
	clang-tidy $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS)

.PHONY: format
format: $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS)
	for file in $^; do clang-format -i $$file; done

# -------------
-include $(wildcard $(DEPS))
