#include <stdio.h>
#include <string.h>

#include "window.h"

#define WINDOW_WIDTH_DEFAULT 1024
#define WINDOW_HEIGHT_DEFAULT 768

GLFWwindow *window = NULL;
char window_name[WINDOW_NAME_MAX_LEN];
int window_width = 0, window_height = 0;

void window_init(const char *map_path)
{
	window_width = WINDOW_WIDTH_DEFAULT;
	window_height = WINDOW_HEIGHT_DEFAULT;

	glfwInit();

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *vm = glfwGetVideoMode(mon);
	int winx = (vm->width >> 1) - (window_width >> 1);
	int winy = (vm->height >> 1) - (window_height >> 1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, vm->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vm->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vm->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);
	snprintf(window_name, WINDOW_NAME_MAX_LEN, "Mappy the Map Editor (%s)",
		 map_path);
	window = glfwCreateWindow(window_width, window_height, window_name,
				  NULL, NULL);
	glfwSetWindowPos(window, winx, winy);
	glfwMakeContextCurrent(window);
}

void window_terminate(void)
{
	glfwDestroyWindow(window);
	window = NULL;
	memset(window_name, 0, WINDOW_NAME_MAX_LEN);
	glfwTerminate();
	window_width = window_height = 0;
}
