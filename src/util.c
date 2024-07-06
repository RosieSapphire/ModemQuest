#include <libdragon.h>

#include "util.h"

void rdpq_fill_rect_border(const int x0, const int y0, const int x1,
			   const int y1, const u16 col, const uint8_t thick)
{
	const u16 lighter16 = col;
	const u16 darker16 = (((col & 0xF800) >> 1) & 0xF800) |
			     (((col & 0x07C0) >> 1) & 0x07C0) |
			     (((col & 0x003E) >> 1) & 0x003E) | 1;
	const color_t lighter = color_from_packed16(lighter16);
	const color_t darker = color_from_packed16(darker16);

	rdpq_set_mode_fill(lighter);
	rdpq_fill_rectangle(x0 + thick, y0 + thick, x1 - thick, y1 - thick);
	rdpq_set_fill_color(darker);
	rdpq_fill_rectangle(x0, y0, x0 + thick, y1); /* left */
	rdpq_fill_rectangle(x1 - thick, y0, x1, y1); /* right */
	rdpq_fill_rectangle(x0 + thick, y0, x1 - thick, y0 + thick); /* up */
	rdpq_fill_rectangle(x0 + thick, y1 - thick, x1 - thick, y1); /* down */
}
