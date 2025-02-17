#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

#include "util.h"

void debugf(const int type, const char *fmt, ...)
{
#ifndef DEBUG_ENABLED
	return;
#endif /* DEBUG_ENABLED */

	va_list args;
	va_start(args, fmt);
	switch (type) {
	case DEBUG_TYPE_NONE:
		break;

	case DEBUG_TYPE_INFO:
		fprintf(stderr, "\033[0;36mINFO: \033[0m");
		break;

	case DEBUG_TYPE_WARN:
		fprintf(stderr, "\033[0;33mWARNING: \033[0m");
		break;

	case DEBUG_TYPE_ERR:
		fprintf(stderr, "\033[0;31mERROR: \033[0m");
		break;
	default:
		return;
	}
	vfprintf(stderr, fmt, args);
	va_end(args);
}

void assertf(const boolean cond, const char *fmt, ...)
{
#ifndef DEBUG_ENABLED
	return;
#endif /* DEBUG_ENABLED */
	if (cond) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "\033[0;31mERROR: \033[0m");
	vfprintf(stderr, fmt, args);
	va_end(args);
	assert(0);
}

char *file_read_data(const char *path)
{
	FILE *file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);

	u32 size = ftell(file);
	rewind(file);

	char *buf = calloc(size + 1, 1);
	fread(buf, 1, size, file);

	fclose(file);

	return buf;
}
