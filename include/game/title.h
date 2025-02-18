#ifndef _GAME_TITLE_H_
#define _GAME_TITLE_H_

#include "types.h"

void title_init(void);
u8 title_update(const f32 dt);
void title_render(const f32 subtick);
void title_free(void);

#endif /* _GAME_TITLE_H_ */
