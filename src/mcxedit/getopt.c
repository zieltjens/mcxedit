/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include "getopt.h"

#include <assert.h>
#include <libmcx/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"

char *optarg = NULL;
int   opterr = 1;
int   optind = 1;
int   optopt = 0;

static int optind2 = 1;

/* Increments optind, after ensuring argv has been reordered so
 * the option (and its argument) comes ahead of non-options.
 * Always returns ret. */
static int opt_unwind(char **argv, int idx, int nargs, int ret)
{
	assert(nargs == 1 || nargs == 2);
	if (optind != idx) {
		char **cargv = argv + optind;
		char  *args[2];
		size_t nargsb = sizeof(*argv) * nargs;
		memcpy(args, argv + idx, sizeof(args)); /* argv[argc] == NULL. */
		memmove(cargv + nargs, cargv, sizeof(*argv) * (idx - optind));
		memcpy(cargv, args, nargsb);
	}
	optind += nargs;
	optind2 = 1;
	return ret;
}

int getopt(int argc, char *const *_argv, const char *opts)
{
	char **argv = (char **)_argv;
	assert(argv[argc] == NULL); /* Likely unnecessary, but Windows. */
	if (optind >= argc) return -1;

	int idx = optind;
	optarg  = NULL;
	optopt  = 0;

	/* Set idx to the first option. */
	while ((argv[idx][0] != '-' || !argv[idx][1]) && ++idx < argc);
	if (idx == argc) return -1;

	/* Terminate when "--" option has been reached. */
	if (argv[idx][1] == '-' && !argv[idx][2])
		return opt_unwind(argv, idx, 1, -1);

	/* Find a matching option out of opts. */
	const char *opt = strchr(opts + (*opts == ':'), argv[idx][optind2]);
	if (!opt || *opt == ':') {
		optopt = argv[idx][optind2];
		if (opterr && *opts != ':')
			warnx("invalid option -- %c", optopt);
		if (!argv[idx][++optind2])
			return opt_unwind(argv, idx, 1, '?');
		return '?';
	}

	/* If not a parametrised option. */
	if (opt[1] != ':') {
		if (!argv[idx][++optind2])
			return opt_unwind(argv, idx, 1, *opt);
		return *opt;
	}

	optarg = argv[idx + 1];
	if (argv[idx][optind2 + 1]) {
		optarg = &argv[idx][optind2 + 1];
		return opt_unwind(argv, idx, 1, *opt);
	}
	if (!optarg) {
		int ret = *opts == ':' ? ':' : '?';
		optopt  = *opt;
		if (opterr && *opts != ':')
			warnx("option requires an argument -- %c", optopt);
		return opt_unwind(argv, idx, 1, ret);
	}
	return opt_unwind(argv, idx, 2, *opt);
}
