/*
 * $Id$
 *
 * Copyright (c) 2004 Peter Bex and Vincent Driessen
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
 * \file ht.c
 * Hash table implementation
 */
#include <assert.h>
#include <stdlib.h>

#include <gune/error.h>
#include <gune/ht.h>

ht_t * const ERROR_HT = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new, empty, hash table.
 *
 * \range   The range of the key function.
 *
 * \return  A new empty hash table object, or ERROR_HT if out of memory.
 */
ht
ht_create(unsigned int range)
{
	ht_t *t;
	unsigned int i, j;

	if ((t = malloc(sizeof(ht_t))) == NULL)
		return (ht)ERROR_HT;

	t->range = range;

	/*
	 * We could use a Gune array, but we're not resizing so we would just
	 * be introducing unnecessary overhead.
	 */
	if ((t->table = (sll *)malloc(range * sizeof(sll_t))) == NULL) {
		free(t);
		return (ht)ERROR_HT;
	}

	/*
	 * Slightly inefficient, we could also use memset, but that's
	 * making assumptions about the value of NULL.  Plus, this code should
	 * not be the bottleneck of any sane program.
	 */
	for (i = 0; i < range; ++i) {
		if ((*(t->table + i) = sll_create()) == ERROR_SLL) {
			/* Creation went wrong halfway?  Destroy all previous */
			for (j = 0; j < i; ++j)
				sll_free(*(t->table + j));

			free(t);
			return (ht)ERROR_HT;
		}
	}

	return (ht)t;
}


/**
 * Free all memory allocated for a hash table.  The data within the table is
 * freed by calling a user-supplied free function.
 * If the same data is included multiple times in the table, the free function
 * gets called that many times.
 *
 * \param t  The hash table to destroy.
 * \param f  The function which is used to free the data.
 */
void
ht_destroy(ht t, free_func f)
{
	unsigned int i;

	assert(t != ERROR_HT);
	assert(t != NULL);

	for (i = 0; i < t->range; ++i)
		sll_destroy(*(t->table + i), f);

	free(t);
}


/**
 * Free all memory allocated for a hash table. The data stored within the
 * table is NOT freed.
 *
 * \param t  The hash table to destroy.
 */
void
ht_free(ht t)
{
	unsigned int i;

	assert(t != ERROR_HT);
	assert(t != NULL);

	for (i = 0; i < t->range; ++i)
		sll_free(*(t->table + i));

	free(t);
}
