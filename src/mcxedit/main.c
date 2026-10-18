/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include "main.h"
#include "compat.h"
#include "err.h"
#include "getopt.h"
#include <assert.h>
#include <errno.h>
#include <libmcx/mcx.h>
#include <libmcx/types.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum options {
	OPT_VERBOSE = 1,
	OPT_QUIET   = 2,
	OPT_CHECK   = 4,
	OPT_DEFRAG  = 8,
	OPT_REPAIR  = 16,

	/* Options that need write access to the file. */
	OPT_NEED_WRITE = OPT_DEFRAG | OPT_REPAIR,
};

const char *argv0;
static int  signaled = 0;

static int remap_truncate(struct file f, void **map, off_t oldsize, off_t newsize, int need_write)
{
	assert(oldsize > 0);
	assert(oldsize != newsize);
	/* POSIX defines shrinking, but growing may result in implicit data loss.
	 * So remaps occur on growing. Windows always needs to remap. */
	int remap = 0 ? oldsize < newsize : 1;
	if (remap) {
		if (compat_unmap(*map, oldsize) < 0) return -1;
		if (compat_truncate(f, newsize) < 0) return -1;
		*map = compat_map(f, newsize, need_write);
		return -(*map == COMPAT_NOMAP);
	}
	return compat_truncate(f, newsize);
}

/* Processes an .mcX file with the given options.
 * Returns non-zero on failure. */
static int procmcx(const char *pat, int opt)
{
	struct file f;

	int   need_write = opt & OPT_NEED_WRITE;
	off_t size, nsize, tmp;
	struct mcx *mcx;

	/* Open the file, get and check the size. */
	size = compat_open(pat, &f, need_write);
	if (size < 0) goto err;
	if (size < MCX_TABLES_SIZE) {
		size = 0;
		if (opt & OPT_REPAIR || opt & OPT_DEFRAG)
			goto suc_close; /* Delete when performing the above. */
		warnx("cannot use '%s': Too small to contain table (%lluB < %lluB)",
			pat, (ullong)size, (ullong)MCX_TABLES_SIZE);
		goto err_close;
	}

	tmp = size % MCX_SECTOR_SIZE;
	if (tmp && !(opt & OPT_QUIET || opt & OPT_CHECK)) {
		warnx("'%s' may be corrupt: Not 4KiB sector aligned! (%+lldB)",
			pat, (llong)-tmp);
	}
	if ((uintmax_t)size > SIZE_MAX) {
		warnx("cannot use '%s': File is too large for the size_t datatype", pat);
		goto err_close;
	}

	mcx = compat_map(f, size, need_write);
	if (mcx == COMPAT_NOMAP) {
		warn("cannot map '%s'", pat);
		goto err_close;
	}

	/* Perform the requested actions from opt. */
	if (opt & OPT_CHECK) {
		off_t calcsize = mcx_calcsize(mcx);
		off_t sumsize  = mcx_sumsize(mcx);
		if (size < calcsize)
			warnx("%s: Contains chunks that exeed file size! (%lluB < %lluB)",
				pat, (ullong)size, (ullong)calcsize);
		else if (size < sumsize)
			warnx("%s: Sum of chunk sizes exeed file size! (%lluB < %lluB)",
				pat, (ullong)size, (ullong)sumsize);
	}

	if (opt & OPT_REPAIR) {
		nsize = mcx_repair(mcx, size);
		if (remap_truncate(f, (void **)&mcx, size, nsize, need_write)) {
			warn("cannot truncate '%s'", pat);
			goto err_unmap;
		}
		size = nsize;
		if (!size) goto suc_close;
	}

	if (opt & OPT_DEFRAG) {
		nsize = mcx_defrag(mcx, size);
		if (remap_truncate(f, (void **)&mcx, size, nsize, need_write)) {
			warn("cannot truncate '%s'", pat);
			goto err_unmap;
		}
		size = nsize;
		if (!size) goto suc_close;
	}

	if (compat_unmap(mcx, size) < 0)
		err(1, "cannot unmap '%s'", pat);
suc_close:
	if (compat_close(f) < 0)
		err(1, "cannot close '%s'", pat);
	if (opt & OPT_NEED_WRITE && !size) {
		if (remove(pat))
			warn("cannot remove '%s'", pat);
		else if (opt & OPT_VERBOSE)
			printf("removed '%s'\n", pat);
	}

	return 0;
err_unmap:
	if (compat_unmap(mcx, size) < 0)
		err(1, "cannot unmap '%s'", pat);
err_close:
	if (compat_close(f) < 0)
		err(1, "cannot close '%s'", pat);
err:
	return 1;
}

static void signal_report(int sig)
{
	signaled = sig;
}

/* Entry-point of the application. */
int main(int argc, char **argv)
{
	signal(SIGINT, signal_report);
	signal(SIGTERM, signal_report);
	argv0 = *argv;

	/* Load the options given in argv. */
	int opt = 0;
	int o;
	while ((o = getopt(argc, argv, "GRcqvVh")) != -1) {
		switch (o) {
		case 'G': opt |= OPT_DEFRAG; break;
		case 'R': opt |= OPT_REPAIR; break;
		case 'c': opt |= OPT_CHECK; break;
		case 'q': opt |= OPT_QUIET; break;
		case 'v': opt |= OPT_VERBOSE; break;
		case 'V':
			printf("%s: %s\n", *argv, MCXEDIT_VERSION);
			return 0;
		case 'h':
			printf(
				"%s [options] /path/to/region1 ...\n"
				"Options:\n"
				"  -G: defra[G]ment  Removes empty sectors from the file.\n"
				"  -R: [R]epair      Identifies and repairs faults within the file.\n"
				"  -c  [c]heck       Performs more thorough checks on the file.\n"
				"  -q: [q]uiet       Silences most error output.\n"
				"  -v: [v]erbose     Outputs more detailed information.\n"
				"  -V: [V]ersion     Outputs version information & exits.\n"
				"  -h: [h]elp        Shows this output & exits.\n"
				"View mcxedit(1) for more information.\n",
				*argv);
			return 0;
		case '?':
			fprintf(stderr, "Try '%s -h' for help.\n", *argv);
			return 1;
		default: errx(-SIGABRT, "Unhandled flag -- '%c', please report this bug.", o);
		}
	}

	/* Loop through the remaining options given in argv (files). */
	int    err = 0;
	char **pat = argv + optind;
	if (!*pat) {
		/* FIX: This bloody mess, I'd like the compiler to understand
		 * that it can just take the substring for the ? case above. */
		fprintf(stderr, "Expected input files\n");
		fprintf(stderr, "Try '%s -h' for help.\n", *argv);
		return 1;
	}
	while (!signaled && *pat)
		err |= procmcx(*pat++, opt);
	return signaled ? signaled + 128 : err;
}
