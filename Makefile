SRC_DIR := ./src
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
TARGET := ./build

CC := cc
CC_FLAGS := -Wall -g

all: $(TARGET)

install: $(TARGET)
	@cp $(TARGET) /usr/local/bin/

clean:
	@rm -f $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CC_FLAGS) -o $@ $^