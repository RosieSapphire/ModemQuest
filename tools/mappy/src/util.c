#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"

void errorf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}
