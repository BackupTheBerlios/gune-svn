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
 * \param range  The range of the key function (0 <= x < range).
 * \param hash   The hashing function to use on keys.
 *
 * \return       A new empty hash table object, or ERROR_HT if out of memory.
 *
 * \sa ht_destroy
 */
ht
ht_create(unsigned int range, hash_func hash)
{
	ht_t *t;
	unsigned int i, j;

	assert(hash != NULL);

	if ((t = malloc(sizeof(ht_t))) == NULL)
		return (ht)ERROR_HT;

	t->range = range;
	t->hash = hash;

	/*
	 * We could use a Gune array, but we're not resizing so we would just
	 * be introducing unnecessary overhead.
	 */
	if ((t->buckets = (alist *)malloc(range * sizeof(alist))) == NULL) {
		free(t);
		return (ht)ERROR_HT;
	}

	/*
	 * Slightly inefficient, we could also use memset, but that's
	 * making assumptions about the value of NULL.  Plus, this code should
	 * not be the bottleneck of any sane program.
	 */
	for (i = 0; i < range; ++i) {
		if ((*(t->buckets + i) = alist_create()) == ERROR_ALIST) {
			/* Creation went wrong halfway?  Destroy all previous */
			for (j = 0; j < i; ++j)
				alist_destroy(*(t->buckets + j), NULL, NULL);

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
 * gets called that many times. <- XXX This is impossible, currently, unless
 * really strange things have been going on! (like, say, a random hashing
 * function)
 *
 * \param t           The hash table to destroy.
 * \param key_free    The function which is used to free the key data, or
 *			NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			NULL if no action should be taken on the value data.
 *
 * \sa ht_create
 */
void
ht_destroy(ht t, free_func key_free, free_func value_free)
{
	alist *al;

	assert(t != ERROR_HT);
	assert(t != NULL);

	for (al = t->buckets; al < (t->buckets + t->range); ++al)
		alist_destroy(*al, key_free, value_free);

	free(t);
}


/**
 * Add a data element to the hash table with the given key or replace an
 *  existing element with the same key.
 *
 * \param t           The hash table to insert the data in.
 * \param key         The key of the data.
 * \param value       The data to insert.
 * \param eq          The equals predicate for two keys.
 * \param free_value  The function used to free the old value's data if it
 *		       needs to be replaced, or NULL if the data does not
 *		       need to be freed.
 *
 * \return      The original hash table, or ERROR_HT if the data could not be
 *               inserted.  Original hash table is still valid in case of error.
 *
 * \sa ht_delete
 */
ht
ht_insert(ht t, gendata key, gendata value, eq_func eq, free_func free_value)
{
	unsigned int bucketnr;
	alist al;

	assert(t != ERROR_HT);
	assert(t != NULL);
	assert(t->hash != NULL);
	assert(eq != NULL);

	bucketnr = t->hash(key, t->range);

#ifdef BOUNDS_CHECKING
	if (bucketnr >= t->range)
		log_entry(WARN_ERROR, "Gune: ht_insert: Key hash (%u) "
			  "out of range", bucketnr);
#endif

	al = *(t->buckets + bucketnr);
	al = alist_insert(al, key, value, eq, free_value);

	if (al == ERROR_ALIST)
		t = ERROR_HT;
	else
		/* Strictly not needed */
		*(t->buckets + bucketnr) = al;

	return t;
}


/**
 * Lookup an element in the hash table.
 *
 * \param t     The hashtable which contains the element.
 * \param key   The key to the element.
 * \param eq    The equals predicate for two keys.
 * \param data  A pointer to the location where the element is stored, if
 *               it was found.
 *
 * \return      0 if the element could not be found, nonzero if it could.
 */
int
ht_lookup(ht t, gendata key, eq_func eq, gendata *data)
{
	unsigned int bucketnr;
	alist al;

	assert(t != ERROR_HT);
	assert(t != NULL);
	assert(t->hash != NULL);
	assert(eq != NULL);

	bucketnr = t->hash(key, t->range);

#ifdef BOUNDS_CHECKING
	if (bucketnr >= t->range)
		log_entry(WARN_ERROR, "Gune: ht_lookup: Key hash (%u) "
			  "out of range", bucketnr);
#endif

	al = *(t->buckets + bucketnr);
	return alist_lookup(al, key, eq, data);
}


/**
 * Delete an element from the hash table.
 *
 * \param t           The hashtable which contains the element to delete.
 * \param key         The key to the element to delete.
 * \param eq          The equals predicate for two keys.
 * \param key_free    The function which is used to free the key data, or
 *			NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			NULL if no action should be taken on the value data.
 *
 * \return      0 if the element could not be found, nonzero if it was deleted.
 *
 * \sa ht_insert
 */
int
ht_delete(ht t, gendata key, eq_func eq, free_func key_free,
	  free_func value_free)
{
	unsigned int bucketnr;
	alist al;

	assert(t != ERROR_HT);
	assert(t != NULL);
	assert(t->hash != NULL);
	assert(eq != NULL);

	bucketnr = t->hash(key, t->range);

#ifdef BOUNDS_CHECKING
	if (bucketnr >= t->range)
		log_entry(WARN_ERROR, "Gune: ht_lookup: Key hash (%u) out "
			  "of range", bucketnr);
#endif

	al = *(t->buckets + bucketnr);
	return alist_delete(al, key, eq, key_free, value_free);
}
