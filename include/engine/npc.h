#ifndef _ENGINE_NPC_H_
#define _ENGINE_NPC_H_

#include <stdint.h>

#define NPC_NAME_MAX_LEN 32
#define NPC_NUM_DIALOGUE_LINES_MAX 128
#define NPC_DIALOGUE_LINE_MAX_LEN 256

enum { NPC_STATE_IDLE, NPC_STATE_TALKING, NUM_NPC_STATES };

typedef struct {
	/* FIXME: this may need to be changed to a global character max
	 * name length, since this can also use the player's name */
	char speaker[NPC_NAME_MAX_LEN];
	char line[NPC_DIALOGUE_LINE_MAX_LEN];
} dialogue_line_t;

typedef struct {
	/* engine only */
	char name[NPC_NAME_MAX_LEN];
	uint16_t pos[2], num_dialogue_lines;
	dialogue_line_t dialogue[NPC_NUM_DIALOGUE_LINES_MAX];

	/* game only */
	uint8_t state;
	int8_t dialogue_cur, dialogue_char_cur;
} npc_t;

void npc_player_interact(npc_t *n);
void npc_dialogue_box_render(const npc_t *n);

#endif /* _ENGINE_NPC_H_ */
