/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_NBT_H
#define LIBMCX_NBT_H 1
#include <stdint.h>
#include <sys/types.h>

#define NBT_NEST_MAX 512

/* https://minecraft.wiki/w/NBT_format#Binary_format */
enum nbt_tagid {
	NBT_END      = 0x00,
	NBT_S8       = 0x01,
	NBT_S16      = 0x02,
	NBT_S32      = 0x03,
	NBT_S64      = 0x04,
	NBT_F32      = 0x05,
	NBT_F64      = 0x06,
	NBT_ARR_S8   = 0x07,
	NBT_STR      = 0x08,
	NBT_LIST     = 0x09,
	NBT_COMPOUND = 0x0A,
	NBT_ARR_S32  = 0x0B,
	NBT_ARR_S64  = 0x0C,
};

struct nbt_cache {
	uint8_t tags[NBT_NEST_MAX];
	int32_t lens[NBT_NEST_MAX];
};

ssize_t nbt_taglen(const uint8_t *restrict tag, size_t maxlen, int root,
	struct nbt_cache *restrict cache);

ssize_t nbt_addkey_end(uint8_t *buf, uint8_t *max);

ssize_t nbt_addkey_int(uint8_t *buf, uint8_t *max,
	const char *restrict name, enum nbt_tagid id, uint64_t val);

ssize_t nbt_addkey_float(uint8_t *buf, uint8_t *max,
	const char *restrict name, enum nbt_tagid id, double val);

ssize_t nbt_addkey_arr(uint8_t *buf, uint8_t *max,
	const char *restrict name, enum nbt_tagid id, int32_t len,
	const void *restrict dat);

ssize_t nbt_addkey_str(uint8_t *buf, uint8_t *max,
	const char *restrict name, const char *restrict str);

ssize_t nbt_addkey_list(uint8_t *buf, uint8_t *max,
	const char *restrict name, enum nbt_tagid id, int32_t len);

ssize_t nbt_addkey_compound(uint8_t *buf, uint8_t *max,
	const char *restrict name);

int nbt_tagnamecmp(const uint8_t *tag, const char *str);

char *nbt_popnode(char *path);

#endif /* LIBMCX_NBT_H */
