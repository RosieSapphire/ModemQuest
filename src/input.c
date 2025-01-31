#include <libdragon.h>

#include "input.h"

input_t input_new, input_old;

void input_init(void)
{
	joypad_init();
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input_new.button_flags[i] = 0;
		input_old.button_flags[i] = 0;
	}
	input_new.stick[0] = input_new.stick[1] = input_old.stick[0] =
		input_old.stick[1] = 0.f;
}

void input_poll(void)
{
	joypad_inputs_t jp;

	joypad_poll();
	jp = joypad_get_inputs(JOYPAD_PORT_1);

	/* buttons */
	const uint8_t jp_vals[BUTTON_COUNT] = {
		jp.btn.a,    jp.btn.b,	    jp.btn.z,	   jp.btn.l,
		jp.btn.r,    jp.btn.start,  jp.btn.d_left, jp.btn.d_right,
		jp.btn.d_up, jp.btn.d_down,
	};

	input_old = input_new;
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input_new.button_flags[i] = 0;
		input_new.button_flags[i] |= jp_vals[i]
					     << BUTTON_FLAG_HELD_SHIFT;
		input_new.button_flags[i] |=
			(((input_new.button_flags[i] & BUTTON_FLAG_HELD) >>
			  BUTTON_FLAG_HELD_SHIFT) &&
			 (((input_old.button_flags[i] & BUTTON_FLAG_HELD) >>
			   BUTTON_FLAG_HELD_SHIFT) ^
			  ((input_new.button_flags[i] & BUTTON_FLAG_HELD) >>
			   BUTTON_FLAG_HELD_SHIFT)))
			<< BUTTON_FLAG_PRESSED_SHIFT;
		input_new.button_flags[i] |=
			(((input_old.button_flags[i] & BUTTON_FLAG_HELD) >>
			  BUTTON_FLAG_HELD_SHIFT) &&
			 (((input_old.button_flags[i] & BUTTON_FLAG_HELD) >>
			   BUTTON_FLAG_HELD_SHIFT) ^
			  ((input_new.button_flags[i] & BUTTON_FLAG_HELD) >>
			   BUTTON_FLAG_HELD_SHIFT)))
			<< BUTTON_FLAG_RELEASED_SHIFT;
	}

	/* stick */
	input_new.stick[0] = (float)jp.stick_x;
	input_new.stick[1] = (float)jp.stick_y;
	input_new.stick_mag = sqrtf(input_new.stick[0] * input_new.stick[0] +
				    input_new.stick[1] * input_new.stick[1]);

	/* normalize the stick so it can't go farther than 64 units */
	if (input_new.stick_mag < STICK_MAG_MIN) {
		input_new.stick_mag = 0.f;
		input_new.stick[0] = input_new.stick[1] = 0.f;
	}
	if (input_new.stick_mag > STICK_MAG_MAX) {
		const float mul = (float)STICK_MAG_MAX / input_new.stick_mag;
		input_new.stick[0] *= mul;
		input_new.stick[1] *= mul;
		input_new.stick_mag = STICK_MAG_MAX;
	}

	/* a little Modem Quest specific thing that makes moving the 2D player
	 * along the grid with the stick actually fucking possible at cardinal
	 * directions. Otherwise, without hits, it's diagonal city. */
	if (input_new.stick[0] > -STICK_MAG_MIN &&
	    input_new.stick[0] < STICK_MAG_MIN) {
		input_new.stick[0] = 0;
	}
	if (input_new.stick[1] > -STICK_MAG_MIN &&
	    input_new.stick[1] < STICK_MAG_MIN) {
		input_new.stick[1] = 0;
	}
}

void input_terminate(void)
{
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input_new.button_flags[i] = 0;
		input_old.button_flags[i] = 0;
	}
	input_new.stick[0] = input_new.stick[1] = input_old.stick[0] =
		input_old.stick[1] = 0.f;
	joypad_close();
}
