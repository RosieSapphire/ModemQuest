/* CSTD */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* OpenGL */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "types.h"

/* Nuklear */
#define NK_VERT_BUF_MAX (512 * 1024)
#define NK_ELEM_BUF_MAX (128 * 1024)

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

/* ModemQuest */
#include "game/tiles.h"
#undef TILE_SIZE
#define TILE_SIZE (is_zoomed ? 64 : 32)

#define WIN_WID 1024
#define WIN_HEI 768

#define PANNING_SPEED 512

#define TILES_W_DEFAULT 10
#define TILES_H_DEFAULT 10

static unsigned int tile_vao, tile_vbo, tile_ebo;
static const u16 tile_indis[6] = {0, 2, 1, 2, 3, 1};

static mat4 mat_proj;

static int scroll_x, scroll_y, is_zoomed;

static tile_t tile_selected = {TILE_TYPE_FLOOR, 0xFFFF};
static struct nk_colorf _tile_selected_colf = {1.0f, 1.0f, 1.0f, 1.0f};
tile_t tiles[TILES_H_MAX][TILES_W_MAX];
u16 tiles_w;
u16 tiles_h;

static GLFWwindow *glwin = NULL;
static struct nk_context *nkctx;

static void rect_buffers_init(void)
{
	glGenVertexArrays(1, &tile_vao);
	glBindVertexArray(tile_vao);

	glGenBuffers(1, &tile_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			      sizeof(float) * 2, NULL);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &tile_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6, tile_indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

static const char *rect_vertshd_src =
"#version 330 core\n"
"\n"
"layout (location = 0) in vec2 a_pos;\n"
"\n"
"uniform mat4 u_proj;\n"
"\n"
"void main(void)\n"
"{\n"
"        gl_Position = u_proj * vec4(a_pos, 0.0, 1.0);\n"
"}\n";

static const char *rect_fragshd_src =
"#version 330 core\n"
"\n"
"uniform vec4 u_col;\n"
"\n"
"out vec4 frag_col;\n"
"\n"
"void main(void)\n"
"{\n"
"        frag_col = vec4(u_col);\n"
"}\n";

static unsigned int tile_shader;

static void rect_shader_compile(void)
{
	unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
	unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
	int vert_stat, frag_stat, prog_stat;
	char log[512];

	glShaderSource(vert, 1, &rect_vertshd_src, NULL);
	glShaderSource(frag, 1, &rect_fragshd_src, NULL);
	glCompileShader(vert);
	glCompileShader(frag);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_stat);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_stat);
	if (!vert_stat)
	{
		glGetShaderInfoLog(vert, 512, NULL, log);
		fprintf(stderr, "Vertex Shader failed to compile: %s\n", log);
		exit(EXIT_FAILURE);
	}
	if (!frag_stat)
	{
		glGetShaderInfoLog(frag, 512, NULL, log);
		fprintf(stderr, "Fragment Shader failed to compile: %s\n", log);
		exit(EXIT_FAILURE);
	}

	tile_shader = glCreateProgram();
	glAttachShader(tile_shader, vert);
	glAttachShader(tile_shader, frag);
	glLinkProgram(tile_shader);
	glGetProgramiv(tile_shader, GL_LINK_STATUS, &prog_stat);
	if (!prog_stat)
	{
		glGetProgramInfoLog(tile_shader, 512, NULL, log);
		fprintf(stderr, "Shader Program failed to link: %s\n", log);
		exit(EXIT_FAILURE);
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}

static void render_rect(const int x0, const int y0, const int x1, const int y1,
			const float r, const float g, const float b,
			const float a)
{
	const float tile_verts[4][2] = {{x0, y0}, {x1, y0}, {x0, y1}, {x1, y1}};
	const int u_proj_loc = glGetUniformLocation(tile_shader, "u_proj");
	const int u_col_loc = glGetUniformLocation(tile_shader, "u_col");
	const float rgba[4] = {r, g, b, a};

	if (u_proj_loc == -1)
	{
		fprintf(stderr, "Couldn't find 'u_proj' in shader\n");
		exit(EXIT_FAILURE);
	}

	if (u_col_loc == -1)
	{
		fprintf(stderr, "Couldn't find 'u_col' in shader\n");
		exit(EXIT_FAILURE);
	}

	glBindVertexArray(tile_vao);
	glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tile_verts),
		     tile_verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(tile_shader);
	glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, (float *)mat_proj);
	glUniform4fv(u_col_loc, 1, rgba);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, tile_indis);
	glBindVertexArray(0);
}

static void render_tile(const int x, const int y)
{
	if (x < 0 || x >= tiles_w || y < 0 || y >= tiles_h)
		return;

	const int pad = 3;
	const int x0 = (x * TILE_SIZE) - scroll_x;
	const int y0 = (y * TILE_SIZE) - scroll_y;
	const int x1 = x0 + TILE_SIZE;
	const int y1 = y0 + TILE_SIZE;
	const tile_t *tile = tiles[y] + x;
	const float r = ((tile->col & 0xF800) >> 11) / 31.0f;
	const float g = ((tile->col & 0x07C0) >> 6) / 31.0f;
	const float b = ((tile->col & 0x003E) >> 1) / 31.0f;
	const float a = ((tile->col & 0x0001) >> 0);

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

static void render_tile_selected(const int mouse_tile_x, const int mouse_tile_y)
{
	if (mouse_tile_x < 0 || mouse_tile_x >= tiles_w ||
	    mouse_tile_y < 0 || mouse_tile_y >= tiles_h)
		return;

	const int pad = 3;
	const int x0 = (mouse_tile_x * TILE_SIZE) - scroll_x;
	const int y0 = (mouse_tile_y * TILE_SIZE) - scroll_y;
	const int x1 = x0 + TILE_SIZE;
	const int y1 = y0 + TILE_SIZE;

	const float r = _tile_selected_colf.r;
	const float g = _tile_selected_colf.g;
	const float b = _tile_selected_colf.b;
	const float a = 0.5f;

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

static void tiles_resize(const int w_new, const int h_new)
{
	if (w_new <= 0 || h_new <= 0 ||
	    w_new > TILES_W_MAX || h_new > TILES_H_MAX)
		return;

	tiles_w = w_new;
	tiles_h = h_new;
}

static void fwrite_ef16(const u16 *ptr, FILE *file)
{
	u16 flip = ((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8);

	fwrite(&flip, 2, 1, file);
}

static void fread_ef16(u16 *ptr, FILE *file)
{
	fread(ptr, 2, 1, file);

	u16 flip = ((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8);

	*ptr = flip;
}

int main(const int argc, const char **argv)
{
	scroll_x = 0;
	scroll_y = -128;
	is_zoomed = 0;

	const char *outpath = "untitled.map";

	if (argc > 2)
	{
		fprintf(stderr, "Too many args\n");
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
		outpath = argv[1];

	FILE *loadfile = fopen(outpath, "rb");

	if (loadfile)
	{
		fread_ef16(&tiles_w, loadfile);
		fread_ef16(&tiles_h, loadfile);
		for (int y = 0; y < tiles_h; y++)
		{
			for (int x = 0; x < tiles_w; x++)
			{
				tile_t *t = tiles[y] + x;

				fread(&t->type, 1, 1, loadfile);
				fread_ef16(&t->col, loadfile);
			}
		}
		fclose(loadfile);
	}
	else
	{
		tiles_w = TILES_W_DEFAULT;
		tiles_h = TILES_H_DEFAULT;
		for (int y = 0; y < TILES_H_MAX; y++)
		{
			for (int x = 0; x < TILES_W_MAX; x++)
			{
				tiles[y][x].type = TILE_TYPE_FLOOR;
				tiles[y][x].col = 0xFFFF;
			}
		}
	}

	/* GLFW */
	glfwInit();

	char winname[256];
	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *vm = glfwGetVideoMode(mon);
	int winx = (vm->width >> 1)  - (WIN_WID >> 1);
	int winy = (vm->height >> 1)  - (WIN_HEI >> 1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, vm->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vm->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vm->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	snprintf(winname, 64, "Mappy the Map Editor (%s)", outpath);
	glwin = glfwCreateWindow(WIN_WID, WIN_HEI, winname, NULL, NULL);
	glfwSetWindowPos(glwin, winx, winy);
	glfwMakeContextCurrent(glwin);

	/* GLEW */
	glewExperimental = 1;
	glewInit();
	glViewport(0, 0, WIN_WID, WIN_HEI);
	glm_ortho(0, WIN_WID, WIN_HEI, 0, -1, 1, mat_proj);
	rect_buffers_init();
	rect_shader_compile();

	/* Nuklear GUI */
	struct nk_font_atlas *atlas;

	nkctx = nk_glfw3_init(glwin, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&atlas);
	nk_glfw3_font_stash_end();

	float time_last = glfwGetTime();

	while (!glfwWindowShouldClose(glwin))
	{
		const float time_now = glfwGetTime();
		const float dt = time_now - time_last;

		time_last = time_now;

		/* updating */
		glfwPollEvents();

		static int z_last = 0;
		static int rmb_last = 0;
		static int mmb_last = 0;
		static int enter_last = 0;
		int w_now = glfwGetKey(glwin, GLFW_KEY_W);
		int s_now = glfwGetKey(glwin, GLFW_KEY_S);
		int a_now = glfwGetKey(glwin, GLFW_KEY_A);
		int d_now = glfwGetKey(glwin, GLFW_KEY_D);
		int z_now = glfwGetKey(glwin, GLFW_KEY_Z);
		int lmb_now = glfwGetMouseButton(glwin, GLFW_MOUSE_BUTTON_LEFT);
		int rmb_now = glfwGetMouseButton(glwin,
						 GLFW_MOUSE_BUTTON_RIGHT);
		int mmb_now = glfwGetMouseButton(glwin,
						 GLFW_MOUSE_BUTTON_MIDDLE);
		int enter_now = glfwGetKey(glwin, GLFW_KEY_ENTER);
		int shift_now = glfwGetKey(glwin, GLFW_KEY_LEFT_SHIFT);
		double mouse_x, mouse_y = 0;
		int mouse_tile_x, mouse_tile_y;

		glfwGetCursorPos(glwin, &mouse_x, &mouse_y);
		mouse_tile_x = (mouse_x + scroll_x) / TILE_SIZE;
		mouse_tile_y = (mouse_y + scroll_y) / TILE_SIZE;

		/* panning */
		for (int i = 0; i < shift_now + 1; i++)
		{
			scroll_x += (d_now - a_now) *
				PANNING_SPEED * dt;
			scroll_y += (s_now - w_now) *
				PANNING_SPEED * dt;
		}

		/* zooming */
		is_zoomed ^= (z_now && !z_last);

		/* placing tile */
		int mouse_inside_map =
			mouse_tile_x >= 0 && mouse_tile_x < tiles_w &&
			mouse_tile_y >= 0 && mouse_tile_y < tiles_h &&
			mouse_x < WIN_WID - 200 && mouse_y > 128;


		if (lmb_now && mouse_inside_map)
			tiles[mouse_tile_y][mouse_tile_x] = tile_selected;

		/* picking tile */
		if (mmb_now && !mmb_last && mouse_inside_map)
		{
			printf("Selected\n");
			const tile_t *t = tiles[mouse_tile_y] + mouse_tile_x;

			tile_selected.type = t->type;
			_tile_selected_colf.r =
				(float)((t->col & 0xF800) >> 11) / 31.0f;
			_tile_selected_colf.g =
				(float)((t->col & 0x07C0) >> 6) / 31.0f;
			_tile_selected_colf.b =
				(float)((t->col & 0x003E) >> 1) / 31.0f;
		}

		/* recting tiles */
		if (rmb_now && !rmb_last && mouse_inside_map)
		{
			static int which_point = 0;
			static int point_ax = 0;
			static int point_ay = 0;
			static int point_bx = 0;
			static int point_by = 0;

			switch (which_point)
			{
			case 0:
				point_ax = mouse_tile_x;
				point_ay = mouse_tile_y;
				break;

			case 1:
				point_bx = mouse_tile_x;
				point_by = mouse_tile_y;
				if (point_ax > point_bx)
				{
					int tmp = point_bx;

					point_bx = point_ax;
					point_ax = tmp;
				}
				if (point_ay > point_by)
				{
					int tmp = point_by;

					point_by = point_ay;
					point_ay = tmp;
				}
				for (int y = point_ay; y <= point_by; y++)
				{
					for (int x = point_ax;
					     x <= point_bx; x++)
					{
						tiles[y][x].type =
							tile_selected.type;
						tiles[y][x].col =
							tile_selected.col;
					}
				}
				break;
			}

			which_point ^= 1;
		}

		/* saving */
		if (enter_now && !enter_last)
		{
			FILE *file = fopen(outpath, "wb");

			fwrite_ef16(&tiles_w, file);
			fwrite_ef16(&tiles_h, file);
			for (int y = 0; y < tiles_h; y++)
			{
				for (int x = 0; x < tiles_w; x++)
				{
					const tile_t *t = tiles[y] + x;

					fwrite(&t->type, 1, 1, file);
					fwrite_ef16(&t->col, file);
				}
			}

			fclose(file);
			printf("SAVED FILE '%s'\n", outpath);
		}

		z_last = z_now;
		rmb_last = rmb_now;
		mmb_last = mmb_now;
		enter_last = enter_now;

		/* rendering */
		glClearColor(0.133f, 0.004f, 0.212f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		/* OpenGL Render State */
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		/* tilemap */
		for (int y = 0; y < tiles_h; y++)
			for (int x = 0; x < tiles_w; x++)
				render_tile(x, y);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		render_tile_selected(mouse_tile_x, mouse_tile_y);
		glDisable(GL_BLEND);

		/* nuklear */
		char project_str[64];
		int num_spawns = 0;

		for (int y = 0; y < tiles_h; y++)
			for (int x = 0; x < tiles_w; x++)
				num_spawns +=
					(tiles[y][x].type ==
					 TILE_TYPE_PLAYER_SPAWN);

		nk_glfw3_new_frame();
		snprintf(project_str, 64, "PROJECT '%s' (%d spawns)", outpath,
			 num_spawns);
		if (nk_begin(nkctx, project_str,
		    nk_rect(0, 0, WIN_WID - 200, 128),
		    NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		{
			int tiles_w_prop = tiles_w;
			int tiles_h_prop = tiles_h;

			nk_layout_row_static(nkctx, 16, 128, 1);
			nk_property_int(nkctx, "WIDTH:", 1, &tiles_w_prop,
					TILES_W_MAX, 1, 1);
			nk_property_int(nkctx, "HEIGHT:", 1, &tiles_h_prop,
					TILES_H_MAX, 1, 1);
			if (nk_button_label(nkctx, "RESET VIEW"))
			{
				scroll_x = 0;
				scroll_y = -128;
			}

			tiles_w = (u16)tiles_w_prop;
			tiles_h = (u16)tiles_h_prop;
			tiles_resize(tiles_w, tiles_h);
		}
		nk_end(nkctx);
		if (nk_begin(nkctx, "TILE SELECTED",
		    nk_rect(WIN_WID - 200, 0, 200, WIN_HEI),
		    NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		{
			char tile_str[64];
			char hover_pos_str[64];
			char hover_type_str[128];
			char hover_col_str[128];

			switch (tile_selected.type)
			{
			case TILE_TYPE_PLAYER_SPAWN:
				snprintf(tile_str, 64,
					 "TILE_TYPE_PLAYER_SPAWN");
				break;

			case TILE_TYPE_FLOOR:
				snprintf(tile_str, 64,
					 "TILE_TYPE_FLOOR");
				break;

			case TILE_TYPE_WALL:
				snprintf(tile_str, 64,
					 "TILE_TYPE_WALL");
				break;
			}

			/*
			CCCCC OOOOO N   N GGGGG  AAA 
			C     O   O NN  N G     A   A
			C     O   O N N N G  GG AAAAA
			C     O   O N  NN G   G A   A
			CCCCC OOOOO N   N GGGGG A   A
			*/
			int tile_selected_type_prop = tile_selected.type;

			nk_layout_row_static(nkctx, 16, 32, 2);
			nk_label(nkctx, "TILE: ", NK_LEFT);
			nk_layout_row_static(nkctx, 16, 180, 2);
			nk_property_int(nkctx, tile_str, 0,
					&tile_selected_type_prop,
					TILE_TYPE_CNT, 1, 1);
			nk_layout_row_static(nkctx, 128, 128, 1);
			nk_color_pick(nkctx, &_tile_selected_colf, NK_RGB);
			tile_selected = (tile_t) {
				.type = tile_selected_type_prop,
				.col = 
					((int)(_tile_selected_colf.r * 31)
					 << 11) |
					((int)(_tile_selected_colf.g * 31)
					 << 6) |
					((int)(_tile_selected_colf.b * 31)
					 << 1) | 1,
			};
			nk_layout_row_static(nkctx, 16, 128, 1);
			const tile_t *hovertile =
				tiles[mouse_tile_y] + mouse_tile_x;

			snprintf(hover_pos_str, 64, "    (%d, %d),",
				 mouse_tile_x, mouse_tile_y);
			switch (hovertile->type)
			{
			case TILE_TYPE_PLAYER_SPAWN:
				snprintf(hover_type_str, 128,
					 "    TILE_TYPE_PLAYER_SPAWN,");
				break;

			case TILE_TYPE_FLOOR:
				snprintf(hover_type_str, 128,
					 "    TILE_TYPE_FLOOR,");
				break;

			case TILE_TYPE_WALL:
				snprintf(hover_type_str, 128,
					 "    TILE_TYPE_WALL,");
				break;
			}
			snprintf(hover_col_str, 128, "    0x%.4X,",
				 hovertile->col);
			if (mouse_inside_map)
			{
				nk_label(nkctx, "HOVER: [", NK_LEFT);
				nk_label(nkctx, hover_pos_str, NK_LEFT);
				nk_label(nkctx, hover_type_str, NK_LEFT);
				nk_label(nkctx, hover_col_str, NK_LEFT);
				nk_label(nkctx, "]", NK_LEFT);
			}
		}
		nk_end(nkctx);
		nk_glfw3_render(NK_ANTI_ALIASING_ON, NK_VERT_BUF_MAX,
				NK_ELEM_BUF_MAX);

		glfwSwapBuffers(glwin);
	}

	nk_glfw3_shutdown();
	glfwDestroyWindow(glwin);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
