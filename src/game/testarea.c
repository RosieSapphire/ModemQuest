#include "engine/fade.h"
#include "engine/config.h"
#include "engine/util.h"

#include "game/scene_index.h"
#include "game/player.h"
#include "game/npc.h"
#include "game/tiles.h"
#include "game/testarea.h"

static int is_exiting;
static npc_t testnpc;

static void testarea_terminate(void *dummy)
{}

void testarea_init(void)
{
	vec2i_t player_spawn_pos;

	tiles_init("rom:/testarea.map", &player_spawn_pos);
	player_init(player_spawn_pos);
	npc_init(&testnpc, VEC2I(8, 6), 2,
		 (const dialogue_line_t[2]) {
		{
			.speaker = "Test NPC",
			.line = "This is just a test line",
		},
		{
			.speaker = "Test NPC",
			.line = "I really hope this works",
		},
		});
	fade_state_setup(FADE_STATE_DISABLED);

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
	tiles_render();
	player_render();
	npc_dialogue_box_render(&testnpc);
	fade_render();
}
