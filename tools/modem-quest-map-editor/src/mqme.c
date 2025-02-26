#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <cglm/cglm.h>

#include "util.h"
#include "glfw_window.h"
#include "shader.h"
#include "endian.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define IS_USING_MQME
#include "../../../include/engine/tilemap.h"

#undef TILE_SIZE_PXLS
#define TILE_SIZE_PXLS (int)((32 * tilemap_zoom))

// #define MQME_TILE_SELECTOR_USE_LIST

#define TILEMAP_WIDTH_MIN 1
#define TILEMAP_HEIGHT_MIN 1
/* TODO: THESE WILL BE NEEDED!
#define TILEMAP_NPC_MAX_COUNT 64
#define TILEMAP_DOOR_MAX_COUNT 64
*/
#define TILEMAP_RENDER_INSTANCE_COUNT 512
#define TILEMAP_RENDER_INSTANCE_SIZE 128
#define TILEMAP_NAME_MAX_LEN 64

#define PANNING_PXLS_PER_SEC 512

#define NK_FONT_SIZE 20
#define NK_VERT_BUF_SIZE (512 * 1024)
#define NK_ELEM_BUF_SIZE (128 * 1024)
#define NK_STR_MAX_LEN 128

#define NK_RECT_PANEL_TOP_HEIGHT 86.f
#define NK_RECT_PANEL_TOP \
	(nk_rect(0.f, 0.f, (f32)glfw_win_size[0], NK_RECT_PANEL_TOP_HEIGHT))

#define NK_RECT_PANEL_MAIN_WIDTH 240.f
#define NK_RECT_PANEL_MAIN                                                \
	(nk_rect(0.f, NK_RECT_PANEL_TOP_HEIGHT, NK_RECT_PANEL_MAIN_WIDTH, \
		 (f32)glfw_win_size[1] - NK_RECT_PANEL_TOP_HEIGHT))

#define NK_RECT_PANEL_TILE_WIDTH 240.f
#define NK_RECT_PANEL_TILE                                           \
	(nk_rect((f32)glfw_win_size[0] - NK_RECT_PANEL_TILE_WIDTH,   \
		 NK_RECT_PANEL_TOP_HEIGHT, NK_RECT_PANEL_TILE_WIDTH, \
		 (f32)glfw_win_size[1] - NK_RECT_PANEL_TOP_HEIGHT))

#define NK_RECT_PANEL_NPC_WIDTH  \
	((f32)glfw_win_size[0] - \
	 (NK_RECT_PANEL_MAIN_WIDTH + NK_RECT_PANEL_TILE_WIDTH))
#define NK_RECT_PANEL_NPC_POS_Y \
	(f32) glfw_win_size[1] - NK_RECT_PANEL_NPC_HEIGHT
#define NK_RECT_PANEL_NPC_HEIGHT 240.f
#define NK_RECT_PANEL_NPC                                           \
	(nk_rect(NK_RECT_PANEL_MAIN_WIDTH, NK_RECT_PANEL_NPC_POS_Y, \
		 NK_RECT_PANEL_NPC_WIDTH, NK_RECT_PANEL_NPC_HEIGHT))

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "mqme.h"

static const char *tilemap_path;
static char tilemap_name[TILEMAP_NAME_MAX_LEN];
static boolean tilemap_size_ratio_lock;
static boolean tilemap_show_types;
static vec2 tilemap_sizef;
static vec2 tilemap_pan;
static f32 tilemap_zoom;
static u32 tile_texture_id;
static struct tile tile_selected;
static struct npc npc_selected;
struct tilemap tilemap;

static struct nk_context *nk_ctx;
static struct nk_font *nk_font;
static struct nk_font_atlas *nk_font_atlas;
static nk_flags string_edit_state = 0;
static double mouse_pos[2] = { 0., 0. };

static const uint16_t tile_indis[6] = { 0, 2, 1, 2, 3, 1 };
static mat4 mat_proj;
static u32 rect_vao;
static u32 rect_vbo;
static u32 rect_ebo;
static u32 rect_shader_prog;

static void mqme_tilemap_center_in_view(void)
{
	vec2 pan_center = {
		(f32)((glfw_win_size[0] >> 1) -
		      ((int)NK_RECT_PANEL_TILE_WIDTH >> 1) -
		      ((tilemap.width >> 1) * TILE_SIZE_PXLS) +
		      ((int)NK_RECT_PANEL_MAIN_WIDTH >> 1)),
		(f32)((glfw_win_size[1] >> 1) -
		      ((tilemap.height >> 1) * TILE_SIZE_PXLS) +
		      ((int)NK_RECT_PANEL_TOP_HEIGHT >> 1)),
	};
	glm_vec2_copy(pan_center, tilemap_pan);
}

static void glfw_frame_size_callback(__attribute__((unused)) GLFWwindow *win,
				     int width, int height)
{
	glViewport(0, 0, width, height);
	glfw_win_size[0] = width;
	glfw_win_size[1] = height;
}

static void glfw_scroll_callback(__attribute__((unused)) GLFWwindow *window,
				 __attribute__((unused)) double x_scroll,
				 double y_scroll)
{
	if ((f32)mouse_pos[1] < NK_RECT_PANEL_TOP_HEIGHT) {
		return;
	}

	if ((tile_selected.type == TILE_TYPE_NPC) &&
	    (f32)mouse_pos[1] > NK_RECT_PANEL_NPC_POS_Y) {
		return;
	}

	tilemap_zoom += (f32)y_scroll * .1f;
	if (tilemap_zoom < .1f) {
		tilemap_zoom = .1f;
	}
}

static void mqme_tilemap_init_from_file(void)
{
	FILE *mapfile = fopen(tilemap_path, "rb");

	if (!mapfile) {
		debugf(DEBUG_TYPE_INFO, "'%s' not found, creating.\n",
		       tilemap_path);
		tilemap.width = 10;
		tilemap.height = 10;
		tilemap_sizef[0] = (f32)tilemap.width;
		tilemap_sizef[1] = (f32)tilemap.height;
		tilemap.map_index = 0;
		for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
			for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
				struct tile *t = tilemap.tiles[y] + x;

				t->type = TILE_TYPE_FLOOR;
				t->color = 0xFFFF;
			}
		}

		tilemap.npc_count = 0;
		tilemap.npcs = malloc(0);

		tilemap.door_count = 0;
		tilemap.doors = malloc(0);
		return;
	}

	debugf(DEBUG_TYPE_INFO, "'%s' found, loading.\n", tilemap_path);
	fread_ef16(&tilemap.map_index, mapfile);
	fread(&tilemap.width, 1, 1, mapfile);
	fread(&tilemap.height, 1, 1, mapfile);
	fread(&tilemap.npc_count, 1, 1, mapfile);
	fread(&tilemap.door_count, 1, 1, mapfile);
	tilemap_sizef[0] = tilemap.width;
	tilemap_sizef[1] = tilemap.height;
	for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
		for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
			struct tile *t = tilemap.tiles[y] + x;
			if (y >= tilemap.height || x >= tilemap.width) {
				t->type = TILE_TYPE_FLOOR;
				t->color = 0xFFFF;
				continue;
			}

			fread(&t->type, 1, 1, mapfile);
			fread_ef16(&t->color, mapfile);
		}
	}

	tilemap.npcs = calloc(tilemap.npc_count, sizeof(*tilemap.npcs));
	for (u8 i = 0; i < tilemap.npc_count; i++) {
		struct npc *n = tilemap.npcs + i;
		u16 read_pos[2];

		fread(n->name, 1, NPC_NAME_MAX_LEN, mapfile);
		fread_ef16(read_pos + 0, mapfile);
		fread_ef16(read_pos + 1, mapfile);
		n->pos[0] = (int)read_pos[0];
		n->pos[1] = (int)read_pos[1];
		fread_ef16(&n->num_dialogue_lines, mapfile);
		for (u16 j = 0; j < n->num_dialogue_lines; j++) {
			struct dialogue_line *line = n->dialogue + j;

			fread(&line->speaker, 1, 1, mapfile);
			fread(line->line, 1, NPC_DIALOGUE_LINE_MAX_LEN,
			      mapfile);
		}
	}

	/* TODO: COME BACK TO THIS! READ THE DOORS!
	tilemap.door_count = 0;
	tilemap.doors = malloc(0);
	*/
}

void mqme_init(const char *path, const char *font_path)
{
	srand((u32)time(NULL));

	/* OPENGL */
	glewExperimental = false;
	glewInit();
	glViewport(0, 0, glfw_win_size[0], glfw_win_size[1]);

	/* Buffers */
	glGenVertexArrays(1, &rect_vao);
	glBindVertexArray(rect_vao);

	glGenBuffers(1, &rect_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 4,
			      (void *)(sizeof(f32) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 4,
			      (void *)(sizeof(f32) * 2));
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rect_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6, tile_indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/* Shader */
	rect_shader_prog =
		shader_init_from_file("shaders/vert.glsl", "shaders/frag.glsl");

	/* Texture */
	glGenTextures(1, &tile_texture_id);
	glBindTexture(GL_TEXTURE_2D, tile_texture_id);

	{
		int w, h, c;
		const char *tile_tx_path = "tile-atlas.png";
		u8 *pixels = stbi_load(tile_tx_path, &w, &h, &c, 3);
		assertf(pixels, "Failed to load tiles from '%s'\n",
			tile_tx_path);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
			     GL_UNSIGNED_BYTE, pixels);
		stbi_image_free(pixels);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	/* State */
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	/* TILEMAP */
	tilemap_path = path;

	{
		char *slash_ptr = strrchr(tilemap_path, '/');
		strncpy(tilemap_name,
			slash_ptr ? (slash_ptr + 1) : tilemap_path,
			TILEMAP_NAME_MAX_LEN);
	}
	tilemap_size_ratio_lock = false;
	tilemap_show_types = false;
	tilemap_zoom = 1.f;
	tile_selected = (struct tile){ .type = TILE_TYPE_FLOOR,
				       .color = (rand() & 0xFFFF) | 1 };
	mqme_tilemap_init_from_file();
	mqme_tilemap_center_in_view();

	/* NUKLEAR */
	nk_ctx = nk_glfw3_init(glfw_win, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&nk_font_atlas);
	nk_font = nk_font_atlas_add_from_file(nk_font_atlas, font_path,
					      NK_FONT_SIZE, 0);
	assertf(nk_font, "Failed to load Nuklear font '%s'\n", font_path);
	nk_glfw3_font_stash_end();
	nk_style_set_font(nk_ctx, &nk_font->handle);

	glfwSetScrollCallback(glfw_win, glfw_scroll_callback);
	glfwSetFramebufferSizeCallback(glfw_win, glfw_frame_size_callback);
}

static void mqme_update_view_offset(const f32 dt)
{
	int speed_mul = 1 + glfwGetKey(glfw_win, GLFW_KEY_LEFT_SHIFT);

	if (string_edit_state == 1) {
		return;
	}

	tilemap_pan[0] += (f32)(glfwGetKey(glfw_win, GLFW_KEY_A) -
				glfwGetKey(glfw_win, GLFW_KEY_D)) *
			  dt * PANNING_PXLS_PER_SEC * (f32)speed_mul;
	tilemap_pan[1] += (f32)(glfwGetKey(glfw_win, GLFW_KEY_W) -
				glfwGetKey(glfw_win, GLFW_KEY_S)) *
			  dt * PANNING_PXLS_PER_SEC * (f32)speed_mul;

	/* this was originally for clamping the borders, but maybe not needed.
	if (tilemap.width <= 52) {
		if (tilemap_pan[0] < NK_RECT_PANEL_MAIN_WIDTH) {
			tilemap_pan[0] = NK_RECT_PANEL_MAIN_WIDTH;
		}
		if (tilemap_pan[0] >=
		    glfw_win_size[0] - NK_RECT_PANEL_TILE_WIDTH -
			    (tilemap.width * TILE_SIZE_PXLS)) {
			tilemap_pan[0] = glfw_win_size[0] -
					 NK_RECT_PANEL_TILE_WIDTH -
					 (tilemap.width * TILE_SIZE_PXLS);
		}
	}
	if (tilemap.height <= 32) {
		if (tilemap_pan[1] < NK_RECT_PANEL_TOP_HEIGHT) {
			tilemap_pan[1] = NK_RECT_PANEL_TOP_HEIGHT;
		}
		if (tilemap_pan[1] >= glfw_win_size[1] - ((tilemap.height - 1) *
							  TILE_SIZE_PXLS)) {
			tilemap_pan[1] =
				glfw_win_size[1] -
				((tilemap.height - 1) * TILE_SIZE_PXLS);
		}
	}
	*/
}

static int rect_place_state = 0;

static void mqme_update_rect_fill(ivec2 hover_pos)
{
	static ivec2 rect_place_a, rect_place_b;
	ivec2 minpos;
	ivec2 maxpos;

	if (!rect_place_state) {
		glm_ivec2_copy(hover_pos, rect_place_a);
		rect_place_state = 1;
		return;
	}

	glm_ivec2_copy(hover_pos, rect_place_b);
	minpos[0] = (int)(fmin((f64)rect_place_a[0], (f64)rect_place_b[0]));
	minpos[1] = (int)(fmin((f64)rect_place_a[1], (f64)rect_place_b[1]));
	maxpos[0] = (int)(fmax((f64)rect_place_a[0], (f64)rect_place_b[0]));
	maxpos[1] = (int)(fmax((f64)rect_place_a[1], (f64)rect_place_b[1]));
	for (int y = minpos[1]; y < maxpos[1] + 1; y++) {
		for (int x = minpos[0]; x < maxpos[0] + 1; x++) {
			tilemap.tiles[y][x] = tile_selected;
		}
	}

	rect_place_state = 0;
}

static void mqme_update_tile_selected(void)
{
	static boolean rmb_new = false;
	static boolean rmb_old = false;
	ivec2 tile_hover_pos;
	struct tile *tile_hover = NULL;
	vec2 mouse_pos_rel;

	glfwGetCursorPos(glfw_win, mouse_pos + 0, mouse_pos + 1);

	if (((f32)mouse_pos[0] < NK_RECT_PANEL_MAIN_WIDTH) ||
	    ((f32)mouse_pos[0] >
	     (f32)glfw_win_size[0] - NK_RECT_PANEL_TILE_WIDTH)) {
		return;
	}

	if ((f32)mouse_pos[1] < NK_RECT_PANEL_TOP_HEIGHT) {
		return;
	}

	if ((tile_selected.type == TILE_TYPE_NPC) &&
	    (f32)mouse_pos[1] > NK_RECT_PANEL_NPC_POS_Y) {
		return;
	}

	mouse_pos_rel[0] = (f32)mouse_pos[0] - tilemap_pan[0];
	mouse_pos_rel[1] = (f32)mouse_pos[1] - tilemap_pan[1];
	tile_hover_pos[0] = (int)(mouse_pos_rel[0] / (f32)TILE_SIZE_PXLS);
	tile_hover_pos[1] = (int)(mouse_pos_rel[1] / (f32)TILE_SIZE_PXLS);
	if ((tile_hover_pos[0] < 0) || (tile_hover_pos[0] >= tilemap.width) ||
	    (tile_hover_pos[1] < 0) || (tile_hover_pos[1] >= tilemap.height)) {
		return;
	}

	tile_hover = tilemap.tiles[tile_hover_pos[1]] + tile_hover_pos[0];
	rmb_new = glfwGetMouseButton(glfw_win, GLFW_MOUSE_BUTTON_RIGHT);
	if ((rmb_new ^ rmb_old) && rmb_new) {
		mqme_update_rect_fill(tile_hover_pos);
	}
	rmb_old = rmb_new;

	/* pick tile */
	if (glfwGetMouseButton(glfw_win, GLFW_MOUSE_BUTTON_MIDDLE)) {
		tile_selected = *tile_hover;
		rect_place_state = 0;
	}

	/* place tile */
	if (glfwGetMouseButton(glfw_win, GLFW_MOUSE_BUTTON_LEFT)) {
		struct tile htile_old = *tile_hover;

		*tile_hover = tile_selected;
		rect_place_state = 0;

		if ((tile_hover->type == TILE_TYPE_NPC) &&
		    (htile_old.type != TILE_TYPE_NPC)) {
			struct npc *npc_new = NULL;

			if (!tilemap.npcs) {
				tilemap.npcs = calloc(++tilemap.npc_count,
						      sizeof(*tilemap.npcs));
			} else {
				tilemap.npcs =
					realloc(tilemap.npcs,
						++tilemap.npc_count *
							sizeof(*tilemap.npcs));
			}

			npc_new = tilemap.npcs + tilemap.npc_count - 1;
			strncpy(npc_new->name, npc_selected.name,
				NPC_NAME_MAX_LEN);
			npc_new->num_dialogue_lines =
				npc_selected.num_dialogue_lines;
			npc_new->dialogue = calloc(npc_new->num_dialogue_lines,
						   sizeof(*npc_new->dialogue));
			memcpy(npc_new->dialogue, npc_selected.dialogue,
			       npc_selected.num_dialogue_lines *
				       sizeof(*npc_selected.dialogue));
			npc_new->pos[0] = tile_hover_pos[0];
			npc_new->pos[1] = tile_hover_pos[1];

			debugf(0, "YOU JUST PLACED AN NPC! (now %d)\n",
			       tilemap.npc_count);
			debugf(0, "\tName: %s\n", npc_new->name);
			debugf(0, "\tPos: %d, %d\n", npc_new->pos[0],
			       npc_new->pos[1]);
			debugf(0, "\tLines Count: %u\n",
			       npc_new->num_dialogue_lines);
			for (u16 i = 0; i < npc_new->num_dialogue_lines; i++) {
				struct dialogue_line *line =
					npc_new->dialogue + i;

				debugf(0, "\tLine %d: %d '%s'\n", i,
				       line->speaker, line->line);
			}
		}
	}
}

void mqme_update(const f32 dt)
{
	mqme_update_view_offset(dt);
	mqme_update_tile_selected();
}

static void mqme_tilemap_size_process(void)
{
	const vec2 min = { TILEMAP_WIDTH_MIN, TILEMAP_HEIGHT_MIN };
	const vec2 max = { TILEMAP_WIDTH_MAX, TILEMAP_HEIGHT_MAX };
	vec2 old, diff, new;
	int ratio_lock_prop;

	glm_vec2_copy(tilemap_sizef, old);
	nk_layout_row_begin(nk_ctx, NK_STATIC, 20, 2);
	for (int i = 0; i < 2; i++) {
		const char *str = i ? "H" : "W";

		nk_layout_row_push(nk_ctx, 115);
		nk_property_float(nk_ctx, str, min[i], tilemap_sizef + i,
				  max[i] - 1.f, 1.f, 1.f);
		nk_layout_row_push(nk_ctx, 64);
		if (nk_button_label(nk_ctx, "Round")) {
			tilemap_sizef[i] = roundf(tilemap_sizef[i]);
			debugf(DEBUG_TYPE_INFO, "Rounded Map Width to %d\n",
			       (int)tilemap_sizef[i]);
		}
	}

	nk_layout_row_end(nk_ctx);
	nk_layout_row_dynamic(nk_ctx, 20, 1);

	ratio_lock_prop = tilemap_size_ratio_lock;
	nk_checkbox_label(nk_ctx, "Lock Aspect", &ratio_lock_prop);
	tilemap_size_ratio_lock = (boolean)ratio_lock_prop;

	if (!tilemap_size_ratio_lock) {
		return;
	}

	glm_vec2_sub(tilemap_sizef, old, diff);
	glm_vec2_copy(old, new);
	for (int i = 0; i < 2; i++) {
		f32 ratio;

		if (diff[i] == 0.f) {
			continue;
		}

		ratio = new[!i] / new[i];
		new[i] += diff[i];
		new[!i] = new[i] * ratio;
		if (new[!i] >= max[!i]) {
			new[!i] = max[!i] - 1;
			new[i] = (max[!i] - 1) / ratio;
		}
		if (new[!i] < min[!i]) {
			new[!i] = min[!i];
			new[i] = min[!i] / ratio;
		}
	}

	glm_vec2_copy(new, tilemap_sizef);
}

static void mqme_nk_panel_main(void)
{
	if (nk_tree_push(nk_ctx, NK_TREE_TAB, "Map Settings", 0)) {
		nk_layout_row_dynamic(nk_ctx, 8, 1);

		if (nk_tree_push(nk_ctx, NK_TREE_TAB, "Size", 0)) {
			mqme_tilemap_size_process();
			if (nk_button_label(nk_ctx, "Center in View")) {
				mqme_tilemap_center_in_view();
			}

			nk_tree_pop(nk_ctx);
		}

		nk_tree_pop(nk_ctx);
	}
}

static void mqme_nk_panel_tile(void)
{
	struct nk_colorf tile_colf;
	const char *type_list[TILE_TYPE_COUNT] = { "Spawn", "NPC", "Floor",
						   "Wall", "Door" };
#ifndef MQME_TILE_SELECTOR_USE_LIST
	char type_str[NK_STR_MAX_LEN];
#endif /* MQME_TILE_SELECTOR_USE_LIST */

	nk_layout_row_dynamic(nk_ctx, 18, 1);
	nk_label(nk_ctx, "Color:", NK_TEXT_LEFT);
	nk_layout_row_dynamic(nk_ctx, 25, 1);

	/* Color */
	tile_colf.r = (f32)((tile_selected.color & 0xF800) >> 11) / 31.f;
	tile_colf.g = (f32)((tile_selected.color & 0x07C0) >> 6) / 31.f;
	tile_colf.b = (f32)((tile_selected.color & 0x003E) >> 1) / 31.f;
	tile_colf.a = 1.f;
	if (nk_combo_begin_color(nk_ctx, nk_rgb_cf(tile_colf),
				 nk_vec2(200, 200))) {
		nk_layout_row_dynamic(nk_ctx, 120, 1);
		tile_colf = nk_color_picker(nk_ctx, tile_colf, NK_RGB);
		nk_layout_row_dynamic(nk_ctx, 18, 2);
		if (nk_button_label(nk_ctx, "White")) {
			tile_colf.r = 1.f;
			tile_colf.g = 1.f;
			tile_colf.b = 1.f;
			tile_colf.a = 1.f;
		}

		if (nk_button_label(nk_ctx, "Black")) {
			tile_colf.r = 0.f;
			tile_colf.g = 0.f;
			tile_colf.b = 0.f;
			tile_colf.a = 1.f;
		}

		if (nk_button_label(nk_ctx, "*2")) {
			struct nk_colorf old = tile_colf;
			tile_colf.r *= 2.f;
			tile_colf.g *= 2.f;
			tile_colf.b *= 2.f;
			tile_colf.a = 1.f;
			if (tile_colf.r > 1.f || tile_colf.g > 1.f ||
			    tile_colf.b > 1.f) {
				tile_colf = old;
			}
		}

		if (nk_button_label(nk_ctx, "/2")) {
			tile_colf.r *= .5f;
			tile_colf.g *= .5f;
			tile_colf.b *= .5f;
			tile_colf.a = 1.f;
		}

		nk_layout_row_dynamic(nk_ctx, 18, 1);
		if (nk_button_label(nk_ctx, "Randomize")) {
			tile_colf.r = (float)(rand() & 0xFFFF) / 0xFFFF;
			tile_colf.g = (float)(rand() & 0xFFFF) / 0xFFFF;
			tile_colf.b = (float)(rand() & 0xFFFF) / 0xFFFF;
			tile_colf.a = 1.f;
		}

		nk_combo_end(nk_ctx);
	}

	tile_selected.color = (u16)((u8)(tile_colf.r * 31.f) << 11 |
				    (u8)(tile_colf.g * 31.f) << 6 |
				    (u8)(tile_colf.b * 31.f) << 1 | 1);

#ifdef MQME_TILE_SELECTOR_USE_LIST
	nk_label(nk_ctx, "Type:", NK_TEXT_LEFT);
	nk_layout_row_dynamic(nk_ctx, 24, 1);
	tile_selected.type = (u8)nk_combo(nk_ctx, type_list, 5,
					  tile_selected.type, 25,
					  nk_vec2(200, 200));
#else /* MQME_TILE_SELECTOR_USE_LIST */
	snprintf(type_str, NK_STR_MAX_LEN, "Type: %s",
		 type_list[tile_selected.type]);
	nk_label(nk_ctx, type_str, NK_TEXT_LEFT);
	nk_layout_row_dynamic(nk_ctx, 24, 3);
	for (u8 i = 0; i < TILE_TYPE_COUNT; i++) {
		if (nk_button_label(nk_ctx, type_list[i])) {
			tile_selected.type = i;
		}
	}

#endif /* MQME_TILE_SELECTOR_USE_LIST */
}

static void mqme_nk_panel_npc(void)
{
	string_edit_state = 0;

	nk_layout_row_begin(nk_ctx, NK_STATIC, 24, 2);
	nk_layout_row_push(nk_ctx, 64);
	nk_label(nk_ctx, "Name:", NK_TEXT_LEFT);

	nk_layout_row_push(nk_ctx, 512);
	if (nk_edit_string_zero_terminated(nk_ctx, NK_EDIT_SIMPLE,
					   npc_selected.name, NPC_NAME_MAX_LEN,
					   nk_filter_ascii) == 1) {
		string_edit_state = 1;
	}

	nk_layout_row_end(nk_ctx);

	for (u16 i = 0; i < npc_selected.num_dialogue_lines; i++) {
		struct dialogue_line *line_cur = npc_selected.dialogue + i;

		nk_layout_row_begin(nk_ctx, NK_STATIC, 24, 4);

		nk_layout_row_push(nk_ctx, 84);
		nk_label(nk_ctx, "Speaker:", NK_TEXT_LEFT);

		nk_layout_row_push(nk_ctx, 64);
		if (nk_option_label(nk_ctx, "NPC",
				    line_cur->speaker == DIAL_SPEAKER_NPC)) {
			line_cur->speaker = DIAL_SPEAKER_NPC;
		}

		nk_layout_row_push(nk_ctx, 84);
		if (nk_option_label(nk_ctx, "Player",
				    line_cur->speaker == DIAL_SPEAKER_PLAYER)) {
			line_cur->speaker = DIAL_SPEAKER_PLAYER;
		}

		nk_layout_row_push(nk_ctx, 512);
		if (nk_edit_string_zero_terminated(
			    nk_ctx, NK_EDIT_SIMPLE, line_cur->line,
			    NPC_DIALOGUE_LINE_MAX_LEN, nk_filter_ascii) == 1) {
			string_edit_state = 1;
		}

		nk_layout_row_end(nk_ctx);
	}

	/* adding or removing lines */
	nk_layout_row_begin(nk_ctx, NK_STATIC, 24, 2);
	nk_layout_row_push(nk_ctx, 128);
	if (nk_button_label(nk_ctx, "+ Add Line")) {
		struct dialogue_line *line_new = NULL;

		if (!npc_selected.dialogue) {
			npc_selected.dialogue =
				calloc(++npc_selected.num_dialogue_lines,
				       sizeof(*npc_selected.dialogue));
		} else {
			npc_selected.dialogue =
				realloc(npc_selected.dialogue,
					++npc_selected.num_dialogue_lines *
						sizeof(*npc_selected.dialogue));
		}

		line_new = npc_selected.dialogue +
			   npc_selected.num_dialogue_lines - 1;
		line_new->speaker = DIAL_SPEAKER_NPC;
		memset(line_new->line, 0, NPC_DIALOGUE_LINE_MAX_LEN);

		debugf(0, "npc_seleceted has %d lines (added 1)\n",
		       npc_selected.num_dialogue_lines);
	}

	if (nk_button_label(nk_ctx, "- Remove Line")) {
		if (npc_selected.num_dialogue_lines) {
			npc_selected.dialogue =
				realloc(npc_selected.dialogue,
					--npc_selected.num_dialogue_lines *
						sizeof(*npc_selected.dialogue));
			debugf(0, "npc_seleceted has %d lines (removed 1)\n",
			       npc_selected.num_dialogue_lines);
		}
	}

	nk_layout_row_end(nk_ctx);
}

static void mqme_tiles_render(void)
{
	const ivec4 rect = { (int)tilemap_pan[0], (int)tilemap_pan[1],
			     (int)tilemap_pan[0] + TILE_SIZE_PXLS,
			     (int)tilemap_pan[1] + TILE_SIZE_PXLS };
	const f32 tile_verts[4][4] = { { (f32)rect[0], (f32)rect[1], 0.f, 0.f },
				       { (f32)rect[2], (f32)rect[1], 1.f, 0.f },
				       { (f32)rect[0], (f32)rect[3], 0.f, 1.f },
				       { (f32)rect[2], (f32)rect[3], 1.f,
					 1.f } };
	const int u_proj_loc = glGetUniformLocation(rect_shader_prog, "u_proj");
	const int u_offsets_loc =
		glGetUniformLocation(rect_shader_prog, "u_offsets");
	const int u_colors_loc =
		glGetUniformLocation(rect_shader_prog, "u_colors");
	const int u_tile_size_loc =
		glGetUniformLocation(rect_shader_prog, "u_tile_size");
	const int u_dimensions_loc =
		glGetUniformLocation(rect_shader_prog, "u_dimensions");
	const int u_tile_type_count_loc =
		glGetUniformLocation(rect_shader_prog, "u_tile_type_count");
	const int u_texture_indis_loc =
		glGetUniformLocation(rect_shader_prog, "u_texture_indis");
	ivec2 offsets[TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX];
	int colors[TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX];
	int texture_indis[TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX];

	glm_ortho(0, (f32)glfw_win_size[0], (f32)glfw_win_size[1], 0.f, -1.f,
		  1.f, mat_proj);
	assertf(u_proj_loc != -1, "Couldn't find 'u_proj' in shader\n");
	assertf(u_offsets_loc != -1, "Couldn't find 'u_offsets' in shader\n");
	assertf(u_colors_loc != -1, "Couldn't find 'u_colors' in shader\n");
	assertf(u_tile_size_loc != -1,
		"Couldn't find 'u_tile_size' in shader\n");
	assertf(u_dimensions_loc != -1,
		"Couldn't find 'u_dimensions' in shader\n");
	assertf(u_tile_type_count_loc != -1,
		"Couldn't find 'u_tile_type_count' in shader\n");
	assertf(u_texture_indis_loc != -1,
		"Couldn't find 'u_texture_indis' in shader\n");

	glBindVertexArray(rect_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tile_verts), tile_verts,
		     GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(rect_shader_prog);
	glBindTexture(GL_TEXTURE_2D, tile_texture_id);
	glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, (f32 *)mat_proj);

	for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
		for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
			offsets[y * TILEMAP_WIDTH_MAX + x][0] = x;
			offsets[y * TILEMAP_WIDTH_MAX + x][1] = y;
		}
	}

	for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
		for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
			colors[y * TILEMAP_WIDTH_MAX + x] =
				tilemap.tiles[y][x].color;
		}
	}

	if (tilemap_show_types && tilemap_zoom > 0.8f) {
		for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
			for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
				texture_indis[y * TILEMAP_WIDTH_MAX + x] =
					tilemap.tiles[y][x].type + 1;
			}
		}
	} else {
		memset(texture_indis, 0,
		       TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX *
			       sizeof(*texture_indis));
	}

	glUniform1i(u_tile_size_loc, TILE_SIZE_PXLS);
	glUniform1i(u_tile_type_count_loc, TILE_TYPE_COUNT);
	glUniform2iv(u_dimensions_loc, 1,
		     (ivec2){ tilemap.width, tilemap.height });
	for (int i = 0; i < TILEMAP_RENDER_INSTANCE_COUNT; i++) {
		int *offsets_chunk =
			(int *)(offsets + (i * TILEMAP_RENDER_INSTANCE_SIZE));
		int *colors_chunk = colors + (i * TILEMAP_RENDER_INSTANCE_SIZE);
		int *textures_chunk =
			texture_indis + (i * TILEMAP_RENDER_INSTANCE_SIZE);
		glUniform2iv(u_offsets_loc, TILEMAP_RENDER_INSTANCE_SIZE,
			     offsets_chunk);
		glUniform1iv(u_colors_loc, TILEMAP_RENDER_INSTANCE_SIZE,
			     colors_chunk);
		glUniform1iv(u_texture_indis_loc, TILEMAP_RENDER_INSTANCE_SIZE,
			     textures_chunk);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
					tile_indis,
					TILEMAP_RENDER_INSTANCE_SIZE);
	}
	glBindVertexArray(0);
}

static void mqme_tilemap_save(void)
{
	FILE *file = fopen(tilemap_path, "wb");

	fwrite_ef16(&tilemap.map_index, file);
	fwrite(&tilemap.width, 1, 1, file);
	fwrite(&tilemap.height, 1, 1, file);
	fwrite(&tilemap.npc_count, 1, 1, file);
	fwrite(&tilemap.door_count, 1, 1, file);

	/* tiles */
	for (int y = 0; y < tilemap.height; y++) {
		for (int x = 0; x < tilemap.width; x++) {
			const struct tile *t = tilemap.tiles[y] + x;
			fwrite(&t->type, 1, 1, file);
			fwrite_ef16(&t->color, file);
		}
	}

	/* npcs */
	for (u8 i = 0; i < tilemap.npc_count; i++) {
		const struct npc *n = tilemap.npcs + i;

		fwrite(n->name, 1, NPC_NAME_MAX_LEN, file);
		fwrite_ef16(((const u16 *)(n->pos + 0)), file);
		fwrite_ef16(((const u16 *)(n->pos + 1)), file);
		fwrite_ef16(&n->num_dialogue_lines, file);
		for (u16 j = 0; j < n->num_dialogue_lines; j++) {
			struct dialogue_line *line = n->dialogue + j;

			fwrite(&line->speaker, 1, 1, file);
			fwrite(line->line, 1, NPC_DIALOGUE_LINE_MAX_LEN, file);
		}
	}

	/* doors */
	/*
	for (int i = 0; i < tilemap.door_count; i++) {
		door_t *d = tilemap.doors + i;

		fwrite(&d->map_index, 1, 1, file);
		fwrite_ef16(d->pos + 0, file);
		fwrite_ef16(d->pos + 1, file);
	}
	*/

	fclose(file);
	/* FIXME: Again, path and name separated */
	debugf(DEBUG_TYPE_INFO, "TILEMAP SAVED TO '%s'\n", tilemap_path);
}

void mqme_render(void)
{
	char panel_top_label[NK_STR_MAX_LEN];

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	/* TILEMAP */
	tilemap.width = (u8)tilemap_sizef[0];
	tilemap.height = (u8)tilemap_sizef[1];
	mqme_tiles_render();

	/* NUKLEAR */
	nk_glfw3_new_frame();

	/* Top border */
	snprintf(panel_top_label, NK_STR_MAX_LEN, "Map Name: '%s'",
		 tilemap_name);
	if (nk_begin(nk_ctx, panel_top_label, NK_RECT_PANEL_TOP,
		     NK_WINDOW_TITLE)) {
		int show_types_prop = tilemap_show_types;

		nk_layout_row_dynamic(nk_ctx, 24, 8);
		if (nk_button_label(nk_ctx, "SAVE")) {
			mqme_tilemap_save();
		}

		if (nk_button_label(nk_ctx, "SAVE & QUIT")) {
			mqme_tilemap_save();
			mqme_free();
			exit(EXIT_SUCCESS);
		}

		nk_checkbox_label(nk_ctx, "Show Tile Types", &show_types_prop);
		tilemap_show_types = show_types_prop;
		nk_end(nk_ctx);
	}

	/* Panel main */
	if (nk_begin(nk_ctx, "Panel Main", NK_RECT_PANEL_MAIN,
		     NK_WINDOW_BORDER)) {
		mqme_nk_panel_main();
		nk_end(nk_ctx);
	}

	/* Panel tile */
	if (nk_begin(nk_ctx, "Tile Selected", NK_RECT_PANEL_TILE,
		     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
		mqme_nk_panel_tile();
		nk_end(nk_ctx);
	}

	/* Panel NPC */
	if (tile_selected.type == TILE_TYPE_NPC) {
		if (nk_begin(nk_ctx, "NPC Settings", NK_RECT_PANEL_NPC,
			     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
			mqme_nk_panel_npc();
			nk_end(nk_ctx);
		}
	}

	nk_glfw3_render(NK_ANTI_ALIASING_ON, NK_VERT_BUF_SIZE,
			NK_ELEM_BUF_SIZE);
}

void mqme_free(void)
{
	/* NUKLEAR */
	nk_glfw3_shutdown();
	nk_free(nk_ctx);

	/* TILEMAP */
	tile_selected.type = 0x0;
	tile_selected.color = 0x0;
	tilemap_zoom = 0.f;

	free(tilemap.doors);
	tilemap.door_count = 0;

	free(tilemap.npcs);
	tilemap.npc_count = 0;

	memset(*tilemap.tiles, 0, TILEMAP_WIDTH_MAX * sizeof(**tilemap.tiles));
	memset(*tilemap.tiles, 0, TILEMAP_HEIGHT_MAX * sizeof(*tilemap.tiles));
	tilemap.map_index = 0;
	tilemap.height = 0;
	tilemap.width = 0;
	tilemap_sizef[1] = 0.f;
	tilemap_sizef[0] = 0.f;
	tilemap_show_types = false;
	tilemap_size_ratio_lock = false;
	tilemap_path = NULL;

	/* OPENGL */
	glDeleteTextures(1, &tile_texture_id);
	shader_free(rect_shader_prog);
	glDeleteBuffers(1, &rect_ebo);
	glDeleteBuffers(1, &rect_vbo);
	glDeleteVertexArrays(1, &rect_vao);
}

#pragma clang diagnostic pop
