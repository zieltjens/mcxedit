/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include "err.h"

#include "main.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <errhandlingapi.h>
#include <windows.h>
#endif

#if defined(_WIN32)
static void windows_print_error(void)
{
	void *buf;
	u32 e = GetLastError();
	u32 n = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		e,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char *)&buf, 0, NULL);
	if (n != 0) {
		fputs(buf, stderr);
		LocalFree(buf);
	} else {
		fputc('\n', stderr);
	}
}
#endif

void vwarn(const char *fmt, va_list args)
{
	int e = errno;
	fprintf(stderr, "%s: ", argv0);
	if (fmt) {
		vfprintf(stderr, fmt, args);
		fputs(": ", stderr);
	}
#if defined(_WIN32)
	if (!e) return windows_print_error();
#endif
	fprintf(stderr, "%s\n", strerror(e));
}

void vwarnx(const char *fmt, va_list args)
{
	fprintf(stderr, "%s: ", argv0);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
}

void verr(int code, const char *fmt, va_list args)
{
	vwarn(fmt, args);
	exit(code);
}

void verrx(int code, const char *fmt, va_list args)
{
	vwarnx(fmt, args);
	exit(code);
}

void err(int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	verr(code, fmt, args);
	va_end(args);
}

void errx(int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	verrx(code, fmt, args);
	va_end(args);
}

void warn(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vwarn(fmt, args);
	va_end(args);
}

void warnx(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vwarnx(fmt, args);
	va_end(args);
}
