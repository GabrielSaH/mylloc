CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =
SRC_DIR = src
BUILD_DIR = build
TARGET = main.out

# List all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate object file names from source file names
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
