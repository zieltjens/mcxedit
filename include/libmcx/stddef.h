/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_STDDEF_H
#define LIBMCX_STDDEF_H 1
#include <assert.h>
#include <libmcx/types.h>
#include <stdbool.h>
#include <stdnoreturn.h>

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
#endif /* MCXEDIT_SOURCE */

#endif /* LIBMCX_ATRB_H */
