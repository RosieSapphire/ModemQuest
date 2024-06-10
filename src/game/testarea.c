#include "config.h"
#include "util.h"

#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/tilemap.h"
#include "game/testarea.h"

static int is_exiting;
static npc_t testnpc;

static void testarea_terminate(void *dummy)
{}

void testarea_init(void)
{
	vec2i player_spawn_pos;

	tilemap_init("rom:/testarea.map", player_spawn_pos);
	player_init(player_spawn_pos);
	debugf("%d, %d\n", player_spawn_pos[0], player_spawn_pos[1]);
	npc_init(&testnpc, (int[2]) {8, 6}, 4,
		 (const dialogue_line_t[4]) {
		{
		.speaker = "Tupac",
		.line = "First off, fuck yo bitch in the clique you claim",
		},
		{
		.speaker = "Tupac",
		.line = "West Side, when we ride, come equipped with game",
		},
		{
		.speaker = "Tupac",
		.line = "You claimed to be a player, but I fucked your wife",
		},
		{
		.speaker = "Tupac",
		.line = "We bust on Bad Boys, niggas fucked for life",
		},
		});
	fade_state_setup(FADE_STATE_IN);

	is_exiting = 0;
}

int testarea_update(const joypad_buttons_t pressed,
		    const joypad_inputs_t held)
{
	npc_player_interact(&testnpc, pressed);
	player_update(held);

	int exit_cond = pressed.start & ~is_exiting;

	is_exiting ^= exit_cond;
	if (fade_update(exit_cond))
	{
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
	npc_dialogue_box_render(&testnpc);
	fade_render();
}
