#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glwin.h"

GLFWwindow *glwin;

void glwin_init(const char *outpath)
{
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
}

void glwin_input_poll(glwin_input_t *inp)
{
	inp->mx_last = inp->mx_now;
	inp->my_last = inp->my_now;
	glfwGetCursorPos(glwin, &inp->mx_now, &inp->my_now);
	inp->w = glfwGetKey(glwin, GLFW_KEY_W);
	inp->s = glfwGetKey(glwin, GLFW_KEY_S);
	inp->a = glfwGetKey(glwin, GLFW_KEY_A);
	inp->d = glfwGetKey(glwin, GLFW_KEY_D);
	inp->z_last = inp->z_now;
	inp->z_now = glfwGetKey(glwin, GLFW_KEY_Z);
	inp->lmb_last = inp->lmb_last;
	inp->lmb_now = glfwGetMouseButton(glwin, GLFW_MOUSE_BUTTON_LEFT);
	inp->rmb_last = inp->rmb_now;
	inp->rmb_now = glfwGetMouseButton(glwin, GLFW_MOUSE_BUTTON_RIGHT);
	inp->mmb_last = inp->mmb_now;
	inp->mmb_now = glfwGetMouseButton(glwin, GLFW_MOUSE_BUTTON_MIDDLE);
	inp->enter_last = inp->enter_now;
	inp->enter_now = glfwGetKey(glwin, GLFW_KEY_ENTER);
	inp->shift_last = inp->shift_now;
	inp->shift_now = glfwGetKey(glwin, GLFW_KEY_LEFT_SHIFT);

	glfwPollEvents();
}
