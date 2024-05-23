#ifndef _GAME_SCENES_H_
#define _GAME_SCENES_H_

#include <stdint.h>

enum
{
	SCENE_INVALID = -1,
	SCENE_TITLE,
	SCENE_TESTAREA,
	SCENE_CNT,
};

extern uint8_t scene_index;

#endif /* _GAME_SCENES_H_ */
