DEBUG_ENABLED := 1
BUILD_DIR := out
TARGET := mqme

INC_DIRS := include
H_FILES := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))
SRC_DIRS := src
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -Ofast $(INC_DIRS:%=-I%)
ifeq ($(DEBUG_ENABLED), 1)
	CFLAGS += -ggdb3 -O0 -DDEBUG_ENABLED
endif
LIB := -lglfw -lGLEW -lGL -lcglm -lm

all: $(TARGET)

$(TARGET): $(O_FILES)
	@mkdir -p $(SRC_DIRS:%=$(BUILD_DIR)/%)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean format todo

clean:
	rm -rf $(BUILD_DIR) $(TARGET) compile_commands.json

format: $(H_FILES) $(C_FILES)
	@clang-format-15 --style=file -i $(filter-out include/nuklear.h include/nuklear_glfw_gl3.h,$^)

todo: $(H_FILES) $(C_FILES)
	grep --color=always -i "todo" $(filter-out include/nuklear.h include/nuklear_glfw_gl3.h include/stb_image.h,$^)
	grep --color=always -i "fixme" $(filter-out include/nuklear.h include/nuklear_glfw_gl3.h include/stb_image.h,$^)
