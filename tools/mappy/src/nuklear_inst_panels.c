#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tilemap.h"
#include "nuklear_inst.h"

#define SAVE_TIMER_MAX 2.5f

static int just_saved;
static float save_timer;

int npc_name_buf_state;
npc_t npc_selected = { 0 };

static void nuklear_inst_panel_project_buttons(const char *outpath)
{
	if (nk_button_label(nkctx, "NEW")) {
		tilemap_load_mappy(NULL);
	}
	if (nk_button_label(nkctx, just_saved ? "SAVED!" : "SAVE")) {
		if (!just_saved) {
			tilemap_save(outpath);
			just_saved = 1;
			save_timer = 0.0f;
		}
	}
	if (nk_button_label(nkctx, "SAVE & QUIT")) {
		tilemap_save(outpath);
		exit(EXIT_SUCCESS);
	}
	if (nk_button_label(nkctx, "QUIT"))
		exit(EXIT_SUCCESS);
}

void nuklear_inst_panel_project(const char *outpath, const float dt)
{
	int tilemap_width_prop = tilemap_width;
	int tilemap_height_prop = tilemap_height;

	nk_layout_row_dynamic(nkctx, 16, 2);
	nk_property_int(nkctx, "WIDTH:", 1, &tilemap_width_prop,
			TILEMAP_WIDTH_MAX, 1, 1);
	nk_property_int(nkctx, "HEIGHT:", 1, &tilemap_height_prop,
			TILEMAP_HEIGHT_MAX, 1, 1);
	nk_layout_row_dynamic(nkctx, 16, 4);
	nuklear_inst_panel_project_buttons(outpath);
	if (just_saved) {
		save_timer += dt;
		if (save_timer >= SAVE_TIMER_MAX) {
			save_timer = 0.0f;
			just_saved = 0;
		}
	}

	tilemap_width = (uint16_t)tilemap_width_prop;
	tilemap_height = (uint16_t)tilemap_height_prop;
	if (tilemap_width_prop > 0 && tilemap_height_prop > 0 &&
	    tilemap_width_prop <= TILEMAP_WIDTH_MAX &&
	    tilemap_height_prop <= TILEMAP_HEIGHT_MAX) {
		tilemap_width = tilemap_width_prop;
		tilemap_height = tilemap_height_prop;
	}
}

static void nuklear_inst_panel_tile_selected_hover(const int mouse_tile[2])
{
	char hover_pos_str[64], hover_type_str[128], hover_col_str[128];
	const tile_t *hovertile = tilemap_tiles[mouse_tile[1]] + mouse_tile[0];
	const char *hover_types[NUM_TILE_TYPES] = {
		"  TILE: SPAWN,",
		"  TILE: NPC,",
		"  TILE: FLOOR,",
		"  TILE: WALL,",
	};

	nk_layout_row_dynamic(nkctx, 16, 1);
	snprintf(hover_pos_str, 64, "  (%d, %d),", mouse_tile[0],
		 mouse_tile[1]);
	snprintf(hover_type_str, 128, "%s", hover_types[hovertile->type]);
	snprintf(hover_col_str, 128, "  0x%.4X,", hovertile->col);
	if (!tilemap_is_mouse_in_range())
		return;

	const char *labels[5] = {
		"HOVER: [", hover_pos_str, hover_type_str, hover_col_str, "]",
	};

	for (int i = 0; i < 5; i++)
		nk_label(nkctx, labels[i], NK_LEFT);
}

void nuklear_inst_panel_tile_selected(const int mouse_tile[2])
{
	char tile_str[64];
	const char *tile_type_strs[NUM_TILE_TYPES] = { "SPAWN", "NPC", "FLOOR",
						       "WALL" };
	int tile_selected_type_prop = tile_selected.type;

	strncpy(tile_str, tile_type_strs[tile_selected.type], 64);
	nk_layout_row_dynamic(nkctx, 18, 1);
	nk_label(nkctx, "TILE: ", NK_LEFT);
	nk_layout_row_dynamic(nkctx, 24, 2);
	for (int i = 0; i < NUM_TILE_TYPES; i++)
		if (nk_option_label(nkctx, tile_type_strs[i],
				    tile_selected_type_prop == i))
			tile_selected_type_prop = i;

	nk_layout_row_dynamic(nkctx, 128, 1);
	nk_color_pick(nkctx, &tile_selected_colf, NK_RGB);
	tile_selected = (tile_t){
		.type = tile_selected_type_prop,
		.col = ((int)(tile_selected_colf.r * 31) << 11) |
		       ((int)(tile_selected_colf.g * 31) << 6) |
		       ((int)(tile_selected_colf.b * 31) << 1) | 1,
	};

	nuklear_inst_panel_tile_selected_hover(mouse_tile);
}

void nuklear_inst_panel_npc(void)
{
	nk_layout_row_begin(nkctx, NK_STATIC, 24, 2);
	nk_layout_row_push(nkctx, 48);
	nk_label(nkctx, "NAME:", NK_LEFT);
	nk_layout_row_push(nkctx, 256);
	npc_name_buf_state = nk_edit_string_zero_terminated(
		nkctx, NK_EDIT_SIMPLE, npc_selected.name, NPC_NAME_MAX_LEN,
		nk_filter_ascii);
	nk_layout_row_end(nkctx);
	nk_layout_row_begin(nkctx, NK_STATIC, 24, 2);
	for (int i = 0; i < npc_selected.num_dialogue_lines; i++) {
		nk_layout_row_push(nkctx, 128);
		nk_edit_string_zero_terminated(nkctx, NK_EDIT_SIMPLE,
					       npc_selected.dialogue[i].speaker,
					       NPC_NAME_MAX_LEN,
					       nk_filter_ascii);
		nk_layout_row_push(nkctx, 512);
		nk_edit_string_zero_terminated(nkctx, NK_EDIT_SIMPLE,
					       npc_selected.dialogue[i].line,
					       NPC_DIALOGUE_LINE_MAX_LEN,
					       nk_filter_ascii);
	}
	nk_layout_row_end(nkctx);
	nk_layout_row_dynamic(nkctx, 24, 2);

	int dlc = npc_selected.num_dialogue_lines;

	dlc += nk_button_label(nkctx, "+Add Line");
	dlc -= nk_button_label(nkctx, "-Remove Line");
	if (dlc >= NPC_NUM_DIALOGUE_LINES_MAX)
		dlc = NPC_NUM_DIALOGUE_LINES_MAX - 1;
	if (dlc < 0)
		dlc = 0;
	npc_selected.num_dialogue_lines = dlc;
}
