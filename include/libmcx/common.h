/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_STDDEF_H
#define LIBMCX_STDDEF_H 1
#include <assert.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

struct mcx;

/* A GCC-compatable compiler is mandated for these definitions.
 */
#ifdef __GNUC__
#define MCX_CONST        __attribute__((__const__))
#define MCX_PRINTF(x, y) __attribute__((__format__(printf, x, y)))
#define MCX_HOT          __attribute__((__hot__))
#define MCX_COLD         __attribute__((__cold__))
#define MCX_MALLOC       __attribute__((__malloc__))
#define MCX_PURE         __attribute__((__pure__))
#else
#define MCX_CONST
#define MCX_PRINTF(x,y)
#define MCX_HOT
#define MCX_COLD
#define MCX_MALLOC
#define MCX_PURE
#endif /* __GNUC__ */

/* These definitions will only be included for MCX source code. */
#ifdef MCXEDIT_SOURCE
#define __maybe_unused __attribute__((__unused__))
#define __used         __attribute__((__used__))

#define fallthrough   __attribute__((__fallthrough__))
#define always_inline __attribute__((__always_inline__)) inline

#define lengthof(symbol) (sizeof(symbol) / sizeof(*symbol))

#define unlikely(x) __builtin_expect(!!(x), 0)
#define   likely(x) __builtin_expect(!!(x), 1)

#define min(_x, _y) __extension__ ({ \
	__auto_type x = _x; \
	__auto_type y = _y; \
	x < y ? x : y; })
#define max(_x, _y) __extension__ ({ \
	__auto_type x = _x; \
	__auto_type y = _y; \
	x > y ? x : y; })

#ifdef __CHECKER__
#define __bitwise __attribute__((bitwise))
#define __force   __attribute__((force))
#else
#define __bitwise
#define __force
#endif /* __CHECKER__ */
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef signed char        schar;
typedef long long          llong;
typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef unsigned long long ullong;

typedef u16 __bitwise be16;
typedef u32 __bitwise be32;
typedef u64 __bitwise be64;
typedef u16 __bitwise le16;
typedef u32 __bitwise le32;
typedef u64 __bitwise le64;

#if defined(__STDC_IEC_559__) || defined(__STDC_IEC_60559_BFP__)
typedef float  f32;
typedef double f64;
#endif
#endif /* MCXEDIT_SOURCE */

#endif /* LIBMCX_ATRB_H */
