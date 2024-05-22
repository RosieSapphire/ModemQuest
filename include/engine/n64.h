#ifndef _ENGINE_N64_H_
#define _ENGINE_N64_H_

#include <stdint.h>

extern uint64_t n64_ticks_last, n64_ticks_now, n64_ticks_accum;

void n64_init(void);
void n64_update_ticks_accum(void);

#endif /* _ENGINE_N64_H_ */
