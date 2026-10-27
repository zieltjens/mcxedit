/* SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef UTEST_TEST_H
#define UTEST_TEST_H 1
#include <errno.h>
#include <error.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TESTLOG_MSG 0x100

struct test {
	int (*test)(void);
	const char *name;
};

extern bool stdout_isatty;
extern struct test *tests;
extern size_t testlen;
extern size_t testcap;

#define test_register(func)                                                    \
	__attribute__((__constructor__))                                       \
	static void test_register_##func(void)                                 \
	{                                                                      \
		if (testlen >= testcap) {                                      \
			testcap = testcap ? testcap : 1;                       \
			testcap *= 2;                                          \
			tests = realloc(tests, testcap * sizeof(*tests));      \
			if (!tests) {                                          \
				perror("cannot realloc test array");           \
				exit(1);                                       \
			}                                                      \
		}                                                              \
		tests[testlen].test =  func;                                   \
		tests[testlen].name = #func;                                   \
		testlen++;                                                     \
	}                                                                      \
	void test_register_##func(void)

/* Execute all tests,
 * return value is 0 if all tests were successful. */
int test_main(int argc, char **argv);

/* Prints in a format similar to what returning a test would do,
 * but visually distinct to indicate this is a "subtask". */
__attribute__((__format__(printf, 3, 4)))
int test_printf(int res, const char *func, const char *fmt, ...);
int test_vprintf(int res, const char *func, const char *fmt, va_list args);

/* Short-hands for test_printf. */
#define testlog(res, ...) test_printf(res, __func__, __VA_ARGS__)
#define testmsg(...)      test_printf(TESTLOG_MSG, __func__, __VA_ARGS__)

/* Useful for chaining conditions, whilst having logging on each step. */
#define testeval(cond) (testlog(!(cond), #cond "\n"), cond)

#ifdef TEST_IMPLEMENTATION
#if defined(__unix__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "Unkown platform, unsure what action to take."
#endif

bool stdout_isatty = false;
struct test *tests = NULL;
size_t testlen = 0;
size_t testcap = 0;

int test_main(int argc, char **argv)
{
	(void)argc, (void)argv; /* NOTE: Flags unneeded at this time. */

#if defined(__unix__)
	stdout_isatty = isatty(STDOUT_FILENO);
#elif defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	stdout_isatty = !GetConsoleScreenBufferInfo(stdout_handle, &info);
#endif

	/* Execute Tests. */
	size_t n = testlen;
	size_t success = 0;
	struct test *test = tests;
	while (n--) {
		const char *str = stdout_isatty
			? "[\033[1;31mFAIL\033[0m] %s %llu/%llu\n"
			: "[FAIL] %s %llu/%llu\n";
		if (!test->test()) {
			success++;
			str = stdout_isatty
				? "[\033[1;32m OK \033[0m] %s %llu/%llu\n"
				: "[ OK ] %s %llu/%llu\n";
		}
		printf(str, test->name,
			(unsigned long long)(testlen - n),
			(unsigned long long)(testlen));
		test++;
	}
	printf("%llu/%llu tests successful.\n", success, testlen);
	return testlen != success;
}

int test_vprintf(int res, const char *func, const char *fmt, va_list args)
{
	const char *status = stdout_isatty
		? " \033[1;32m OK \033[0m  %s\t"
		: "  OK   %s\t";
	if (res) {
		if (stdout_isatty) {
			status = res == TESTLOG_MSG
				? " \033[36minfo\033[0m  %s\t"
				: " \033[1;31mFAIL\033[0m  %s\t";
		} else {
			status = res == TESTLOG_MSG
				? " info  %s\t"
				: " FAIL  %s\t";
		}
	}
	return printf(status, func) +
		vprintf(fmt, args);
}
int test_printf(int res, const char *func, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = test_vprintf(res, func, fmt, args);
	va_end(args);
	return ret;
}
#endif /* TEST_IMPLEMENTATION */
#endif /* UTEST_TEST_H */
