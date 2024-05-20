BUILD_DIR := build

include $(N64_INST)/include/n64.mk

MKFONT_FLAGS   ?=
MKSPRITE_FLAGS ?=

SRC_DIRS := src src/engine src/game
H_FILES  := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))
C_FILES  := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES  := $(C_FILES:%.c=$(BUILD_DIR)/%.o)
D_FILES  := $(O_FILES:%.o=%.d)

ASSETS_TTF  := $(wildcard assets/*.ttf)
ASSETS_PNG  := $(wildcard assets/*.png)
ASSETS_CONV := $(ASSETS_TTF:assets/%.ttf=filesystem/%.font64) \
	       $(ASSETS_PNG:assets/%.png=filesystem/%.sprite)

TARGET := modemquest
ELF    := $(BUILD_DIR)/$(TARGET).elf
DFS    := $(BUILD_DIR)/$(TARGET).dfs
ROM    := $(TARGET).z64

N64_CFLAGS += -std=gnu99 -Os -Iinclude

default: $(ROM)
$(ROM): N64_ROM_TITLE="MODEMQUEST"
$(ROM): $(DFS)
$(DFS): $(ASSETS_CONV)
$(ELF): $(O_FILES)

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

clean:
	rm -rf $(BUILD_DIR) $(ROM)

-include $(D_FILES)

BETTY_IGNORE := --ignore=PREFER_KERNEL_TYPES
BETTY_FLAGS  := -strict -subjective --show-types \
		--allow-global-variables $(BETTY_IGNORE)
BETTY_SCAN   := $(H_FILES) $(C_FILES)

betty:
	betty-style $(BETTY_FLAGS) $(BETTY_SCAN)
