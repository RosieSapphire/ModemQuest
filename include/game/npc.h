#ifndef _GAME_NPC_H_
#define _GAME_NPC_H_

#include "types.h"

#define NPC_NAME_MAX 32
#define DIALOGUE_MAX 128
#define DIALOGUE_LINE_MAX 256

enum
{
	NPC_STATE_IDLE,
	NPC_STATE_TALKING,
	NPC_STATE_CNT,
};

typedef struct
{
	char speaker[NPC_NAME_MAX];
	char line[DIALOGUE_LINE_MAX];
} dialogue_line_t;

typedef struct
{
	/* engine */
	char name[NPC_NAME_MAX];
	u16 pos[2], dialogue_line_cnt;
	dialogue_line_t dialogue[DIALOGUE_MAX];

	/* game-only */
	u8 state;
	s8 dialogue_cur, dialogue_char_cur;
} npc_t;

void npc_player_interact(npc_t *n, joypad_buttons_t pressed);
void npc_dialogue_box_render(const npc_t *n);

#endif /* _GAME_NPC_H_ */
