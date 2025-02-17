#ifndef _MQME_UTIL_H_
#define _MQME_UTIL_H_

#include "types.h"

enum {
	DEBUG_TYPE_NONE,
	DEBUG_TYPE_INFO,
	DEBUG_TYPE_WARN,
	DEBUG_TYPE_ERR,
	DEBUG_TYPE_COUNT
};

void debugf(const int type, const char *fmt, ...);
void assertf(const boolean cond, const char *fmt, ...);
char *file_read_data(const char *path);

#endif /* _MQME_UTIL_H_ */
