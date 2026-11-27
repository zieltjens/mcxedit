/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2026 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_SNBT_H
#define LIBMCX_SNBT_H 1
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

char *snbt_popnode(char *path);

#ifdef __cplusplus
}
#endif
#endif /* LIBMCX_SNBT_H */
