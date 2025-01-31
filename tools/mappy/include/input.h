#ifndef _MAPPY_INPUT_H_
#define _MAPPY_INPUT_H_

#include <stdint.h>
#include <GLFW/glfw3.h>

#define INPUT_GET_KEY(KEY, STATE)                                 \
	((input_new.key_flags[KEY_##KEY] & INPUT_FLAG_##STATE) >> \
	 INPUT_FLAG_##STATE##_BITSHIFT)
#define INPUT_GET_MB(MB, STATE)                                          \
	((input_new.mb_flags[MOUSE_BUTTON_##MB] & INPUT_FLAG_##STATE) >> \
	 INPUT_FLAG_##STATE##_BITSHIFT)
#define INPUT_GET_MOUSE(COMP, STATE)                                   \
	(MOUSE_##STATE ? input_new.mouse_pos_diff[MOUSE_COMP_##COMP] : \
			 input_new.mouse_pos[MOUSE_COMP_##COMP])

enum { KEY_W, KEY_S, KEY_A, KEY_D, KEY_Z, KEY_SHIFT, KEY_ENTER, KEY_COUNT };

enum {
	INPUT_FLAG_HELD_BITSHIFT,
	INPUT_FLAG_PRESSED_BITSHIFT,
	INPUT_FLAG_RELEASED_BITSHIFT
};

enum {
	INPUT_FLAG_HELD = (1 << INPUT_FLAG_HELD_BITSHIFT),
	INPUT_FLAG_PRESSED = (1 << INPUT_FLAG_PRESSED_BITSHIFT),
	INPUT_FLAG_RELEASED = (1 << INPUT_FLAG_RELEASED_BITSHIFT)
};

enum {
	MOUSE_COMP_X,
	MOUSE_COMP_Y,
};

enum { MOUSE_NOW, MOUSE_DIFF };

enum {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_COUNT
};

typedef struct {
	int key_flags[KEY_COUNT];
	int mb_flags[MOUSE_BUTTON_COUNT], mouse_pos[2], mouse_pos_diff[2];
} input_t;

extern input_t input_new, input_old;

void input_reset(void);
void input_poll(GLFWwindow *win);

#endif /* _MAPPY_INPUT_H_ */
