#ifndef _FONT_H_
#define _FONT_H_

#include <libdragon.h>

void font_init(void);
void font_printf(const float x, const float y, const rdpq_textparms_t *parms,
		 const char *fmt, ...);
void font_terminate(void);

#endif /* _FONT_H_ */
