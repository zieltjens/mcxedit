/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_ERROR_H
#define LIBMCX_ERROR_H 1
#include <libmcx/stddef.h>

/*
 * Error values are defined sequentially from 4096 (0x1000) to not collide
 * with any pre-existing error numbers.
 * Linux in particular reserves the last page (-0x1000) for error values, using
 * values lower (or when defining, higher) should be safe.
 */

#define EMCXTAG  0x1000 /* Invalid binary tag */
#define EMCXITER 0x1001 /* Too many iterations */

#ifdef __cplusplus
extern "C" {
#endif

/* Returns a pointer to the error string,
 * or NULL if the error code was invalid. */
const char *mcx_strerror(int code) MCX_CONST MCX_COLD;

#ifdef __cplusplus
}
#endif

#endif /* LIBMCX_ERROR_H */
