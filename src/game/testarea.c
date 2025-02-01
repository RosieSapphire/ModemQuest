#include <libdragon.h>

#include "input.h"
#include "vec2.h"

#include "engine/tilemap.h"
#include "engine/player.h"

#include "game/fade_transition.h"
#include "game/testarea.h"

void testarea_init(void)
{
	vec2i player_spawn_pos;

	tilemap_init("rom:/testarea.map", player_spawn_pos);
	player_init(player_spawn_pos);
	fade_transition_set(FADE_TRANSITION_IN);
}

scene_index_t testarea_update(const float dt)
{
	if (fade_transition_update(INPUT_GET_BTN(START, PRESSED), dt)) {
		return SCENE_INDEX_TITLE;
	}

	tilemap_update();
	player_update(dt);

	return SCENE_INDEX_TESTAREA;
}

void testarea_render(const float subtick)
{
	rdpq_clear(color_from_packed16(0x0));
	tilemap_render(subtick);
	player_render(subtick);
	tilemap_render_npc_dialogue_boxes();
	fade_transition_render(subtick);
}

void testarea_terminate(void)
{
	player_terminate();
	tilemap_terminate();
}
