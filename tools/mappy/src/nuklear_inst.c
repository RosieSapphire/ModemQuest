#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

#include "config.h"
#include "input.h"
#include "tilemap.h"
#include "window.h"
#include "door.h"

#include "engine/tilemap.h"

#define FONT_SIZE 20

#define SAVE_TIMER_MAX 2.5f

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear_inst.h"
#include "nuklear_glfw_gl3.h"

struct nk_context *nkctx;
struct nk_colorf tile_selected_colf;
struct nk_font *jbm_font;

static int just_saved;
static float save_timer;

int npc_name_buf_state;
npc_t npc_selected = { 0 };
door_t door_selected = { 0 };

/********
 * BASE *
 ********/

void nuklear_inst_init(void)
{
	const char *jbm_path_main = "fonts/jbm.ttf";
	const char *jbm_path_backup =
		"/home/rosie/.local/share/fonts/JetBrainsMono-Regular.ttf";

	struct nk_font_atlas *atlas;

	nkctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&atlas);
	jbm_font =
		nk_font_atlas_add_from_file(atlas, jbm_path_main, FONT_SIZE, 0);
	if (!jbm_font) {
		printf("Falling back on system-provided font\n");
		jbm_font = nk_font_atlas_add_from_file(atlas, jbm_path_backup,
						       FONT_SIZE, 0);
	}
	nk_glfw3_font_stash_end();
	nk_style_set_font(nkctx, &jbm_font->handle);
}

void nuklear_inst_render(const char *outpath, const int mouse[2],
			 const float dt)
{
	int num_spawns = 0;
	char project_str[128];

	for (int y = 0; y < tilemap.height; y++)
		for (int x = 0; x < tilemap.width; x++)
			num_spawns += (tilemap.tiles[y][x].type ==
				       TILE_TYPE_PLAYER_SPAWN);

	nk_glfw3_new_frame();
	snprintf(project_str, 128,
		 "PROJECT '%s' %d spawn(s), %d npc(s) %d door(s)", outpath,
		 num_spawns, tilemap.num_npcs, tilemap.num_doors);

	if (nk_begin(nkctx, project_str, nk_rect(0, 0, window_width - 180, 128),
		     NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		nuklear_inst_panel_project(outpath, dt);
	nk_end(nkctx);

	if (nk_begin(nkctx, "TILE SELECTED",
		     nk_rect(window_width - 180, 0, 180, window_height),
		     NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		nuklear_inst_panel_tile_selected(mouse);
	nk_end(nkctx);

	if (tile_selected.type == TILE_TYPE_NPC) {
		if (nk_begin(nkctx, "NPC",
			     nk_rect(0, window_height - 180, window_width - 180,
				     180),
			     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
			nuklear_inst_panel_npc();
		}
		nk_end(nkctx);
	}

	nk_glfw3_render(NK_ANTI_ALIASING_ON, NK_VERT_BUF_MAX, NK_ELEM_BUF_MAX);
}

void nuklear_inst_terminate(void)
{
	nk_glfw3_shutdown();
}

/**********
 * PANELS *
 **********/

void nuklear_inst_panel_project(const char *outpath, const float dt)
{
	/* Width & Height properties */
	int tilemap_width_prop = tilemap.width;
	int tilemap_height_prop = tilemap.height;

	nk_layout_row_dynamic(nkctx, 16, 2);
	nk_property_int(nkctx, "WIDTH:", 1, &tilemap_width_prop,
			TILEMAP_WIDTH_MAX, 1, 1);
	nk_property_int(nkctx, "HEIGHT:", 1, &tilemap_height_prop,
			TILEMAP_HEIGHT_MAX, 1, 1);
	nk_layout_row_dynamic(nkctx, 16, 4);

	/* New button */
	if (nk_button_label(nkctx, "NEW")) {
		tilemap_load_mappy(NULL);
	}

	/* Save button */
	if (nk_button_label(nkctx, just_saved ? "SAVED!" : "SAVE")) {
		if (!just_saved) {
			tilemap_save(outpath);
			just_saved = 1;
			save_timer = 0.0f;
		}
	}

	/* Save & Quit button */
	if (nk_button_label(nkctx, "SAVE & QUIT")) {
		tilemap_save(outpath);
		exit(EXIT_SUCCESS);
	}

	/* Quit button */
	if (nk_button_label(nkctx, "QUIT")) {
		exit(EXIT_SUCCESS);
	}

	/* Save timer */
	if (just_saved) {
		save_timer += dt;
		if (save_timer >= SAVE_TIMER_MAX) {
			save_timer = 0.0f;
			just_saved = 0;
		}
	}

	/* Map Index */
	int tilemap_index_prop = tilemap.map_index;

	nk_layout_row_dynamic(nkctx, 16, 2);
	nk_property_int(nkctx, "MAP INDEX:", 0, &tilemap_index_prop,
			TILEMAP_MAP_INDEX_MAX, 1, 1);

	/* assign new properties */
	tilemap.width = (uint16_t)tilemap_width_prop;
	tilemap.height = (uint16_t)tilemap_height_prop;
	tilemap.map_index = (uint16_t)tilemap_index_prop;
	if (tilemap_width_prop > 0 && tilemap_height_prop > 0 &&
	    tilemap_width_prop <= TILEMAP_WIDTH_MAX &&
	    tilemap_height_prop <= TILEMAP_HEIGHT_MAX) {
		tilemap.width = tilemap_width_prop;
		tilemap.height = tilemap_height_prop;
	}
}

static void nuklear_inst_panel_tile_selected_hover(const int mouse_tile[2])
{
	char hover_pos_str[64], hover_type_str[128], hover_col_str[128];
	const tile_t *hovertile = tilemap.tiles[mouse_tile[1]] + mouse_tile[0];
	const char *hover_types[NUM_TILE_TYPES] = {
		"  TILE: SPAWN,", "  TILE: NPC,", "  TILE: FLOOR,",
		"  TILE: WALL,",  "  TILE: DOOR",
	};

	nk_layout_row_dynamic(nkctx, 16, 1);
	snprintf(hover_pos_str, 64, "  (%d, %d),", mouse_tile[0],
		 mouse_tile[1]);
	snprintf(hover_type_str, 128, "%s", hover_types[hovertile->type]);
	snprintf(hover_col_str, 128, "  0x%.4X,", hovertile->col);
	if (!tilemap_is_mouse_in_range()) {
		return;
	}

	const char *labels[4] = {
		"HOVER: [",
		hover_pos_str,
		hover_type_str,
		hover_col_str,
	};

	for (int i = 0; i < 4; i++) {
		nk_label(nkctx, labels[i], NK_LEFT);
	}

	if (hovertile->type == TILE_TYPE_DOOR) {
		char hover_door_str[64];
		door_t *hd = door_get_from_pos(mouse_tile[0], mouse_tile[1]);

		/* FIXME: There is a massive bug where if you overwrite
		 * a previous door, the whole fucking thing collapses.
		 * FIX IT */
		snprintf(hover_door_str, 64, "  DOOR IND: %u", hd->map_index);
		nk_label(nkctx, hover_door_str, NK_LEFT);
	}

	nk_label(nkctx, "]", NK_LEFT);
}

void nuklear_inst_panel_tile_selected(const int mouse_tile[2])
{
	char tile_str[64];
	const char *tile_type_strs[NUM_TILE_TYPES] = { "SPAWN", "NPC", "FLOOR",
						       "WALL", "DOOR" };
	int tile_selected_type_prop = tile_selected.type;

	strncpy(tile_str, tile_type_strs[tile_selected.type], 64);
	nk_layout_row_dynamic(nkctx, 18, 1);
	nk_label(nkctx, "TILE: ", NK_LEFT);
	nk_layout_row_dynamic(nkctx, 24, 2);
	for (int i = 0; i < NUM_TILE_TYPES; i++)
		if (nk_option_label(nkctx, tile_type_strs[i],
				    tile_selected_type_prop == i))
			tile_selected_type_prop = i;

	if (tile_selected.type == TILE_TYPE_DOOR) {
		int door_selected_index_prop = door_selected.map_index;

		nk_layout_row_dynamic(nkctx, 32, 1);
		nk_property_int(nkctx, "MAP INDEX:", 0,
				&door_selected_index_prop,
				TILEMAP_MAP_INDEX_MAX, 1, 1);

		door_selected.map_index = (uint16_t)door_selected_index_prop;
	}

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
	/* name */
	nk_layout_row_begin(nkctx, NK_STATIC, 24, 2);
	nk_layout_row_push(nkctx, 48);
	nk_label(nkctx, "NAME:", NK_LEFT);
	nk_layout_row_push(nkctx, 256);
	npc_name_buf_state = nk_edit_string_zero_terminated(
		nkctx, NK_EDIT_SIMPLE, npc_selected.name, NPC_NAME_MAX_LEN,
		nk_filter_ascii) == 8;
	nk_layout_row_end(nkctx);

	/* dialogue */
	nk_layout_row_begin(nkctx, NK_STATIC, 24, 3);
	for (int i = 0; i < npc_selected.num_dialogue_lines; i++) {
		char *speaker = npc_selected.dialogue[i].speaker;
		nk_layout_row_push(nkctx, 80);
		if (nk_option_label(nkctx, "Player",
				    !strncmp(speaker, "Player",
					     NPC_NAME_MAX_LEN))) {
			memset(speaker, 0, NPC_NAME_MAX_LEN);
			strncpy(speaker, "Player", NPC_NAME_MAX_LEN);
		}
		nk_layout_row_push(nkctx, 54);
		if (nk_option_label(nkctx, "NPC",
				    !strncmp(speaker, npc_selected.name,
					     NPC_NAME_MAX_LEN))) {
			memset(speaker, 0, NPC_NAME_MAX_LEN);
			strncpy(speaker, npc_selected.name, NPC_NAME_MAX_LEN);
		}
		nk_layout_row_push(nkctx, 512);
		nk_edit_string_zero_terminated(nkctx, NK_EDIT_SIMPLE,
					       npc_selected.dialogue[i].line,
					       NPC_DIALOGUE_LINE_MAX_LEN,
					       nk_filter_ascii);
	}
	nk_layout_row_end(nkctx);

	/* adding and removing lines */
	int num_dlg = npc_selected.num_dialogue_lines;

	nk_layout_row_dynamic(nkctx, 24, 2);
	num_dlg += nk_button_label(nkctx, "+Add Line");
	if (num_dlg) {
		if (nk_button_label(nkctx, "-Remove Line")) {
			memset(npc_selected.dialogue[num_dlg - 1].line, 0,
			       NPC_DIALOGUE_LINE_MAX_LEN);
			memset(npc_selected.dialogue[num_dlg - 1].speaker, 0,
			       NPC_NAME_MAX_LEN);
			num_dlg--;
		}
	}
	if (num_dlg >= NPC_NUM_DIALOGUE_LINES_MAX) {
		num_dlg = NPC_NUM_DIALOGUE_LINES_MAX - 1;
	}
	npc_selected.num_dialogue_lines = num_dlg;
}
