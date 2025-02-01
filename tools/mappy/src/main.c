#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "input.h"
#include "nuklear_inst.h"
#include "render.h"
#include "endian.h"
#include "tilemap.h"
#include "util.h"
#include "window.h"

#define USAGE_STATEMENT_STR(ARGV0)                                             \
	"usage: %s [map_path]\n"                                               \
	"\toptions:\n"                                                         \
	"\t\tmap_path\tPath to where to read from and save to the map file\n", \
		ARGV0

static const char *map_path = NULL;
static float time_last = 0.f;

static void _init(const char *map_path);
static void _update(int mouse_tile[2], const float dt);
static void _terminate(void);

int main(const int argc, const char **argv)
{
	if (argc > 2)
		errorf(USAGE_STATEMENT_STR(argv[0]));

	_init((map_path = argc == 2 ? argv[1] : "untitled.map"));
	time_last = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		const float time_now = glfwGetTime();
		const float dt = time_now - time_last;

		time_last = time_now;

		/* updating */
		int mouse_tile[2];

		_update(mouse_tile, dt);

		/* rendering */
		glClearColor(0.133f, 0.004f, 0.212f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_BLEND);
		for (int y = 0; y < tilemap_height; y++)
			for (int x = 0; x < tilemap_width; x++)
				tilemap_tile_render(x, y,
						    x == mouse_tile[0] &&
							    y == mouse_tile[1]);
		nuklear_inst_render(map_path, mouse_tile, dt);
		glfwSwapBuffers(window);
	}

	_terminate();

	return 0;
}

static void _init(const char *map_path)
{
	/* GLFW */
	window_init(map_path);
	input_reset();

	/* GLEW */
	render_init();

	/* NUKLEAR */
	nuklear_inst_init();

	/* TILEMAP */
	tilemap_load_mappy(map_path);
}

static void _update(int mouse_tile[2], const float dt)
{
	input_poll(window);
	mouse_tile[0] =
		(INPUT_GET_MOUSE(X, NOW) + tilemap_pan_x) / TILE_SIZE_PXLS;
	mouse_tile[1] =
		(INPUT_GET_MOUSE(Y, NOW) + tilemap_pan_y) / TILE_SIZE_PXLS;
	if (mouse_tile[0] < 0)
		mouse_tile[0] = 0;
	if (mouse_tile[0] >= tilemap_width)
		mouse_tile[0] = tilemap_width - 1;
	if (mouse_tile[1] < 0)
		mouse_tile[1] = 0;
	if (mouse_tile[1] >= tilemap_height)
		mouse_tile[1] = tilemap_height - 1;
	tilemap_update_mappy(mouse_tile, dt);
}

static void _terminate(void)
{
	tilemap_unload_mappy();
	nuklear_inst_terminate();
	input_reset();
	window_terminate();
}
