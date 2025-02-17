#ifndef _MAPPY_DOOR_H_
#define _MAPPY_DOOR_H_

#include "engine/door.h"

door_t *door_get_from_pos(const int x, const int y);
void door_duplicate(door_t *dst, const door_t *src);
void door_destroy(door_t *d);

#endif /* _MAPPY_DOOR_H_ */
