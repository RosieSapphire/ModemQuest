#ifndef _GAME_TESTAREA_H_
#define _GAME_TESTAREA_H_

#include "types.h"

void testarea_init(void);
u8 testarea_update(const f32 dt);
void testarea_render(const f32 subtick);
void testarea_free(void);

#endif /* _GAME_TESTAREA_H_ */
