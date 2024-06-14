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

static void update(glwin_input_t *inp, int *mx_tile,
		   int *my_tile, const float dt)
{
	int mxt, myt;

	glwin_input_poll(inp);
	mxt = (inp->mx_now + tilemap_pan_x) / TILE_SIZE;
	myt = (inp->my_now + tilemap_pan_y) / TILE_SIZE;
	if (mxt < 0)
		mxt = 0;
	if (mxt >= tilemap_w)
		mxt = tilemap_w - 1;
	if (myt < 0)
		myt = 0;
	if (myt >= tilemap_h)
		myt = tilemap_h - 1;
	tilemap_update(inp, mxt, myt, dt);

	*mx_tile = mxt;
	*my_tile = myt;
}

int main(const int argc, const char **argv)
{
	if (argc > 2)
		errorf("Too many args\n");

	const char *outpath = argc == 2 ? argv[1] : "untitled.map";

	glwin_init(outpath);
	render_init();
	nuklear_inst_init();
	tilemap_load_mappy(outpath);
	float time_last = glfwGetTime();

	while (!glfwWindowShouldClose(glwin))
	{
		const float time_now = glfwGetTime();
		const float dt = time_now - time_last;

		time_last = time_now;

		/* updating */
		glwin_input_t inp;
		int mx_tile, my_tile;

		update(&inp, &mx_tile, &my_tile, dt);

		/* rendering */
		glClearColor(0.133f, 0.004f, 0.212f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_BLEND);
		for (int y = 0; y < tilemap_h; y++)
			for (int x = 0; x < tilemap_w; x++)
				tilemap_tile_render(&inp, x, y,
						    x == mx_tile &&
						    y == my_tile);
		nuklear_inst_render(&inp, outpath, mx_tile, my_tile, dt);
		glfwSwapBuffers(glwin);
	}
	nuklear_inst_terminate();
	glfwDestroyWindow(glwin);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
