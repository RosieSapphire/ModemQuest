#ifndef _ENGINE_FONT_H_
#define _ENGINE_FONT_H_

#include <libdragon.h>

void font_init(void);
void font_printf(const float x, const float y, const rdpq_textparms_t *parms,
		 const char *fmt, ...);

#endif /* _ENGINE_FONT_H_ */
