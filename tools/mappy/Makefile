DEBUG := 1
BUILD_DIR := build
TARGET := mappy
LIB := -lglfw -lGLEW -lGL -lcglm

INC_DIRS := include ../../include
SRC_DIRS := src
H_FILES := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

ifeq ($(DEBUG),1)
	OPT := -O0 -ggdb3
else
	OPT := -Ofast
endif
CFLAGS := $(OPT) -Wall -Wextra -Werror -pedantic -std=c99

final: $(TARGET)

$(TARGET): $(O_FILES)
	@echo "    [LD] $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LIB)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "    [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_DIRS:%=-I%)

clean:
	@echo "Clean Previous Build"
	@rm -rf $(TARGET) $(BUILD_DIR)

format: $(H_FILES) $(C_FILES)
	@echo Formatting Code...
	@clang-format --style=file -i $(filter-out include/nuklear.h,$^)
