#ifndef _GAME_NPC_H_
#define _GAME_NPC_H_

#include "types.h"

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
	const char *speaker;
	const char *line;
} dialogue_line_t;

typedef struct
{
	int pos[2];
	u16 dialogue_line_cnt;
	dialogue_line_t dialogue[DIALOGUE_MAX];
	u8 state;
	s8 dialogue_cur, dialogue_char_cur;
} npc_t;

void npc_init(npc_t *n, const int pos[2], const u16 dialogue_line_cnt,
	      const dialogue_line_t *dialogue);
void npc_player_interact(npc_t *n, joypad_buttons_t pressed);
void npc_dialogue_box_render(const npc_t *n);

#endif /* _GAME_NPC_H_ */
