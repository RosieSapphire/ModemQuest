#ifndef _ENGINE_NPC_H_
#define _ENGINE_NPC_H_

#define NPC_NAME_MAX_LEN 32
#define NPC_DIALOGUE_LINE_MAX_COUNT 256
#define NPC_DIALOGUE_LINE_MAX_LEN 256

#ifndef IS_USING_MQME
#include "vector.h"
#endif /* IS_USING_MQME */

enum { NPC_STATE_IDLE, NPC_STATE_TALKING, NUM_NPC_STATES };

struct dialogue_line {
	/* FIXME: this may need to be changed to a global character max
	 * name length, since this can also use the player's name */
	char speaker[NPC_NAME_MAX_LEN];
	char line[NPC_DIALOGUE_LINE_MAX_LEN];
};

struct npc {
	/* engine only */
	char name[NPC_NAME_MAX_LEN];
#ifndef IS_USING_MQME
	vec2s pos;
#else /* IS_USING_MQME */
	ivec2 pos;
#endif /* IS_USING_MQME */
	u8 num_dialogue_lines;
	struct dialogue_line dialogue[NPC_DIALOGUE_LINE_MAX_COUNT];

	/* game only */
#ifndef IS_USING_MQME
	u8 state;
	s8 dialogue_cur;
	s8 dialogue_char_cur;
#endif /* IS_USING_MQME */
};

#ifndef IS_USING_MQME
void npc_player_interact(struct npc *n);
void npc_dialogue_box_render(const struct npc *n);
#endif /* IS_USING_MQME */

#endif /* _ENGINE_NPC_H_ */
