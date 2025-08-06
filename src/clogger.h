
/*
 * This is a header-only logging library for C.
 * To use in your program simply define the `CLOG_IMPLEMENTATION` macro and include the file as so:
 * ```c
 * #define CLOG_IMPLEMENTATION
 * #include "clogger.h"
 *
 * int main(void)
 * {
 *		clog_info("This is a log.");
 *		clog_debug("This is a debug.");
 *		clog_warn("This is a warning.");
 *		clog_error("This is an error.");
 *		return 0;
 * }
 * ```
 *
 * The goal of this library is to be easily customizable[*] by the user through macro definitions.
 * Here are the implemented options so far:
 * - CLOG_SUPPRESS_COLOR:   removes the coloring from the log messages.
 * - CLOG_SUPPRESS_TAG:     removes the tag from the log messages (`INFO`, `DEBUG`, etc.).
 * - CLOG_SUPPRESS_LOC:     removes the file location and number from the log messages (`./file/to/path:line`).
 * - CLOG_SUPPRESS_NEWLINE: removes the new line from the end of the log messages.
 * - CLOG_SUPPRESS_NEWLINE: removes the new line from the end of the log messages.
 * - CLOG_*_COLOR: you can define your own preferred colors for each log level using ansi escape codes (https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797); simply #define your desired escape sequence with the corresponding name before including this file. Does nothing if the CLOG_SUPPRESS_COLOR macro is defined.
 * - CLOG_*_TAG: you can define your own preferred tag for each log level; simply #define your desired tag with the corresponding name before including this file. Does nothing if the CLOG_SUPPRESS_TAG macro is defined.
 * - CLOG_*_OUT: you can define your own preferred output file for each log level; simply #define your desired output with the corresponding name before including this file.
 *
 * [*]: This library is still in development; I do not claim to have the best or most extensive customizability, I simply implement what I need as I need it.
 */

// TODO:
// - add default colors (escape sequences) to make it easier to redefine them
// - add support for also suppressing elements for specific levels, instead of all of them at once
// - add support for coloring the message itself, instead of just the info preceding it

#ifndef _CLOG_H_
#define _CLOG_H_

typedef enum {
	CLOG_INFO,
	CLOG_DEBUG,
	CLOG_WARN,
	CLOG_ERROR,

	// always last
	CLOG_MAX,
} Clog_Level;

#ifndef CLOG_SUPPRESS_COLOR
#	ifndef CLOG_INFO_COLOR
#		define CLOG_INFO_COLOR "\x1b[1;37m"
#	endif
#	ifndef CLOG_DEBUG_COLOR
#		define CLOG_DEBUG_COLOR "\x1b[1;32m"
#	endif
#	ifndef CLOG_WARN_COLOR
#		define CLOG_WARN_COLOR "\x1b[1;33m"
#	endif
#	ifndef CLOG_ERROR_COLOR
#		define CLOG_ERROR_COLOR "\x1b[1;31m"
#	endif
#endif

#ifndef CLOG_SUPPRESS_TAG
#	ifndef CLOG_INFO_TAG
#		define CLOG_INFO_TAG "INFO"
#	endif
#	ifndef CLOG_DEBUG_TAG
#		define CLOG_DEBUG_TAG "DEBUG"
#	endif
#	ifndef CLOG_WARN_TAG
#		define CLOG_WARN_TAG "WARN"
#	endif
#	ifndef CLOG_ERROR_TAG
#		define CLOG_ERROR_TAG "ERROR"
#	endif
#endif

#ifndef CLOG_INFO_OUT
#	define CLOG_INFO_OUT stderr
#endif
#ifndef CLOG_DEBUG_OUT
#	define CLOG_DEBUG_OUT stderr
#endif
#ifndef CLOG_WARN_OUT
#	define CLOG_WARN_OUT stderr
#endif
#ifndef CLOG_ERROR_OUT
#	define CLOG_ERROR_OUT stderr
#endif

// Not recommended to use directly, but no one's going to stop you
void __clog_generic(Clog_Level level, const char *path, int line, const char *fmt, ...);

#define clog_info(...)  __clog_generic(CLOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define clog_debug(...) __clog_generic(CLOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define clog_warn(...)  __clog_generic(CLOG_WARN, __FILE__, __LINE__,__VA_ARGS__)
#define clog_error(...) __clog_generic(CLOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#endif // _CLOG_H_

#ifdef CLOG_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define UNUSED(x) ((void)(x))

#ifndef CLOG_SUPPRESS_COLOR
static const char *clog_colors[] = {
	[CLOG_INFO] = CLOG_INFO_COLOR,
	[CLOG_DEBUG] = CLOG_DEBUG_COLOR,
	[CLOG_WARN] = CLOG_WARN_COLOR,
	[CLOG_ERROR] = CLOG_ERROR_COLOR,
};
#endif // CLOG_SUPPRESS_COLOR
#ifndef CLOG_SUPPRESS_TAG
static const char *clog_tags[] = {
	[CLOG_INFO] = CLOG_INFO_TAG,
	[CLOG_DEBUG] = CLOG_DEBUG_TAG,
	[CLOG_WARN] = CLOG_WARN_TAG,
	[CLOG_ERROR] = CLOG_ERROR_TAG,
};
#endif // CLOG_SUPPRESS_TAG

inline static FILE *__clog_get_output(Clog_Level level)
{
	switch (level) {
	case CLOG_INFO:  return CLOG_INFO_OUT;
	case CLOG_DEBUG: return CLOG_DEBUG_OUT;
	case CLOG_WARN:  return CLOG_WARN_OUT;
	case CLOG_ERROR: return CLOG_ERROR_OUT;
	default:
		assert(false && "Unreachable");
	}
}

void __clog_generic(Clog_Level level, const char *path, int line, const char *fmt, ...)
{
	FILE *out = __clog_get_output(level);
#ifndef CLOG_SUPPRESS_COLOR
	fprintf(out, "%s", clog_colors[level]);
#endif // CLOG_SUPPRESS_COLOR
#ifndef CLOG_SUPPRESS_TIME
	time_t tloc;
	time(&tloc);
	struct tm *timeinfo = NULL;
	timeinfo = localtime(&tloc); // not thread safe
	fprintf(out, "[%02d:%02d:%02d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
#endif // CLOG_SUPPRESS_TIME

#ifndef CLOG_SUPPRESS_LOC
	fprintf(out, "%s:%d: ", path, line);
#else
	UNUSED(path);
	UNUSED(line);
#endif // CLOG_SUPPRESS_LOC

#ifndef CLOG_SUPPRESS_TAG
	// TODO: consider padding the tags
		// hardcoded 19 means that if the tags change this may not be formatted correctly
		// fprintf(out, "%-19s", type);
	fprintf(out, "%s: ", clog_tags[level]);
#endif // CLOG_SUPPRESS_TAG

#ifndef CLOG_SUPPRESS_COLOR
	fprintf(out, "\x1b[0m");
#endif // CLOG_SUPPRESS_COLOR
	va_list va;
	va_start(va, fmt);
	vfprintf(out, fmt, va);
	va_end(va);
#ifndef CLOG_SUPPRESS_NEWLINE
	fprintf(out, "\n");
#endif // CLOG_SUPPRESS_NEWLINE
}

#endif // CLOG_IMPLEMENTATION

