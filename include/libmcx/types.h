/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_TYPES_H
#define LIBMCX_TYPES_H 1
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

struct mcx;

#ifdef MCXEDIT_SOURCE
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
#endif /* LIBMCX_TYPES_H */
