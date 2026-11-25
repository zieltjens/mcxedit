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

/* Function is unlikely to be executed, and shall be compiled for size. */
#define MCX_COLD __attribute__((__cold__))

/* The return value is not affected by changes to the observable
 * state of the program, and has no observable effect on such state.
 * NOTE: The function mustn't examine data pointed at by pointers. */
#define MCX_CONST __attribute__((__const__))

/* Specifies that the function takes `printf`, `scanf`, `strftime`,
 * or `strfmon` style arguments, which must be type-checked against
 * a format string. */
#define MCX_PRINTF(x, y) __attribute__((__format__(printf, x, y)))

/* Function is in a particular hot section of the code, and shall be aggressively
 * optimised for performance. Furthermore, it will be placed
 * inside a special subsection in .text, which improves locality. */
#define MCX_HOT __attribute__((__hot__))

/* Indicates that the function is `malloc`-like, i.e. the returned
 * pointer may not alias any other pointer valid when the function returns.
 * Furthermore, this pointer is assumed to not be `NULL` in most cases.
 * Specifying an argument defines the deallocator that must be used to
 * deallocate the returned pointer. Which may be the allocator function.
 * Where the second argument specifies to which parameter the pointer
 * should be written, when calling the deallocator.
 * When no arguments have been specified, MALLOC means that the
 * returned pointer is non-aliassing, due to being newly obtained.
 * `realloc` does not meet this requirement.
 * Both versions should be used if both can be satisfied. */
#define MCX_MALLOC        __attribute__((__malloc__))

/* The function may not have an effect on the observable state of the programme.
 * Unlike `const`, a pure function may read memory, even if it changes between
 * calls. */
#define MCX_PURE __attribute__((__pure__))
#else
#define MCX_COLD
#define MCX_CONST
#define MCX_PRINTF
#define MCX_HOT
#define MCX_MALLOC
#define MCX_PURE
#endif /* __GNUC__ */

/* These definitions will only be included for MCX source code. */
#ifdef MCXEDIT_SOURCE

#define lengthof(symbol) (sizeof(symbol) / sizeof(*symbol))

/* Marks a function as unused and will prevent GCC from emitting a warning. */
#define __maybe_unused __attribute__((__unused__))

/* Marks a function as used, and will cause the code must be emitted,
 * even if it appears like the function is never referenced. */
#define __used __attribute__((__used__))

/* Explicitly defines that a statement falls through to a following case
 * or user-defined label. */
#if __has_attribute(__fallthrough__)
#define fallthrough __attribute__((__fallthrough__))
#else
#define fallthrough
#endif /* __has_attribute(__fallthrough__) */

#define unlikely(x) __builtin_expect(!!(x), 0)
#define   likely(x) __builtin_expect(!!(x), 1)
#endif /* MCXEDIT_SOURCE */

#endif /* LIBMCX_ATRB_H */
