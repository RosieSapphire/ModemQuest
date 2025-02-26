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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
	vfprintf(stderr, fmt, args);
#pragma clang diagnostic pop
	va_end(args);
}

void assertf(const boolean cond, const char *fmt, ...)
{
#ifndef DEBUG_ENABLED
	return;
#endif /* DEBUG_ENABLED */

	va_list args;

	if (cond) {
		return;
	}

	va_start(args, fmt);
	fprintf(stderr, "\033[0;31mERROR: \033[0m");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
	vfprintf(stderr, fmt, args);
#pragma clang diagnostic pop
	va_end(args);
	assert(0);
}

char *file_read_data(const char *path)
{
	FILE *file = fopen(path, "rb");
	size_t size;
	char *buf;

	fseek(file, 0, SEEK_END);
	size = (size_t)(ftell(file) + 1);
	rewind(file);
	buf = calloc(size, 1);
	fread(buf, 1, size, file);

	fclose(file);

	return buf;
}
