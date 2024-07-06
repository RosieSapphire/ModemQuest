#include "config.h"
#include "util.h"

#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/tilemap.h"
#include "game/testarea.h"

static int is_exiting;

static void testarea_terminate(void *dummy)
{
}

void testarea_init(void)
{
	vec2i player_spawn_pos;

	tilemap_load("rom:/testarea.map", player_spawn_pos);
	player_init(player_spawn_pos);
	fade_state_setup(FADE_STATE_IN);

	is_exiting = 0;
}

int testarea_update(const joypad_buttons_t pressed, const joypad_inputs_t held)
{
	for (int i = 0; i < tilemap_npc_cnt; i++)
		npc_player_interact(tilemap_npcs + i, pressed);
	player_update(held);

	int exit_cond = pressed.start & ~is_exiting;

	is_exiting ^= exit_cond;
	if (fade_update(exit_cond)) {
		rdpq_call_deferred(testarea_terminate, NULL);
		return (SCENE_TITLE);
	}

	return (SCENE_TESTAREA);
}

void testarea_render(void)
{
	rdpq_clear(RGBA16(0, 0, 0, 1));
	tilemap_render();
	player_render();
	for (int i = 0; i < tilemap_npc_cnt; i++)
		npc_dialogue_box_render(tilemap_npcs + i);
	fade_render();
}
