DEBUG_ENABLED := 1
BUILD_DIR := build

include $(N64_INST)/include/n64.mk

INC_DIRS := include include/engine include/game
INC_FLAGS := $(INC_DIRS:%=-I%)
SRC_DIRS := src src/engine src/game
H_FILES := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

TARGET := modemquest
TARGET_STR := "ModemQuest"
ROM := $(TARGET).z64
ELF := $(BUILD_DIR)/$(TARGET).elf
DFS := $(BUILD_DIR)/$(TARGET).dfs
N64_CFLAGS += -Wall -Wextra -Werror -Os $(INC_FLAGS)
ifeq ($(DEBUG_ENABLED),1)
	N64_CFLAGS += -DDEBUG_ENABLED
endif

ASSETS_PNG := $(wildcard assets/*.png)
ASSETS_WAV := $(wildcard assets/*.wav)
ASSETS_TTF := $(wildcard assets/*.ttf)
ASSETS_MAP := $(wildcard assets/*.map)
ASSETS_CONV := \
	$(ASSETS_PNG:assets/%.png=filesystem/%.sprite) \
	$(ASSETS_WAV:assets/%.wav=filesystem/%.wav64) \
	$(ASSETS_TTF:assets/%.ttf=filesystem/%.font64) \
	$(ASSETS_MAP:assets/%.map=filesystem/%.map)

final: $(ROM)
$(ROM): N64_ROM_TITLE=$(TARGET_STR)
$(ROM): $(DFS) 
$(DFS): $(ASSETS_CONV)
$(ELF): $(O_FILES)

AUDIOCONV_FLAGS := --wav-compress 1
MKSPRITE_FLAGS := --compress 1
MKFONT_FLAGS := --compress 1
MKASSET_FLAGS := --compress 1

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o filesystem "$<"

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

filesystem/%.map: assets/%.map
	@mkdir -p $(dir $@)
	@echo "    [MQ-MAP] $@"
	cp $< $@
	$(N64_BINDIR)/mkasset $(MKASSET_FLAGS) -o filesystem $@

clean:
	rm -rf $(ROM) $(BUILD_DIR) filesystem

format: $(H_FILES) $(C_FILES)
	clang-format-15 --style=file -i $^

-include $(wildcard $(BUILD_DIR)/*.d)
