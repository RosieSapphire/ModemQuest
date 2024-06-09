#include <libdragon.h>

#include "config.h"

#include "engine/n64.h"

u64 n64_ticks_last, n64_ticks_now, n64_ticks_accum;

void n64_init(void)
{
	/* libdragon */
	display_init(DISPLAY_RESOLUTION, DISPLAY_BITDEPTH,
		     DISPLAY_BUFFER_CNT, DISPLAY_GAMMA, DISPLAY_FILTER);
	joypad_init();
	timer_init();
	rdpq_init();
#if DEBUG
	debug_init_isviewer();
	debug_init_usblog();
	rdpq_debug_start();
#endif
	dfs_init(DFS_DEFAULT_LOCATION);

	/* varaibles */
	n64_ticks_last = timer_ticks();
	n64_ticks_now = n64_ticks_last;
	n64_ticks_accum = 0;
}

void n64_update_ticks_accum(void)
{
	u64 ticks_frame;

	n64_ticks_now = timer_ticks();
	ticks_frame = TICKS_DISTANCE(n64_ticks_last, n64_ticks_now);
	n64_ticks_last = n64_ticks_now;
	n64_ticks_accum += ticks_frame;
}
