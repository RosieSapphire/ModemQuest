#include <libdragon.h>

#include "engine/config.h"
#include "engine/font.h"

#include "game/player.h"
#include "game/npc.h"

void npc_init(npc_t *n, const vec2i_t pos, const uint16_t dialogue_line_cnt,
	      const dialogue_line_t *dialogue)
{
	n->pos = pos;
	n->dialogue_line_cnt = dialogue_line_cnt;
	for (int i = 0; i < dialogue_line_cnt; i++)
		n->dialogue[i] = dialogue[i];
	n->dialogue_cur = -1;
}

static int npc_dialogue_advance(npc_t *n, int pressed_a, int *linelen)
{
	if (!pressed_a)
		return (0);

	int last_char = *linelen - 1;

	/* normally, dialogue prints char by char. This skips to the end */
	if (n->dialogue_char_cur < last_char)
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
	/* TODO: This needs to be changed */
	if (player.pos.x != n->pos.x || player.pos.y != n->pos.y ||
	    player.move_timer != 0)
	{
		n->dialogue_cur = -1;
		return;
	}

	/* if this npc isn't currently talking, but we're trying to, talk */
	if (n->dialogue_cur == -1)
	{
		player.flags |= PLAYER_FLAG_TALKING;
		n->dialogue_cur = 0;
		n->dialogue_char_cur = 0;
		return;
	}
	int linelen = strlen(n->dialogue[n->dialogue_cur].line);
	int should_exit = 0;

	should_exit = npc_dialogue_advance(n, pressed.a, &linelen);
	if (should_exit)
		goto exit_convo;

	n->dialogue_char_cur += (n->dialogue_char_cur < linelen);

	if (n->dialogue_cur >= n->dialogue_line_cnt)
	{
exit_convo:
		player.flags &= ~(PLAYER_FLAG_TALKING);
		n->dialogue_cur = -1;
		return;
	}
}

void npc_dialogue_box_render(const npc_t *n)
{
	const color_t lighter = RGBA32(0x18, 0x30, 0x48, 0x7F);
	const color_t darker = RGBA32(0x18 >> 1, 0x30 >> 1, 0x48 >> 1, 0x7F);

	if (n->dialogue_cur == -1)
		return;

	rdpq_set_mode_standard();
	rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
	rdpq_mode_blender(RDPQ_BLENDER((FOG_RGB, FOG_ALPHA,
					MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_set_fog_color(lighter);
	rdpq_fill_rectangle(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) - 20,
			    18 + 80, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2));
	rdpq_set_fog_color(darker);
	rdpq_fill_rectangle(0, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2),
			    DISPLAY_WIDTH, DISPLAY_HEIGHT);
	font_printf(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) - 4, NULL,
		    n->dialogue[n->dialogue_cur].speaker);

	char line[DIALOGUE_LINE_MAX];

	snprintf(line, n->dialogue_char_cur + 1,
		 n->dialogue[n->dialogue_cur].line);
	font_printf(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) + 18,
		    &(const rdpq_textparms_t) {
		    .width = DISPLAY_WIDTH - (18 << 1),
		    .wrap = WRAP_WORD}, line);
}
