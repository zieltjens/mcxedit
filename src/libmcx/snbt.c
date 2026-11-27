/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include <libmcx/snbt.h>

char *snbt_popnode(char *path)
{
	char c;
	do c = *path++;
	while (c && c != '.');
	path[-1] = '\0';
	return c ? path : NULL;
}
