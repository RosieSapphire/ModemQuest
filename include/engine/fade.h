#ifndef _ENGINE_FADE_H_
#define _ENGINE_FADE_H_

#define FADE_RADIUS_MAX 512
#define FADE_PIXELS_PER_FRAME_DEFAULT 16

enum
{
	FADE_STATE_IN = 0,
	FADE_STATE_OUT,
	FADE_STATE_CNT,
};

void fade_init_spr(void);
void fade_init_vals(const int fade_state_new, const int fade_radius_new);
void fade_update(const int pixels_per_frame, const uint8_t start_pressed);
void fade_render(void);
void fade_terminate_spr(void);

#endif /* _ENGINE_FADE_H_ */
