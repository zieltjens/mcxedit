/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef MCXEDIT_ENDIAN_H
#define MCXEDIT_ENDIAN_H 1

#include <libmcx/common.h>

/* Mostly copied from Linux using:
 * $ grep HAVE_EFFICIENT_UNALIGNED_ACCESS arch/.../Kconfig
 */
#if defined(__i386__) || defined(__x86_64__) ||\
	defined(__aarch64__) || defined(__riscv__)
#define MAY_UNALIGNED_ACCESS 1
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define le16_to_host(x) ((__force u16)(le16)x)
#define le32_to_host(x) ((__force u32)(le32)x)
#define le64_to_host(x) ((__force u64)(le64)x)
#define host_tole16(x) ((__force le16)(u16)x)
#define host_tole32(x) ((__force le32)(u32)x)
#define host_tole64(x) ((__force le64)(u64)x)
#define be16_to_host(x) (__builtin_bswap16((__force u16)(be16)x))
#define be32_to_host(x) (__builtin_bswap32((__force u32)(be32)x))
#define be64_to_host(x) (__builtin_bswap64((__force u64)(be64)x))
#define host_to_be16(x) ((__force be16)__builtin_bswap16(x))
#define host_to_be32(x) ((__force be32)__builtin_bswap32(x))
#define host_to_be64(x) ((__force be64)__builtin_bswap64(x))
#define unaligned_read_be16  unaligned_bswap_read16
#define unaligned_read_be32  unaligned_bswap_read32
#define unaligned_read_be64  unaligned_bswap_read64
#define unaligned_read_le16  unaligned_read16
#define unaligned_read_le32  unaligned_read32
#define unaligned_read_le64  unaligned_read64
#define unaligned_write_be16 unaligned_bswap_write16
#define unaligned_write_be32 unaligned_bswap_write32
#define unaligned_write_be64 unaligned_bswap_write64
#define unaligned_write_le16 unaligned_write16
#define unaligned_write_le32 unaligned_write32
#define unaligned_write_le64 unaligned_write64
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define le16_to_host(x) (__builtin_bswap16((__force u16)(le16)x))
#define le32_to_host(x) (__builtin_bswap32((__force u32)(le32)x))
#define le64_to_host(x) (__builtin_bswap64((__force u64)(le64)x))
#define host_to_le16(x) ((__force le16)__builtin_bswap16(x))
#define host_to_le32(x) ((__force le32)__builtin_bswap32(x))
#define host_to_le64(x) ((__force le64)__builtin_bswap64(x))
#define be16_to_host(x) ((__force u16)(be16)x)
#define be32_to_host(x) ((__force u32)(be32)x)
#define be64_to_host(x) ((__force u64)(be64)x)
#define host_to_be16(x) ((__force be16)(u16)x)
#define host_to_be32(x) ((__force be32)(u32)x)
#define host_to_be64(x) ((__force be64)(u64)x)
#define unaligned_read_be16  unaligned_read16
#define unaligned_read_be32  unaligned_read32
#define unaligned_read_be64  unaligned_read64
#define unaligned_read_le16  unaligned_bswap_read16
#define unaligned_read_le32  unaligned_bswap_read32
#define unaligned_read_le64  unaligned_bswap_read64
#define unaligned_write_be16 unaligned_write16
#define unaligned_write_be32 unaligned_write32
#define unaligned_write_be64 unaligned_write64
#define unaligned_write_le16 unaligned_bswap_write16
#define unaligned_write_le32 unaligned_bswap_write32
#define unaligned_write_le64 unaligned_bswap_write64
#else
#error "Machine architecture unsupported! Expected either big-endian or little-endian."
#endif

/* Reads a 16-bit integer from an unaligned buffer. */
static inline MCX_PURE u16 unaligned_read16(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return *(u16 *)buf;
#else
	return (u16)buf[0] << 000 | (u16)buf[1] << 010;
#endif
}

/* Reads and bswaps a 16-bit integer from an unaligned buffer. */
static inline MCX_PURE u16 unaligned_bswap_read16(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return __builtin_bswap16(*(u16 *)buf);
#else
	return (u16)buf[1] << 000 | (u16)buf[0] << 010;
#endif
}

/* Writes a 16-bit integer to an unaligned buffer. */
static inline size_t unaligned_write16(u8 *buf, u16 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u16 *)buf = v;
#else
	buf[0] = v >> 000; buf[1] = v >> 010;
#endif
	return sizeof(u16);
}

/* Writes and bswaps a 16-bit integer to an unaligned buffer. */
static inline size_t unaligned_bswap_write16(u8 *buf, u16 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u16 *)buf = __builtin_bswap16(v);
#else
	buf[1] = v >> 000; buf[0] = v >> 010;
#endif
	return sizeof(u16);
}

/* Reads a 32-bit integer from an unaligned buffer. */
static inline MCX_PURE u32 unaligned_read32(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return *(u32 *)buf;
#else
	return (u32)buf[0] << 000 | (u32)buf[1] << 010 |
	       (u32)buf[2] << 020 | (u32)buf[3] << 030;
#endif
}

/* Reads and bswaps a 32-bit integer from an unaligned buffer. */
static inline MCX_PURE u32 unaligned_bswap_read32(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return __builtin_bswap32(*(u32 *)buf);
#else
	return (u32)buf[3] << 000 | (u32)buf[2] << 010 |
	       (u32)buf[1] << 020 | (u32)buf[0] << 030;
#endif
}

/* Writes a 32-bit integer to an unaligned buffer. */
static inline size_t unaligned_write32(u8 *buf, u32 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u32 *)buf = v;
#else
	buf[0] = v >> 000; buf[1] = v >> 010;
	buf[2] = v >> 020; buf[3] = v >> 030;
#endif
	return sizeof(u32);
}

/* Writes and bswaps a 32-bit integer to an unaligned buffer. */
static inline size_t unaligned_bswap_write32(u8 *buf, u32 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u32 *)buf = __builtin_bswap32(v);
#else
	buf[3] = v >> 000; buf[2] = v >> 010;
	buf[1] = v >> 020; buf[0] = v >> 030;
#endif
	return sizeof(u32);
}

/* Reads a 64-bit integer from an unaligned buffer. */
static inline MCX_PURE u64 unaligned_read64(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return *(u64 *)buf;
#else
	return (u64)buf[0] << 000 | (u64)buf[1] << 010 |
	       (u64)buf[2] << 020 | (u64)buf[3] << 030 |
	       (u64)buf[4] << 040 | (u64)buf[5] << 050 |
	       (u64)buf[6] << 060 | (u64)buf[7] << 070;
#endif
}

/* Reads and bswaps a 64-bit integer from an unaligned buffer. */
static inline MCX_PURE u64 unaligned_bswap_read64(const u8 *buf)
{
#if defined(MAY_UNALIGNED_ACCESS)
	return __builtin_bswap64(*(u64 *)buf);
#else
	return (u64)buf[7] << 000 | (u64)buf[6] << 010 |
	       (u64)buf[5] << 020 | (u64)buf[4] << 030 |
	       (u64)buf[3] << 040 | (u64)buf[2] << 050 |
	       (u64)buf[1] << 060 | (u64)buf[0] << 070;
#endif
}

/* Writes a 64-bit integer to an unaligned buffer. */
static inline size_t unaligned_write64(u8 *buf, u64 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u64 *)buf = v;
#else
	buf[0] = v >> 000; buf[1] = v >> 010;
	buf[2] = v >> 020; buf[3] = v >> 030;
	buf[4] = v >> 040; buf[5] = v >> 050;
	buf[6] = v >> 060; buf[7] = v >> 070;
#endif
	return sizeof(u64);
}

/* Writes and bswaps a 64-bit integer to an unaligned buffer. */
static inline size_t unaligned_bswap_write64(u8 *buf, u64 v)
{
#if defined(MAY_UNALIGNED_ACCESS)
	*(u64 *)buf = __builtin_bswap64(v);
#else
	buf[7] = v >> 000; buf[6] = v >> 010;
	buf[5] = v >> 020; buf[4] = v >> 030;
	buf[3] = v >> 040; buf[2] = v >> 050;
	buf[1] = v >> 060; buf[0] = v >> 070;
#endif
	return sizeof(u64);
}
#endif /* MCXEDIT_ENDIAN_H */
