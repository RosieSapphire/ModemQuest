#ifndef _MAPPY_NPC_H_
#define _MAPPY_NPC_H_

#define joypad_buttons_t void *
#include "game/npc.h"
#undef joypad_buttons_t

npc_t *npc_get_from_pos(const int x, const int y);

#endif /* _MAPPY_NPC_H_ */
