#include <libdragon.h>
#include <string.h>

#include "vec2.h"
#include "input.h"
#include "config.h"
#include "font.h"

#include "engine/player.h"
#include "engine/npc.h"

static int npc_handle_dialogue(npc_t *n, int *line_len)
{
	int char_last = *line_len - 1;

	/* normally, dialogue prints char by char, but
	 * this code will skip it to the end */
	if ((n->dialogue_char_cur < char_last) && n->dialogue_char_cur) {
		n->dialogue_char_cur = char_last;
		return 0;
	}

	/* FIXME: This is not tickrate aligned. Need to have some kind
	 * of timer that fixes how many characters can be wrote out a second */

	/* we've reached the end of a conversation */
	if (++n->dialogue_cur >= n->num_dialogue_lines) {
		return 1;
	}

	/* start on whatever dialogue line is active */
	n->dialogue_char_cur = 0;
	*line_len = strlen(n->dialogue[n->dialogue_cur].line);

	return 0;
}

void npc_player_interact(npc_t *n)
{
	vec2i player_dist;
	vec2i npc_pos = { n->pos[0], n->pos[1] };

	/* FIXME: This code is being run every frame for every NPC.
	 * It should only be running on a given NPC if the distance is
	 * close enough that it would actually matter.
	 * Optimization for later */

	vec2i_sub(player_dist, player.pos, npc_pos);

	int player_is_close_enough = ((int)fabsf((float)player_dist[0]) +
				      (int)fabsf((float)player_dist[1])) == 1 &&
				     player.move_timer == 0;
	int player_is_facing = false;
	for (int i = 0; i < 4; i++) {
		const vec2i dist_vecs[4] = {
			{ -1, 0 }, /* on left */
			{ 1, 0 }, /* on right */
			{ 0, -1 }, /* above */
			{ 0, 1 } /* below */
		};
		const int valid_dirs[4] = {
			PLAYER_DIR_RIGHT,
			PLAYER_DIR_LEFT,
			PLAYER_DIR_DOWN,
			PLAYER_DIR_UP,
		};

		if (vec2i_equals(dist_vecs[i], player_dist)) {
			if (player.dir == valid_dirs[i]) {
				player_is_facing = true;
				break;
			}
		}
	}

	int npc_state_last = n->state;

	/* starting an interaction */
	if (player_is_close_enough && player_is_facing &&
	    INPUT_GET_BTN(A, PRESSED) && (n->state != NPC_STATE_TALKING)) {
		player.flags |= PLAYER_FLAG_IS_TALKING;
		n->state = NPC_STATE_TALKING;
		n->dialogue_cur = 0;
		n->dialogue_char_cur = 0;
	}

	/* if not, we just bounce */
	if (n->state != NPC_STATE_TALKING) {
		return;
	}

	int line_len = strlen(n->dialogue[n->dialogue_cur].line);
	int should_exit = false;

	if (INPUT_GET_BTN(A, PRESSED) &&
	    (npc_state_last == NPC_STATE_TALKING)) {
		should_exit = npc_handle_dialogue(n, &line_len);
	}

	if (!should_exit) {
		n->dialogue_char_cur += (n->dialogue_char_cur < line_len);
		return;
	}

	/* exiting dialogue */
	player.flags &= ~(PLAYER_FLAG_IS_TALKING);
	n->state = NPC_STATE_IDLE;
	n->dialogue_cur = -1;
}

void npc_dialogue_box_render(const npc_t *n)
{
	if (n->state != NPC_STATE_TALKING) {
		return;
	}

	const uint8_t alpha = 0xC0;
	const color_t lighter_col = RGBA32(0x18, 0x30, 0x48, alpha);
	const color_t darker_col = RGBA32(0xC, 0x18, 0x24, alpha);

	rdpq_set_mode_standard();

	rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
	rdpq_mode_blender(
		RDPQ_BLENDER((FOG_RGB, FOG_ALPHA, MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_set_fog_color(lighter_col);

	/* speaker box */
	const char *speaker_name = n->dialogue[n->dialogue_cur].speaker;
	rdpq_fill_rectangle(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) - 20,
			    20 + (8 * strlen(speaker_name)),
			    DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2));

	/* dialogue box */
	rdpq_set_fog_color(darker_col);
	rdpq_fill_rectangle(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2),
			    DISPLAY_WIDTH - 18, DISPLAY_HEIGHT - 6);

	/* speaker's name */
	font_printf(22, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) - 6, NULL,
		    speaker_name);

	/* speaker's dialogue */
	char line[NPC_DIALOGUE_LINE_MAX_LEN];
	strncpy(line, n->dialogue[n->dialogue_cur].line,
		n->dialogue_char_cur + 1);
	font_printf(22, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) + 18,
		    &(const rdpq_textparms_t){ .width = DISPLAY_WIDTH - 36,
					       .wrap = WRAP_WORD },
		    line);
}
