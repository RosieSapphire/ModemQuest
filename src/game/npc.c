#include <libdragon.h>

#include "engine/config.h"
#include "engine/font.h"

#include "game/player.h"
#include "game/npc.h"

void npc_init(npc_t *n, const uint16_t x, const uint16_t y,
	      const uint16_t dialogue_line_cnt,
	      const dialogue_line_t *dialogue)
{
	n->x = x;
	n->y = y;
	n->dialogue_line_cnt = dialogue_line_cnt;
	for (int i = 0; i < dialogue_line_cnt; i++)
		n->dialogue[i] = dialogue[i];
	n->dialogue_cur = -1;
}

void npc_player_interact(npc_t *n, joypad_buttons_t pressed)
{
	if (player.x != n->x || player.y != n->y || player.move_timer != 0)
	{
		n->dialogue_cur = -1;
		return;
	}

	player.flags |= PLAYER_FLAG_TALKING;
	n->dialogue_cur = 0;
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
	font_printf(18, DISPLAY_HEIGHT - (DISPLAY_HEIGHT >> 2) + 18,
		    &(const rdpq_textparms_t) {
		    .width = DISPLAY_WIDTH - (18 << 1),
		    .wrap = WRAP_WORD,
		    }, n->dialogue[n->dialogue_cur].line);
}
