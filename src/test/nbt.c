/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include "utest/test.h"
#include <libmcx/error.h>
#include <libmcx/nbt.h>

static u8 nbtdat[0x1000];
static u8 *nbtdatmax = nbtdat + sizeof(nbtdat);
static struct nbt_cache cache;

static inline ssize_t logwrap(ssize_t n)
{
	if (n < 0)
		testmsg("wrapped error: %s\n", mcx_strerror(-n));
	return n;
}

static int nbt_taglen_list_of_lists(void)
{
	u8 *buf = nbtdat;
	buf += logwrap(nbt_addkey_list(buf, nbtdatmax, "list", NBT_LIST, 3));
	buf += logwrap(nbt_addkey_list(buf, nbtdatmax, NULL, NBT_COMPOUND, 1));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, "Hi!", "sloink"));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, "Hi2!", "sloink2"));
	buf += logwrap(nbt_addkey_end(buf, nbtdatmax));
	buf += logwrap(nbt_addkey_list(buf, nbtdatmax, NULL, NBT_STR, 4));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, NULL, "World"));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, NULL, "Hello"));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, NULL, "Haha"));
	buf += logwrap(nbt_addkey_str(buf, nbtdatmax, NULL, "UwU"));
	buf += logwrap(nbt_addkey_list(buf, nbtdatmax, NULL, NBT_F64, 1));
	buf += logwrap(nbt_addkey_float(buf, nbtdatmax, NULL, NBT_F64, 0.5f));

	ssize_t expected = buf - nbtdat;
	ssize_t acquired = nbt_taglen(nbtdat, expected, 0, &cache);
	if (acquired < 0)
		testmsg("%s\n", mcx_strerror(-acquired));
	if (acquired != expected)
		testmsg("expected: %lld, acquired: %lld\n",
			(llong)expected, (llong)acquired);
	return !(acquired == expected);
}
test_register(nbt_taglen_list_of_lists);

static int nbt_taglen_value(void)
{
	u8 *buf = nbtdat;
	buf += logwrap(nbt_addkey_int(buf, nbtdatmax, "value",
		NBT_S32, 0x50505050));
	ssize_t expected = buf - nbtdat;
	ssize_t acquired = nbt_taglen(nbtdat, expected, 0, &cache);
	if (acquired < 0)
		testmsg("%s\n", mcx_strerror(-acquired));
	if (acquired != expected)
		testmsg("expected: %lld, acquired: %lld\n",
			(llong)expected, (llong)acquired);
	return !(acquired == expected);
}
test_register(nbt_taglen_value);

static int nbt_taglen_compound(void)
{
	u8 *buf = nbtdat;
	buf += logwrap(nbt_addkey_compound(buf, nbtdatmax, "root"));
	buf += logwrap(nbt_addkey_end(buf, nbtdatmax));
	ssize_t expected = buf - nbtdat;
	ssize_t acquired = nbt_taglen(nbtdat, expected, 0, &cache);
	if (acquired < 0)
		testmsg("%s\n", mcx_strerror(-acquired));
	if (acquired != expected)
		testmsg("expected: %lld, acquired: %lld\n",
			(llong)expected, (llong)acquired);
	return !(acquired == expected);
}
test_register(nbt_taglen_compound);
