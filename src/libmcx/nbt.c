/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#include <libmcx/nbt.h>

#include "endian.h"
#include <errno.h>
#include <libmcx/common.h>
#include <libmcx/error.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#define PRIMITIVES 7

static const u8 nbt_primitive_size[] = {
	[NBT_END]      = 0,
	[NBT_S8]       = 1,
	[NBT_S16]      = 2,
	[NBT_S32]      = 4,
	[NBT_S64]      = 8,
	[NBT_F32]      = 4,
	[NBT_F64]      = 8,

	/* Non-primitive items instead contain
	 * the size of the primitive type they store */
	[NBT_ARR_S8]   = 1,
	[NBT_STR]      = 1,
	[NBT_LIST]     = -1,
	[NBT_COMPOUND] = -1,
	[NBT_ARR_S32]  = 4,
	[NBT_ARR_S64]  = 8,
};

/* If (for whatever reason) cache needs to be expanded in the future,
 * here's my suggested method for doing so:
 *   Instead of the cache being 512 entries, we'll use a dynamic array of which
 *   each item will contain the cache entry. Due to padding, this'll be 8 bytes.
 *   We can use those 3 extra bytes, though I'd start with 2 as a count.
 *   This count will increment for compounds, "depth" is only ever decremented
 *   if this count is 0. (so, a list) */
ssize_t nbt_taglen(const u8 *restrict tag, size_t maxlen, int root,
	struct nbt_cache *restrict cache)
{
	if (0 > root || root >= NBT_NEST_MAX)
		return -EINVAL;
	if ((ssize_t)maxlen < 0)
		return -EINVAL;

	/* This ensures we're working with a clean cache.
	 * (in case it's corrupt due to an error) */
	cache->lens[root] = 0;
	cache->tags[root] = 0;

	int depth = root;
	enum nbt_tagid id;
	const u8 *head = tag;
	const u8 *max  = tag + maxlen;
	do {
		if (!cache->tags[depth]) {
			if (head+1 > max)
				return -EFAULT;
			id = *head++;

			if (id == NBT_END) {
				/* If we were called on an end tag,
				 * we should stop here. */
				if (unlikely(root == depth)) break;
				goto depth_decrease;
			}

			/* Skip the tag name. */
			if (head+2 > max)
				return -EFAULT;
			head += unaligned_read_be16(head) + 2;

			if (id < PRIMITIVES) {
				head += nbt_primitive_size[id];
				continue;
			}
		} else {
			/* Lists are a bitch to parse. */
			if (!cache->lens[depth])
				goto depth_decrease;
			cache->lens[depth]--;
			id = cache->tags[depth];
		}

		s32 n;
		switch (id) {
		case NBT_STR:
			if (head+2 > max)
				return -EFAULT;

			head += unaligned_read_be16(head) + 2;
			continue;
		case NBT_LIST:
			if (head+5 > max)
				return -EFAULT;

			id = *head++;
			n = unaligned_read_be32(head);
			if (n < 0) return -EOVERFLOW;
			head += 4;

			if (n == 0) continue;

			/* NOTE: TAG_END is allowed, but has a size of 0. */
			if (id < PRIMITIVES) {
				/* TODO: Test how Minecraft treats depth.*/
				/* WARN: Because depth never increases, it isn't
				 * checked.  This may result in the data being
				 * treated as "fine", whilst the Minecraft
				 * code doesn't treat it as such.
				 * I don't know what the Minecraft code does,
				 * nor do I see this as too big of an issue. */
				head += nbt_primitive_size[id] * n;
				continue;
			}
			goto depth_increase;
		case NBT_COMPOUND:
			n  = 0;
			id = 0;
			goto depth_increase;
		case NBT_ARR_S8:
		case NBT_ARR_S32:
		case NBT_ARR_S64:
			if (head+4 > max)
				return -EFAULT;

			n = unaligned_read_be32(head);
			if (n < 0) return -EOVERFLOW;
			head += nbt_primitive_size[id] * n + 4;
			continue;
		}
		return -EMCXTAG;
depth_increase:
		depth++;
		if (depth >= NBT_NEST_MAX)
			return -EMCXITER;
		cache->lens[depth] = n;
		cache->tags[depth] = id;
		continue;
depth_decrease:
		depth--;
		continue;
	} while (depth != root);
	return head - tag;
}

/* Copies "str" to "buf+2" without NUL-terminator.
 * If the bytes copied exceeds 0xFFFF, -EINVAL is returned.
 * The bytes copied is written to the first two bytes of buf.
 * Returns the offset in bytes from buf at which the data ended. */
static ssize_t nbt_copy_str(u8 *restrict buf, const u8 *max,
	const char *str)
{
	u8 *head = buf+2;
	size_t n = 0;
	while (*str) {
		if (head >= max)
			return -EFAULT;
		if (n >= 0xFFFF)
			return -EOVERFLOW;
		*head++ = *str++;
		n++;
	}
	return n + unaligned_write_be16(buf, n);
}

static ssize_t nbt_add_header(u8 *restrict buf, const u8 *max,
                            const char *name, enum nbt_tagid id)
{
	u8 *head = buf;
	if (name) {
		if (buf == max)
			return -EFAULT;
		*head++ = id;
		ssize_t n = nbt_copy_str(head, max, name);
		if (n < 0) return n;
		head += n;
	}
	return head - buf;
}

ssize_t nbt_addkey_end(u8 *restrict buf, const u8 *max)
{
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	if (max == buf)
		return -EFAULT;
	*buf = NBT_END;
	return 1;
}

static ssize_t nbt_addkey_int(u8 *restrict buf, const u8 *max,
		const char *name, u64 val, enum nbt_tagid id)
{
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	if ((unsigned)id >= PRIMITIVES)
		return -EINVAL;
	u8 *head = buf;

	ssize_t n = nbt_add_header(buf, max, name, id);
	if (n < 0) return n;
	head += n;

	if (head + nbt_primitive_size[id] > max)
		return -EFAULT;

	switch (id) {
	case NBT_S8:
		*head++ = val;
		break;
	case NBT_S16:
		head += unaligned_write_be16(head, val);
		break;
	case NBT_S32:
		head += unaligned_write_be32(head, val);
		break;
	case NBT_S64:
		head += unaligned_write_be64(head, val);
		break;
	default:
		return -EINVAL;
	}
	return head - buf;
}
ssize_t nbt_addkey_s8(u8 *restrict buf, const u8 *max,
		const char *name, s8 val)
{
	return nbt_addkey_int(buf, max, name, val, NBT_S8);
}
ssize_t nbt_addkey_s16(u8 *restrict buf, const u8 *max,
		const char *name, s16 val)
{
	return nbt_addkey_int(buf, max, name, val, NBT_S16);
}
ssize_t nbt_addkey_s32(u8 *restrict buf, const u8 *max,
		const char *name, s32 val)
{
	return nbt_addkey_int(buf, max, name, val, NBT_S32);
}
ssize_t nbt_addkey_s64(u8 *restrict buf, const u8 *max,
		const char *name, s64 val)
{
	return nbt_addkey_int(buf, max, name, val, NBT_S64);
}

union nbt_float {
	u32 as_u32;
	u64 as_u64;
	f32 as_f32;
	f64 as_f64;
};
static ssize_t nbt_addkey_float(u8 *restrict buf, const u8 *max,
		const char *name, union nbt_float val, enum nbt_tagid id)
{
	if ((unsigned)id >= PRIMITIVES)
		return -EINVAL;
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	u8 *head = buf;

	ssize_t n = nbt_add_header(buf, max, name, id);
	if (n < 0) return n;
	head += n;

	if (head + nbt_primitive_size[id] > max)
		return -EFAULT;

	switch (id) {
	case NBT_F32:
		head += unaligned_write_be32(head, val.as_u32);
		break;
	case NBT_F64:
		head += unaligned_write_be64(head, val.as_u64);
		break;
	default:
		return -EINVAL;
	}
	return head - buf;
}
ssize_t nbt_addkey_f32(u8 *restrict buf, const u8 *max,
		const char *name, float val)
{
	return nbt_addkey_float(buf, max, name,
			(union nbt_float){.as_f32 = val}, NBT_F32);
}
ssize_t nbt_addkey_f64(u8 *restrict buf, const u8 *max,
		const char *name, double val)
{
	return nbt_addkey_float(buf, max, name,
			(union nbt_float){.as_f64 = val}, NBT_F64);
}

static ssize_t nbt_addkey_arr(u8 *restrict buf, const u8 *max,
		const char *name, const void *dat, s32 len, enum nbt_tagid id)
{
	if (id != NBT_ARR_S8 && id != NBT_ARR_S32 && id != NBT_ARR_S64)
		return -EINVAL;
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	if (len < 0)
		return -EINVAL;
	u8 *head = buf;

	ssize_t n = nbt_add_header(buf, max, name, id);
	if (n < 0) return n;
	head += n;

	int membsize = nbt_primitive_size[id];
	size_t size = len * membsize;
	if (head+4+size > max)
		return -EFAULT;
	unaligned_write_be32(head, len);
	head += 4;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	memcpy(head, dat, size);
	head += size;
#else
	switch (membsize) {
	case 1:
		memcpy(head, dat, size);
		head += size;
		break;
	case 4:
		while (len--) {
			unaligned_write_be32(head, *( u32*)dat);
			head += 4;
			dat  += 4;
		}
		break;
	case 8:
		while (len--) {
			unaligned_write_be64(head, *( u64*)dat);
			head += 8;
			dat  += 8;
		}
		break;
	}
#endif
	return head - buf;
}
ssize_t nbt_addkey_arr_s8(u8 *restrict buf, const u8 *max,
		const char *name, const s8 *dat, s32 len)
{
	return nbt_addkey_arr(buf, max, name, dat, len, NBT_ARR_S8);
}
ssize_t nbt_addkey_arr_s32(u8 *restrict buf, const u8 *max,
		const char *name, const s32 *dat, s32 len)
{
	return nbt_addkey_arr(buf, max, name, dat, len, NBT_ARR_S32);
}
ssize_t nbt_addkey_arr_s64(u8 *restrict buf, const u8 *max,
		const char *name, const s64 *dat, s32 len)
{
	return nbt_addkey_arr(buf, max, name, dat, len, NBT_ARR_S64);
}

ssize_t nbt_addkey_str(u8 *restrict buf, const u8 *max,
	const char *name, const char *str)
{
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	u8 *head = buf;
	ssize_t n;

	n = nbt_add_header(buf, max, name, NBT_STR);
	if (n < 0) return n;
	head += n;

	n = nbt_copy_str(head, max, str);
	if (n < 0) return n;
	head += n;
	return head - buf;
}

ssize_t nbt_addkey_list(u8 *restrict buf, const u8 *max,
	const char *name, enum nbt_tagid id, s32 len)
{
	if (0 > id || id >= sizeof(nbt_primitive_size))
		return -EINVAL;
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	if (len < 0)
		return -EINVAL;
	u8 *head = buf;

	ssize_t n = nbt_add_header(buf, max, name, NBT_LIST);
	if (n < 0) return n;
	head += n;

	if (head+5 > max)
		return -EFAULT;
	*head++ = id;
	unaligned_write_be32(head, len);
	head += 4;
	return head - buf;
}

ssize_t nbt_addkey_compound(u8 *restrict buf, const u8 *max, const char *name)
{
	if ((ssize_t)(max - buf) < 0)
		return -EINVAL;
	u8 *head = buf;

	ssize_t n = nbt_add_header(buf, max, name, NBT_COMPOUND);
	if (n < 0) return n;
	head += n;
	return head - buf;
}

int nbt_tagnamecmp(const u8 *tag, const char *str)
{
	u16 n = unaligned_read_be16(++tag);
	tag += 2;
	int v;

	if (!n) return !!*str;
	while (n-- && *str && !(v = *str++ - *tag++));
	return v;
}

char *nbt_popnode(char *path)
{
	char c;
	do c = *path++;
	while (c && c != '.');
	path[-1] = '\0';
	return c ? path : NULL;
}
