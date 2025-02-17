#ifndef _GAME_FADE_TRANSITION_H_
#define _GAME_FADE_TRANSITION_H_

enum {
	FADE_TRANSITION_DISABLED = -1,
	FADE_TRANSITION_IN,
	FADE_TRANSITION_OUT,
	NUM_FADE_TRANSITIONS
};

void fade_transition_init(void);
void fade_transition_set(const int fade_state_new);
int fade_transition_update(const int attempting_exit, const float dt);
void fade_transition_render(const float subtick);
void fade_transition_free(void);

#endif /* _GAME_FADE_TRANSITION_H_ */
