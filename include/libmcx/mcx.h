/* SPDX-License-Identifier: BSD-2-Clause
 * SPDX-FileCopyrightText: ©2025 Quinn Zieltjens <zieltjens@pigeonware.org>
 */
#ifndef LIBMCX_MCX_H
#define LIBMCX_MCX_H 1

#include <libmcx/stddef.h>
#include <libmcx/types.h>
#include <stdio.h>

#define MCX_SECTOR_SIZE  0x1000
#define MCX_TABLE_SIZE   0x1000
#define MCX_TABLES_SIZE  0x2000
#define MCX_TABLE_ITEMS  0x400
#define MCX_TABLES_ITEMS 0x800

#ifdef __cplusplus
extern "C" {
#endif

/* Repairs faults in the file format,
 * outputting warnings describing the repaired fault & affected chunk.
 * Returns the size of the file, which may be larger than,
 * less than, or equal to the original size. */
off_t mcx_repair(struct mcx *mcx, off_t size);

/* Prunes the unused sections in a .mcX file.
 * It is assumed that the table is formatted correctly and
 * that all data is accessible.
 * Returns the new file size.*/
off_t mcx_defrag(struct mcx *mcx, off_t size);

/* Computes the size in bytes of the entire .mcX file,
 * according to the table.
 * This is done by summing the chunk sector offset and length
 * and taking the highest value.
 *
 * This value may not be reflective of the actual value due to unused
 * sectors at the end, or that overlapping values are present.
 * If this value is higher than the actual file size,
 * then corruption has taken place. */
off_t mcx_calcsize(const struct mcx *mcx) MCX_PURE;

/* Computes the minimum size in bytes required for the entire .mcX file,
 * according to the table.
 * This is done through summing together the length sectors of each chunk,
 * and adding 2 for the table.
 *
 * This value is a very optimistic value, and unlikely to be reflective
 * of the actual size due to the presence of unused sectors.
 * In the edge case of overlapping sectors, this value may be larger
 * than the actual file size. */
off_t mcx_sumsize(const struct mcx *mcx) MCX_PURE;

#ifdef __cplusplus
}
#endif

#endif /* LIBMCX_MCX_H */
