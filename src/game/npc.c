#include <libdragon.h>

#include "util.h"
#include "config.h"

#include "engine/font.h"

#include "game/player.h"
#include "game/npc.h"

void npc_init(npc_t *n, const int pos[2], const u16 dialogue_line_cnt,
	      const dialogue_line_t *dialogue)
{
	VEC2_COPY(n->pos, pos);
	n->dialogue_line_cnt = dialogue_line_cnt;
	for (int i = 0; i < dialogue_line_cnt; i++)
		n->dialogue[i] = dialogue[i];
	n->state = NPC_STATE_IDLE;
}

static int npc_dialogue_skip(npc_t *n, int *linelen)
{
	int last_char = *linelen - 1;

	/* normally, dialogue prints char by char. This skips to the end */
	if (n->dialogue_char_cur < last_char && n->dialogue_char_cur != 0)
	{
		n->dialogue_char_cur = last_char;
		return (0);
	}

	/* we've reached the end of a conversation */
	if (++n->dialogue_cur >= n->dialogue_line_cnt)
		return (1);

	/* start on whatever dialogue line is active */
	n->dialogue_char_cur = 0;
	*linelen = strlen(n->dialogue[n->dialogue_cur].line);

	return (0);
}

void npc_player_interact(npc_t *n, joypad_buttons_t pressed)
{
	int player_dist[2];

	VEC2_SUB(player_dist, player.pos, n->pos);

	int player_can_interact =
		(ABS(player_dist[0]) + ABS(player_dist[1])) == 1 &&
		player.move_timer == 0;
	int n_state_last = n->state;

	/* starting an interaction */
	if (player_can_interact && pressed.a && n->state != NPC_STATE_TALKING)
	{
		player.flags |= PLAYER_FLAG_TALKING;
		n->state = NPC_STATE_TALKING;
		n->dialogue_cur = 0;
		n->dialogue_char_cur = 0;
	}

	if (n->state != NPC_STATE_TALKING)
		return;

	int linelen = strlen(n->dialogue[n->dialogue_cur].line);
	int should_exit = 0;

	if (pressed.a && n_state_last == NPC_STATE_TALKING)
		should_exit = npc_dialogue_skip(n, &linelen);

	if (should_exit)
	{
		player.flags &= ~(PLAYER_FLAG_TALKING);
		n->state = NPC_STATE_IDLE;
		n->dialogue_cur = -1;
		return;
	}
	n->dialogue_char_cur += (n->dialogue_char_cur < linelen);
}

void npc_dialogue_box_render(const npc_t *n)
{
	const u8 alpha = 0xC0;
	const color_t lighter = RGBA32(0x18, 0x30, 0x48, alpha);
	const color_t darker = RGBA32(0x18 >> 1, 0x30 >> 1, 0x48 >> 1, alpha);

	if (n->state != NPC_STATE_TALKING)
		return;

	rdpq_set_mode_standard();
	rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
	rdpq_mode_blender(RDPQ_BLENDER((FOG_RGB, FOG_ALPHA,
					MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_set_fog_color(lighter);
	rdpq_fill_rectangle(18, DSP_HEI - (DSP_HEI >> 2) - 20,
			    18 + 80, DSP_HEI - (DSP_HEI >> 2));
	rdpq_set_fog_color(darker);
	rdpq_fill_rectangle(0, DSP_HEI - (DSP_HEI >> 2), DSP_WID, DSP_HEI);
	font_printf(18, DSP_HEI - (DSP_HEI >> 2) - 4, NULL,
		    n->dialogue[n->dialogue_cur].speaker);

	char line[DIALOGUE_LINE_MAX];

	snprintf(line, n->dialogue_char_cur + 1,
		 n->dialogue[n->dialogue_cur].line);
	font_printf(18, DSP_HEI - (DSP_HEI >> 2) + 18,
		    &(const rdpq_textparms_t) {
		    .width = DSP_WID - (18 << 1),
		    .wrap = WRAP_WORD}, line);
}
