CC ?= clang
CFLAGS ?= -std=c11 -Wall -Wextra -g

BUILD_DIR := build
TEST_BIN := $(BUILD_DIR)/test_runner

CORE_SRCS := sneknew.c snekobject.c vm.c stack/stack.c
TEST_SRCS := main.c munit/munit.c $(CORE_SRCS)

.PHONY: all test test-build clean

all: test-build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_BIN): $(TEST_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. $(TEST_SRCS) -o $(TEST_BIN)

test-build: $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
