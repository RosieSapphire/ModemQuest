#include <libdragon.h>

#include "rdpq_ext.h"

void rdpq_fill_rect_border(const int x0, const int y0, const int x1,
			   const int y1, const uint16_t col,
			   const uint8_t thick)
{
	const color_t lighter = color_from_packed16(col);
	const color_t darker =
		color_from_packed16((((col & 0xF800) >> 1) & 0xF800) |
				    (((col & 0x07C0) >> 1) & 0x07C0) |
				    (((col & 0x003E) >> 1) & 0x003E) | 1);

	/* inside */
	rdpq_set_mode_fill(lighter);
	rdpq_fill_rectangle(x0 + thick, y0 + thick, x1 - thick, y1 - thick);

	/* outside */
	rdpq_set_fill_color(darker);
	rdpq_fill_rectangle(x0, y0, x0 + thick, y1); /* left */
	rdpq_fill_rectangle(x1 - thick, y0, x1, y1); /* right */
	rdpq_fill_rectangle(x0 + thick, y0, x1 - thick, y0 + thick); /* up */
	rdpq_fill_rectangle(x0 + thick, y1 - thick, x1 - thick, y1); /* down */
}
