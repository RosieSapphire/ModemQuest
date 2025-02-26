#ifndef ENGINE_NPC_H
#define ENGINE_NPC_H

#define NPC_NAME_MAX_LEN 32
#define NPC_DIALOGUE_LINE_MAX_COUNT 256
#define NPC_DIALOGUE_LINE_MAX_LEN 64

#ifndef IS_USING_MQME
#include "vector.h"
#endif /* IS_USING_MQME */

enum { NPC_STATE_IDLE, NPC_STATE_TALKING, NUM_NPC_STATES };
enum { DIAL_SPEAKER_NPC, DIAL_SPEAKER_PLAYER };

struct dialogue_line {
	u8 speaker;
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
	u16 num_dialogue_lines;
#ifdef IS_USING_MQME
	u16 padding0;
	u32 padding1;
#endif /* IS_USING_MQME */
	struct dialogue_line *dialogue;

	/* game only */
#ifndef IS_USING_MQME
	u8 state;
	s8 dialogue_cur;
	s8 dialogue_char_cur;
#else /* IS_USING_MQME */
	u64 padding2;
#endif /* IS_USING_MQME */
};

#ifndef IS_USING_MQME
void npc_player_interact(struct npc *n);
void npc_dialogue_box_render(const struct npc *n, const char *player_name);
#endif /* IS_USING_MQME */

#endif /* ENGINE_NPC_H */
