
/*
 * MIT License
 * 
 * Copyright (c) 2025 Gustavo Feio
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This is a header-only logging library for C.
 *
 * The goal of this library is to be easily customizable[*] by the user through macro definitions.
 * Here are the implemented options so far:
 * - CLOG_SUPPRESS_COLOR:   removes the coloring from the log messages.
 * - CLOG_SUPPRESS_TAG:     removes the tag from the log messages (`INFO`, `DEBUG`, etc.).
 * - CLOG_SUPPRESS_LOC:     removes the file location and number from the log messages (`./file/to/path:line`).
 * - CLOG_SUPPRESS_NEWLINE: removes the new line from the end of the log messages.
 * - CLOG_*_COLOR: you can define your own preferred colors for each log level using CLOG_OUTPUT_* macros or custom ansi escape codes (https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797). Does nothing if the CLOG_SUPPRESS_COLOR macro is defined.
 * - CLOG_*_TAG: you can define your own preferred tag for each log level; simply #define your desired tag with the corresponding name before including this file. Does nothing if the CLOG_SUPPRESS_TAG macro is defined.
 * - CLOG_*_OUT: you can define your own preferred output file for each log level; simply #define your desired output with the corresponding name before including this file.
 * - CLOG_LOG_ORDER: you can define your own order for the timestamp, type, and location of the logs by defining this macro and defining an array with the CLOG_TIME, CLOG_TAG, and CLOG_LOC macros.
 * - CLOG_ENABLE_MESSAGE_COLOR: colors the whole log, including the message itself.
 *
 * See the `examples/`  folder for how to use the library.
 *
 * [*]: This library is still in development; I do not claim to have the best or most extensive customizability. I simply implement what I need as I need it.
 */

// TODO:
// - allow suppressing bold text independently of color
// - add support for also suppressing elements for specific levels, instead of all of them at once

#ifndef _CLOG_H_
#define _CLOG_H_

#include <stdio.h>

typedef enum {
	CLOG_INFO,
	CLOG_DEBUG,
	CLOG_WARN,
	CLOG_ERROR,
} Clog_Level;

// Not intended to be used directly, but no one's going to stop you
void __clog_generic(Clog_Level level, const char *path, int line, const char *fmt, ...);
void __clog_time     (FILE *out, Clog_Level level, const char *path, int line);
void __clog_location (FILE *out, Clog_Level level, const char *path, int line);
void __clog_tag      (FILE *out, Clog_Level level, const char *path, int line);

#define clog_info(...)  __clog_generic(CLOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define clog_debug(...) __clog_generic(CLOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define clog_warn(...)  __clog_generic(CLOG_WARN, __FILE__, __LINE__,__VA_ARGS__)
#define clog_error(...) __clog_generic(CLOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

// Macros to define a custom log order.
// See examples to learn more.
#define CLOG_TIME &__clog_time
#define CLOG_LOC  &__clog_location
#define CLOG_TAG  &__clog_tag

#ifndef CLOG_LOG_ORDER
#define CLOG_LOG_ORDER {CLOG_TIME, CLOG_LOC, CLOG_TAG}
#endif // CLOG_LOG_ORDER

#define CLOG_OUTPUT_DEFAULT "\x1b[0m"
#define CLOG_OUTPUT_BOLD    "\x1b[1m"
#define CLOG_OUTPUT_RED     "\x1b[31m"
#define CLOG_OUTPUT_GREEN   "\x1b[32m"
#define CLOG_OUTPUT_YELLOW  "\x1b[33m"
#define CLOG_OUTPUT_BLUE    "\x1b[34m"
#define CLOG_OUTPUT_MAGENTA "\x1b[35m"
#define CLOG_OUTPUT_CYAN    "\x1b[36m"
#define CLOG_OUTPUT_WHITE   "\x1b[37m"

#ifndef CLOG_SUPPRESS_COLOR
#	ifndef CLOG_INFO_COLOR
#		define CLOG_INFO_COLOR (CLOG_OUTPUT_BOLD CLOG_OUTPUT_WHITE)
#	endif
#	ifndef CLOG_DEBUG_COLOR
#		define CLOG_DEBUG_COLOR (CLOG_OUTPUT_BOLD CLOG_OUTPUT_GREEN)
#	endif
#	ifndef CLOG_WARN_COLOR
#		define CLOG_WARN_COLOR (CLOG_OUTPUT_BOLD CLOG_OUTPUT_YELLOW)
#	endif
#	ifndef CLOG_ERROR_COLOR
#		define CLOG_ERROR_COLOR (CLOG_OUTPUT_BOLD CLOG_OUTPUT_RED)
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

#ifdef CLOG_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
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

void __clog_time(FILE *out, Clog_Level level, const char *path, int line)
{
	UNUSED(level);
	UNUSED(path);
	UNUSED(line);
#ifndef CLOG_SUPPRESS_TIME
	time_t tloc;
	time(&tloc);
	struct tm *timeinfo = NULL;
	timeinfo = localtime(&tloc); // not thread safe
	fprintf(out, "[%02d:%02d:%02d] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
#else
	UNUSED(out);
#endif // CLOG_SUPPRESS_TIME
}

void __clog_location(FILE *out, Clog_Level level, const char *path, int line)
{
	UNUSED(level);
#ifndef CLOG_SUPPRESS_LOC
	fprintf(out, "%s:%d: ", path, line);
#else
	UNUSED(path);
	UNUSED(line);
#endif // CLOG_SUPPRESS_LOC
}

void __clog_tag(FILE *out, Clog_Level level, const char *path, int line)
{
	UNUSED(path);
	UNUSED(line);
#ifndef CLOG_SUPPRESS_TAG
	// TODO: consider padding the tags
		// hardcoded 19 means that if the tags change this may not be formatted correctly
		// fprintf(out, "%-19s", type);
	fprintf(out, "%s: ", clog_tags[level]);
#else
	UNUSED(out);
	UNUSED(level);
#endif // CLOG_SUPPRESS_TAG
}

typedef void (*__clog_func)(FILE *out, Clog_Level level, const char *path, int line);
static const __clog_func __clog_funcs[] = CLOG_LOG_ORDER;

void __clog_generic(Clog_Level level, const char *path, int line, const char *fmt, ...)
{
	FILE *out = __clog_get_output(level);

#ifndef CLOG_SUPPRESS_COLOR
	fprintf(out, "%s", clog_colors[level]);
#elif defined CLOG_ENABLE_MESSAGE_COLOR
#	error Both `CLOG_SUPPRESS_COLOR` and `CLOG_ENABLE_MESSAGE_COLOR` are defined. // Can't be bothered dealing with the consequences, so just throw an error
#endif // CLOG_SUPPRESS_COLOR

	for (size_t i = 0; i < sizeof(__clog_funcs)/sizeof(__clog_funcs[0]); i++) {
		__clog_funcs[i](out, level, path, line);
	}

#if !defined CLOG_SUPPRESS_COLOR && !defined CLOG_ENABLE_MESSAGE_COLOR
	fprintf(out, CLOG_OUTPUT_DEFAULT);
#endif // CLOG_SUPPRESS_COLOR

	va_list va;
	va_start(va, fmt);
	vfprintf(out, fmt, va);
	va_end(va);

#ifdef CLOG_ENABLE_MESSAGE_COLOR
	fprintf(out, CLOG_OUTPUT_DEFAULT);
#endif // CLOG_ENABLE_MESSAGE_COLOR

#ifndef CLOG_SUPPRESS_NEWLINE
	fprintf(out, "\n");
#endif // CLOG_SUPPRESS_NEWLINE
}

#endif // CLOG_IMPLEMENTATION

#endif // _CLOG_H_

