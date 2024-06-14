DEBUG     := 0
BUILD_DIR := build

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

MKFONT_FLAGS ?=
MKSPRITE_FLAGS ?=

INC_DIRS := include include/engine include/game
SRC_DIRS := src src/engine src/game
ifeq ($(DEBUG),1)
	INC_DIRS += include/debug
	SRC_DIRS += src/debug
endif
H_FILES  := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))
C_FILES  := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES  := $(C_FILES:%.c=$(BUILD_DIR)/%.o)
D_FILES  := $(O_FILES:%.o=%.d)

ASSETS_TTF  := $(wildcard assets/*.ttf)
ASSETS_PNG  := $(wildcard assets/*.png)
ASSETS_MAP  := $(wildcard assets/*.map)
ASSETS_CONV := $(ASSETS_TTF:assets/%.ttf=filesystem/%.font64) \
	       $(ASSETS_PNG:assets/%.png=filesystem/%.sprite) \
	       $(ASSETS_MAP:assets/%.map=filesystem/%.map) \

TARGET := modemquest
ELF    := $(BUILD_DIR)/$(TARGET).elf
DFS    := $(BUILD_DIR)/$(TARGET).dfs
ROM    := $(TARGET).z64

INC_FLAGS  := $(INC_DIRS:%=-I%)
N64_CFLAGS += $(INC_FLAGS)
ifeq ($(DEBUG),1)
	N64_CFLAGS += -g -O0 -DDEBUG -DDEBUG_MODE=1
	N64_LDFLAGS += -g
else
	N64_CFLAGS += -Os -DDEBUG_MODE=0
endif

default: $(ROM)
$(ROM): N64_ROM_TITLE="MODEMQUEST"
$(ROM): $(ELF)
$(ROM): $(DFS)
$(ELF): $(O_FILES)
$(DFS): $(ASSETS_CONV)

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.map: assets/%.map
	@mkdir -p $(dir $@)
	@echo "    [MAP] $@"
	cp $< $@

clean:
	rm -rf $(BUILD_DIR) $(ROM) filesystem/

BETTY_IGNORE := --ignore=CONSTANT_COMPARISON,REDUNDANT_CODE
BETTY_FLAGS  := -strict -subjective --show-types \
		--allow-global-variables $(BETTY_IGNORE)
BETTY_SCAN   := $(filter-out include/debug/debug.h include/types.h,\
		$(H_FILES)) \
		$(filter-out src/debug/debug.c,$(C_FILES))

betty:
	betty-style $(BETTY_FLAGS) $(BETTY_SCAN)

-include $(D_FILES)
