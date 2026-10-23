/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include "utest/test.h"
#include <libmcx/error.h>
#include <libmcx/nbt.h>

static u8 nbtdat[0x1000];
static u8 *nbtdatmax = nbtdat + sizeof(nbtdat);
static struct nbt_cache cache;

static __maybe_unused ssize_t addkey_end(u8 *buf, u8 *max)
{
	ssize_t n = nbt_addkey_end(buf, max);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_int(u8 *buf, u8 *max,
	const char *restrict name, enum nbt_tagid id, u64 val)
{
	ssize_t n = nbt_addkey_int(buf, max, name, id, val);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_float(u8 *buf, u8 *max,
	const char *restrict name, enum nbt_tagid id, f64 val)
{
	ssize_t n = nbt_addkey_float(buf, max, name, id, val);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_arr(u8 *buf, u8 *max,
	const char *restrict name, enum nbt_tagid id, s32 len,
	const void *restrict dat)
{
	ssize_t n = nbt_addkey_arr(buf, max, name, id, len, dat);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_str(u8 *buf, u8 *max,
	const char *restrict name, const char *restrict str)
{
	ssize_t n = nbt_addkey_str(buf, max, name, str);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_list(u8 *buf, u8 *max,
	const char *restrict name, enum nbt_tagid id, s32 len)
{
	ssize_t n = nbt_addkey_list(buf, max, name, id, len);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}
static __maybe_unused ssize_t addkey_compound(u8 *buf, u8 *max,
	const char *restrict name)
{
	ssize_t n = nbt_addkey_compound(buf, max, name);
	if (n<0) testmsg("%s\n", mcx_strerror(-n));
	return n;
}

static int nbt_taglen_list_of_lists(void)
{
	u8 *buf = nbtdat;
	buf += addkey_compound(buf, nbtdatmax, "root");
	buf += addkey_list(buf, nbtdatmax, "list", NBT_LIST, 3);
	buf += addkey_list(buf, nbtdatmax, NULL, NBT_COMPOUND, 1);
	buf += addkey_str(buf, nbtdatmax, "Hi!", "sloink");
	buf += addkey_str(buf, nbtdatmax, "Hi2!", "sloink2");
	buf += addkey_end(buf, nbtdatmax);
	buf += addkey_list(buf, nbtdatmax, NULL, NBT_STR, 4);
	buf += addkey_str(buf, nbtdatmax, NULL, "World");
	buf += addkey_str(buf, nbtdatmax, NULL, "Hello");
	buf += addkey_str(buf, nbtdatmax, NULL, "Haha");
	buf += addkey_str(buf, nbtdatmax, NULL, "UwU");
	buf += addkey_list(buf, nbtdatmax, NULL, NBT_F64, 1);
	buf += addkey_float(buf, nbtdatmax, NULL, NBT_F64, 0.5f);
	buf += addkey_end(buf, nbtdatmax);

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
