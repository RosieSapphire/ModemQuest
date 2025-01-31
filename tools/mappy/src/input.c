#include "input.h"

input_t input_new, input_old;

void input_reset(void)
{
	for (int i = 0; i < KEY_COUNT; i++) {
		input_new.key_flags[i] = input_old.key_flags[i] = 0;
	}
	for (int i = 0; i < MOUSE_BUTTON_COUNT; i++) {
		input_old.mb_flags[i] = input_new.mb_flags[i] = 0;
	}
	for (int i = 0; i < 2; i++) {
		input_old.mouse_pos[i] = input_new.mouse_pos[i] = 0;
		input_old.mouse_pos_diff[i] = input_new.mouse_pos_diff[i] = 0;
	}
}

void input_poll(GLFWwindow *win)
{
	glfwPollEvents();

	/* keys */
	const uint8_t key_vals[KEY_COUNT] = {
		glfwGetKey(win, GLFW_KEY_W),
		glfwGetKey(win, GLFW_KEY_S),
		glfwGetKey(win, GLFW_KEY_A),
		glfwGetKey(win, GLFW_KEY_D),
		glfwGetKey(win, GLFW_KEY_Z),
		glfwGetKey(win, GLFW_KEY_LEFT_SHIFT),
		glfwGetKey(win, GLFW_KEY_ENTER)
	};

	input_old = input_new;
	for (int i = 0; i < KEY_COUNT; i++) {
		input_new.key_flags[i] = 0;
		input_new.key_flags[i] |= key_vals[i]
					  << INPUT_FLAG_HELD_BITSHIFT;
		input_new.key_flags[i] |=
			(((input_new.key_flags[i] & INPUT_FLAG_HELD) >>
			  INPUT_FLAG_HELD_BITSHIFT) &&
			 (((input_old.key_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT) ^
			  ((input_new.key_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT)))
			<< INPUT_FLAG_PRESSED_BITSHIFT;
		input_new.key_flags[i] |=
			(((input_old.key_flags[i] & INPUT_FLAG_HELD) >>
			  INPUT_FLAG_HELD_BITSHIFT) &&
			 (((input_old.key_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT) ^
			  ((input_new.key_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT)))
			<< INPUT_FLAG_RELEASED_BITSHIFT;
	}

	/* mouse buttons */
	const uint8_t mb_vals[MOUSE_BUTTON_COUNT] = {
		glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT),
		glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE),
		glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT)
	};

	for (int i = 0; i < MOUSE_BUTTON_COUNT; i++) {
		input_new.mb_flags[i] = 0;
		input_new.mb_flags[i] |= mb_vals[i] << INPUT_FLAG_HELD_BITSHIFT;
		input_new.mb_flags[i] |=
			(((input_new.mb_flags[i] & INPUT_FLAG_HELD) >>
			  INPUT_FLAG_HELD_BITSHIFT) &&
			 (((input_old.mb_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT) ^
			  ((input_new.mb_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT)))
			<< INPUT_FLAG_PRESSED_BITSHIFT;
		input_new.mb_flags[i] |=
			(((input_old.mb_flags[i] & INPUT_FLAG_HELD) >>
			  INPUT_FLAG_HELD_BITSHIFT) &&
			 (((input_old.mb_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT) ^
			  ((input_new.mb_flags[i] & INPUT_FLAG_HELD) >>
			   INPUT_FLAG_HELD_BITSHIFT)))
			<< INPUT_FLAG_RELEASED_BITSHIFT;
	}

	/* mouse pos */
	double mx, my;

	glfwGetCursorPos(win, &mx, &my);
	input_new.mouse_pos[0] = (int)mx;
	input_new.mouse_pos[1] = (int)my;
	for (int i = 0; i < 2; i++) {
		input_new.mouse_pos_diff[i] =
			input_new.mouse_pos[i] - input_old.mouse_pos[i];
	}
}
