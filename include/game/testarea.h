#ifndef _GAME_TESTAREA_H_
#define _GAME_TESTAREA_H_

#include <libdragon.h>

void testarea_init(void);
int testarea_update(const joypad_buttons_t pressed);
void testarea_render(void);

#endif /* _GAME_TESTAREA_H_ */
