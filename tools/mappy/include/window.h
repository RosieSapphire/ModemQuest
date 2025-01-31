#ifndef _MAPPY_WINDOW_H_
#define _MAPPY_WINDOW_H_

#include <GLFW/glfw3.h>

#define WINDOW_NAME_MAX_LEN 256

extern GLFWwindow *window;
extern char window_name[WINDOW_NAME_MAX_LEN];
extern int window_width, window_height;

void window_init(const char *map_path);
void window_terminate(void);

#endif /* _MAPPY_WINDOW_H_ */
