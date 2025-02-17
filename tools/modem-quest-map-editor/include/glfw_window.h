#ifndef _MQME_WINDOW_H_
#define _MQME_WINDOW_H_

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern GLFWwindow *glfw_win;
extern ivec2 glfw_win_size;

void glfw_window_init(void);
void glfw_window_free(void);

#endif /* _MQME_WINDOW_H_ */
