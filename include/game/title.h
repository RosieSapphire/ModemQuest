#ifndef _GAME_TITLE_H_
#define _GAME_TITLE_H_

#include "game/scene.h"

void title_init(void);
scene_index_t title_update(const float dt);
void title_render(const float subtick);
void title_free(void);

#endif /* _GAME_TITLE_H_ */
