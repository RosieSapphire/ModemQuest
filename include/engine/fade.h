#ifndef _ENGINE_FADE_H_
#define _ENGINE_FADE_H_

#include <stdint.h>

enum {
	FADE_STATE_DISABLED = -1,
	FADE_STATE_IN = 0,
	FADE_STATE_OUT,
	FADE_STATE_CNT,
};

void fade_load_spr(void);
void fade_state_setup(const int fade_state_new);
int fade_update(const int should_switch);
void fade_render(void);

#endif /* _ENGINE_FADE_H_ */
