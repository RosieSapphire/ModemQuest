#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "glfw_window.h"
#include "mqme.h"

#define ESCAPE_TIMER_MAX 2.f
#define MAP_PATH_MAX_LEN 4096

#define USAGE_STATEMENT_STR(ARGV0) "usage: %s map_path\n", ARGV0

static f32 escape_timer;

static void
_map_path_parse(__attribute__((unused)) char map_path[MAP_PATH_MAX_LEN],
		const char *argv1)
{
	const char *endstr = strrchr(argv1, '.');
	memset(map_path, 0, MAP_PATH_MAX_LEN);
	if (!endstr) {
		snprintf(map_path, MAP_PATH_MAX_LEN, "%s.map", argv1);
		debugf(DEBUG_TYPE_INFO,
		       "Imported '%s' (had to add `.map` at end)\n", map_path);
		return;
	}

	if (!strncmp(endstr, ".map", 4)) {
		strncpy(map_path, argv1, strlen(argv1));
		debugf(DEBUG_TYPE_INFO,
		       "Imported '%s' (already has `.map` at end)\n", map_path);
		return;
	}

	assertf(0, "Please enter a valid name, either with "
		   "'.map' at the end or not. Nothing in between\n");
}

int main(const int argc, const char **argv)
{
	assertf(argc == 2, USAGE_STATEMENT_STR(argv[0]));
	glfw_window_init();
	debugf(DEBUG_TYPE_INFO, "GLFW Instance Initialized\n");
	debugf(DEBUG_TYPE_INFO, "OpenGL Instance Initialized\n");

	char map_path[MAP_PATH_MAX_LEN];
	_map_path_parse(map_path, argv[1]);
	mqme_init(map_path, "fonts/jbm.ttf");
	debugf(DEBUG_TYPE_INFO, "MQME Instance Initialized\n");

	escape_timer = 0.f;

	f32 time_old = glfwGetTime();
	while (!glfwWindowShouldClose(glfw_win)) {
		f32 time_new, dt;
		time_new = glfwGetTime();
		dt = time_new - time_old;
		time_old = time_new;
		glfwPollEvents();
		if (glfwGetKey(glfw_win, GLFW_KEY_ESCAPE)) {
			escape_timer += dt;
			glfwSetWindowShouldClose(glfw_win, (escape_timer >=
							    ESCAPE_TIMER_MAX));
		} else {
			escape_timer = 0.f;
		}

		mqme_update(dt);
		mqme_render();
		glfwSwapBuffers(glfw_win);
	}

	mqme_free();
	debugf(DEBUG_TYPE_INFO, "MQME Freed\n");
	debugf(DEBUG_TYPE_INFO, "OpenGL Instance Freed\n");
	glfw_window_free();
	debugf(DEBUG_TYPE_INFO, "GLFW Window Freed\n");

	return 0;
}
