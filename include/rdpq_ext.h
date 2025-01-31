#ifndef _RDPQ_EXT_H_
#define _RDPQ_EXT_H_

#include <stdint.h>

void rdpq_fill_rect_border(const int x0, const int y0, const int x1,
			   const int y1, const uint16_t col,
			   const uint8_t thick);

#endif /* _RDPQ_EXT_H_ */
