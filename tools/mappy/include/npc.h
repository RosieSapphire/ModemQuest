#ifndef _MAPPY_NPC_H_
#define _MAPPY_NPC_H_

#include "engine/npc.h"

npc_t *npc_get_from_pos(const int x, const int y);
void npc_duplicate(npc_t *to, const npc_t *from);
void npc_destroy(npc_t *n);

#endif /* _MAPPY_NPC_H_ */
