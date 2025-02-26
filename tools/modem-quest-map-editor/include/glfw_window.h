#ifndef MQME_WINDOW_H
#define MQME_WINDOW_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern GLFWwindow *glfw_win;
extern ivec2 glfw_win_size;

void glfw_window_init(void);
void glfw_window_free(void);

#endif /* MQME_WINDOW_H */
