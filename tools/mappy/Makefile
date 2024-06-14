DEBUG := 1
BUILD_DIR := build
TARGET := mappy
LIB := -lglfw -lGLEW -lGL -lcglm

INC_DIRS := include ../../include
SRC_DIRS := src
H_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

ifeq ($(DEBUG),1)
	OPT := -O0 -ggdb3
else
	OPT := -O3
endif
CFLAGS := $(OPT) -Wall -Wextra

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

BETTY_IGNORE := --ignore=CAMELCASE
BETTY_FLAGS := $(BETTY_IGNORE) -strict -subjective \
	       --show-types --allow-global-variables
BETTY_SCAN := $(H_FILES) $(C_FILES)

betty:
	betty-style $(BETTY_FLAGS) $(BETTY_SCAN)
