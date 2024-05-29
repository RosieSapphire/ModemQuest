#ifndef _GAME_NPC_H_
#define _GAME_NPC_H_

#include <stdint.h>

#define DIALOGUE_MAX 128

typedef struct
{
	const char *speaker;
	const char *line;
} dialogue_line_t;

typedef struct
{
	uint16_t x, y, dialogue_line_cnt;
	dialogue_line_t dialogue[DIALOGUE_MAX];
	int8_t dialogue_cur;
} npc_t;

void npc_init(npc_t *n, const uint16_t x, const uint16_t y,
	      const uint16_t dialogue_line_cnt,
	      const dialogue_line_t *dialogue);
void npc_player_interact(npc_t *n, joypad_buttons_t pressed);
void npc_dialogue_box_render(const npc_t *n);

#endif /* _GAME_NPC_H_ */
