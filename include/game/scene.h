#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <stdint.h>

enum { SCENE_INDEX_TITLE, SCENE_INDEX_TESTAREA, NUM_SCENE_INDICES };

typedef uint8_t scene_index_t;

extern scene_index_t scene_index;

#endif /* _GAME_SCENE_H_ */
