#include "util.h"

#include "glfw_window.h"

#define WIN_TITLE "ModemQuest Map Editor"

GLFWwindow *glfw_win;
ivec2 glfw_win_size;

void glfw_window_init(void)
{
	int glfw_init_ret = glfwInit();
	assertf(glfw_init_ret, "GLFW failed to init\n");

	const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfw_win_size[0] = vidmode->width;
	glfw_win_size[1] = vidmode->height;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	glfw_win = glfwCreateWindow(glfw_win_size[0], glfw_win_size[1],
				    WIN_TITLE, NULL, NULL);
	glfwMakeContextCurrent(glfw_win);
}

void glfw_window_free(void)
{
	glfwDestroyWindow(glfw_win);
	glfw_win = NULL;
	glm_ivec2_zero(glfw_win_size);
	glfwTerminate();
}
