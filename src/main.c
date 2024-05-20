#include <libdragon.h>

#include "engine/config.h"
#include "engine/n64.h"

#include "game/title.h"

int main(void)
{
	n64_init();
	title_init();

	for (;;)
	{
		n64_update_ticks_accum();

		while (n64_ticks_accum >= DELTATICKS)
		{
			title_update();
			n64_ticks_accum -= DELTATICKS;
		}

		surface_t *disp = display_get();

		rdpq_attach(disp, NULL);
		rdpq_clear(RGBA16(0xB, 0x6, 0x10, 0x1));

		title_render();

		rdpq_detach();
		display_show(disp);
	}

	title_terminate();
	n64_terminate();
}
