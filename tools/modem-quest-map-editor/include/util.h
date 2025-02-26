#ifndef MQME_UTIL_H
#define MQME_UTIL_H

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

#endif /* MQME_UTIL_H */
