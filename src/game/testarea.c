#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/testarea.h"

static int is_exiting;

static void testarea_terminate(void *dummy)
{}

void testarea_init(void)
{
	fade_state_setup(FADE_STATE_IN);

	is_exiting = 0;
}

int testarea_update(const joypad_buttons_t pressed)
{
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
	rdpq_clear(RGBA16(0x3, 0x6, 0x9, 0x1));
	fade_render();
}
