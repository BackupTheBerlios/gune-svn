/*
 * $Id$
 *
 * Copyright (c) 2003-2004 Peter Bex and Vincent Driessen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of Peter Bex or Vincent Driessen nor the names of any
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PETER BEX AND VINCENT DRIESSEN AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \brief Custom types.
 *
 * \file types.h
 * Certain custom (re-usable) types.
 */
#ifndef GUNE_TYPES_H
#define GUNE_TYPES_H

/**
 * \brief Generic data type to put in lists, stacks etc.
 *
 * This data type exists so we can make our functions more generic
 * without putting the burden of having to malloc() every little integer
 * or character on the programmer.
 * Compare:
 * \code
 * int x = 10;
 * gendata d;
 *
 * d.num = x;
 *
 * stack_push(st, d);
 * \endcode
 * to the more error-prone and annoying:
 * \code
 * int *x;
 *
 * if ((x = malloc(sizeof(int))) == NULL)
 *	log_entry(WARN_ERROR, "Out of memory for a lousy int!");
 * *x = 10;
 * stack_push(st, x);
 * \endcode
 * <em>Don't forget to free \p x</em> in the last example.
 * Also, the last example needs one extra pointer for each \c int you store.
 */
/*
 * Note: We can't use the names of C (char, int), so we have to be a little
 * creative here.
 */
typedef union gendata {
	int		num;			/**< Number */
	unsigned int	posnum;			/**< Positive number */
	char		sym;			/**< Symbol */
	void *		ptr;			/**< Pointer */
} gendata;

/**
 * \brief Function type to be supplied to the _destroy functions to free data.
 */
typedef void (*free_func) (void *);

/**
 * \brief Equals predicate function type for comparing keys.
 */
typedef int (* eq_func) (gendata, gendata);

/**
 * \brief Function for traveling through lists that have (key, value) pairs.
 */
typedef void (* assoc_func) (gendata *, gendata *, gendata);

#endif /* GUNE_TYPES_H */
