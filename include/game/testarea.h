#ifndef _GAME_TESTAREA_H_
#define _GAME_TESTAREA_H_

#include "game/scene.h"

void testarea_init(void);
scene_index_t testarea_update(const float dt);
void testarea_render(const float subtick);
void testarea_terminate(void);

#endif /* _GAME_TESTAREA_H_ */
