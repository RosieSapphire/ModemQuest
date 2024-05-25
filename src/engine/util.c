#include <libdragon.h>

#include "engine/util.h"

float lerpf(const float a, const float b, const float t)
{
	return (a + (b - a) * t);
}

void rdpq_fill_rect_border(const int x0, const int y0,
			   const int x1, const int y1,
			   const uint8_t r, const uint8_t g, const uint8_t b,
			   const uint8_t thick)
{
	const color_t lighter = RGBA16(r, g, b, 0x1);
	const color_t darker = RGBA16(r >> 1, g >> 1, b >> 1, 0x1);

	rdpq_set_mode_fill(lighter);
	rdpq_fill_rectangle(x0 + thick, y0 + thick, x1 - thick, y1 - thick);
	rdpq_set_fill_color(darker);
	rdpq_fill_rectangle(x0, y0, x0 + thick, y1); /* left */
	rdpq_fill_rectangle(x1 - thick, y0, x1, y1); /* right */
	rdpq_fill_rectangle(x0 + thick, y0, x1 - thick, y0 + thick); /* up */
	rdpq_fill_rectangle(x0 + thick, y1 - thick, x1 - thick, y1); /* down */
}
