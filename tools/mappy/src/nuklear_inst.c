#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

#include "config.h"
#include "input.h"
#include "tilemap.h"
#include "window.h"

#include "game/tilemap.h"

#define FONT_SIZE 20

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

	for (int y = 0; y < tilemap_height; y++)
		for (int x = 0; x < tilemap_width; x++)
			num_spawns +=
				(tilemap[y][x].type == TILE_TYPE_PLAYER_SPAWN);

	nk_glfw3_new_frame();
	snprintf(project_str, 128, "PROJECT '%s' %d spawn(s), %d npc(s)",
		 outpath, num_spawns, tilemap_num_npcs);

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
				     window_height),
			     NK_WINDOW_BORDER | NK_WINDOW_TITLE))
			nuklear_inst_panel_npc();
		nk_end(nkctx);
	}

	nk_glfw3_render(NK_ANTI_ALIASING_ON, NK_VERT_BUF_MAX, NK_ELEM_BUF_MAX);
}

void nuklear_inst_terminate(void)
{
	nk_glfw3_shutdown();
}
