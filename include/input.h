#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdint.h>

#define INPUT_GET_BTN(BUTTON, STATE)                                        \
	((input_new.button_flags[BUTTON_##BUTTON] & BUTTON_FLAG_##STATE) >> \
	 BUTTON_FLAG_##STATE##_SHIFT)
#define INPUT_GET_STICK(COMP)                         \
	((STICK_COMP_##COMP < STICK_COMP_MAG) ?       \
		 input_new.stick[STICK_COMP_##COMP] : \
		 input_new.stick_mag)

#define STICK_MAG_MIN 40
#define STICK_MAG_MAX 64

enum {
	STICK_COMP_X,
	STICK_COMP_Y,
	STICK_COMP_MAG,
};

enum {
	BUTTON_A,
	BUTTON_B,
	BUTTON_Z,
	BUTTON_L,
	BUTTON_R,
	BUTTON_START,
	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_RIGHT,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_DOWN,
	BUTTON_COUNT
};

enum {
	BUTTON_FLAG_HELD_SHIFT,
	BUTTON_FLAG_PRESSED_SHIFT,
	BUTTON_FLAG_RELEASED_SHIFT
};

enum {
	BUTTON_FLAG_HELD = (1 << BUTTON_FLAG_HELD_SHIFT),
	BUTTON_FLAG_PRESSED = (1 << BUTTON_FLAG_PRESSED_SHIFT),
	BUTTON_FLAG_RELEASED = (1 << BUTTON_FLAG_RELEASED_SHIFT)
};

typedef struct {
	uint8_t button_flags[BUTTON_COUNT];
	float stick[2], stick_mag;
} input_t;

extern input_t input_new, input_old;

void input_init(void);
void input_poll(void);
void input_terminate(void);

#endif /* _INPUT_H_ */
