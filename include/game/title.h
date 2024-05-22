#ifndef _GAME_TITLE_H_
#define _GAME_TITLE_H_

/* sprites */
extern sprite_t *title_spr_logo;
extern sprite_t *title_spr_traces;
extern sprite_t *title_spr_gradient;

/* variables */
extern int title_bg_pos[2];

/* base */
void title_init(void);
void title_update(void);
void title_terminate(void);

/* rendering */
void title_render(void);
void title_background_render(const float time_now);
void title_logo_render(void);
void title_credits_render(void);

#endif /* _GAME_TITLE_H_ */
