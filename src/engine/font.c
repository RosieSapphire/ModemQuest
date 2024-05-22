#include <stdarg.h>
#include <libdragon.h>

#include "engine/font.h"

#define FONT_ID 1

static rdpq_font_t *font;

void font_init(void)
{
	font = rdpq_font_load("rom:/jetbrainsmono-bold.font64");
	rdpq_font_style(font, 0, &(const rdpq_fontstyle_t){
			.color = RGBA16(0x1F, 0x1F, 0x1F, 0x1),
			});
	rdpq_text_register_font(FONT_ID, font);
}

void font_printf(const float x, const float y,
		 const rdpq_textparms_t *parms, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	rdpq_text_printf(parms, FONT_ID, x, y, fmt, args);
	va_end(args);
}
