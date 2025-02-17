#include "font.h"

#define FONT_ID 1

static rdpq_font_t *font = NULL;

void font_init(void)
{
	font = rdpq_font_load("rom:/jetbrainsmono-bold.font64");
	rdpq_font_style(font, 0,
			&(const rdpq_fontstyle_t){
				.color = color_from_packed16(0xFFFF) });
	rdpq_text_register_font(FONT_ID, font);
}

void font_printf(const float x, const float y, const rdpq_textparms_t *parms,
		 const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	rdpq_text_printf(parms, FONT_ID, x, y, fmt, args);
	va_end(args);
}

void font_free(void)
{
	rdpq_text_unregister_font(FONT_ID);
	rdpq_font_free(font);
}
