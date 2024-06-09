#include <stdlib.h>
#include <stdarg.h>

#include "glwin.h"
#include "nuklear_inst.h"
#include "render.h"
#include "endian.h"
#include "tilemap.h"

#define WIN_WID 1024
#define WIN_HEI 768

static void errorf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

static void update(const char *outpath, const glwin_input_t *inp,
		   int *mx_tile, int *my_tile, const float dt)
{
	tilemap_get_mouse_tile_pos(inp, mx_tile, my_tile);
	tilemap_update(inp, *mx_tile, *my_tile, outpath, dt);
}

int main(const int argc, const char **argv)
{
	if (argc > 2)
		errorf("Too many args\n");

	const char *outpath = argc == 2 ? argv[1] : "untitled.map";

	glwin_init(outpath);
	render_init(outpath);
	nuklear_inst_init();
	tilemap_load(outpath);
	float time_last = glfwGetTime();

	while (!glfwWindowShouldClose(glwin))
	{
		const float time_now = glfwGetTime();
		const float dt = time_now - time_last;

		time_last = time_now;

		/* updating */
		glwin_input_t inp;
		int mx_tile, my_tile;

		glwin_input_poll(&inp);
		update(outpath, &inp, &mx_tile, &my_tile, dt);

		/* rendering */
		glClearColor(0.133f, 0.004f, 0.212f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		tilemap_render(mx_tile, my_tile);
		nuklear_inst_render(&inp, outpath, mx_tile, my_tile);
		glfwSwapBuffers(glwin);
	}
	nuklear_inst_terminate();
	glfwDestroyWindow(glwin);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
