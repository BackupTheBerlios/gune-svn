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

/** Hash table entry (stored in the linked list) */
typedef struct ht_entry_t {
	gendata key;
	gendata value;
} ht_entry_t, * ht_entry;

ht_t * const ERROR_HT = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new, empty, hash table.
 *
 * \param range  The range of the key function (0 <= x < range).
 * \param hash   The hashing function to use on keys.
 * \param eq     The equals predicate for two keys.
 *
 * \return       A new empty hash table object, or ERROR_HT if out of memory.
 */
ht
ht_create(unsigned int range, hash_func hash, eq_func eq)
{
	ht_t *t;
	unsigned int i, j;

	assert(hash != NULL);
	assert(eq != NULL);

	if ((t = malloc(sizeof(ht_t))) == NULL)
		return (ht)ERROR_HT;

	t->range = range;
	t->eq = eq;
	t->hash = hash;

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
				sll_destroy(*(t->table + j), NULL);

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
 */
void
ht_destroy(ht t, free_func key_free, free_func value_free)
{
	sll *p;
	ht_entry e;

	assert(t != ERROR_HT);
	assert(t != NULL);

	for (p = t->table; p < (t->table + t->range); ++p) {
		if (p != NULL) {
			/*
			 * XXX: What we would really like to do is just call
			 * sll_destroy(*p, f);
			 * Problem is we can't pass a function which knows
			 * about the {key, value}_free functions.
			 */
			while (!sll_empty(*p)) {
				e = sll_get_data(*p).ptr;
				if (key_free != NULL)
					key_free(e->key.ptr);
				if (value_free != NULL)
					value_free(e->value.ptr);
				free(e);
				*p = sll_remove_head(*p);
			}
		}
	}

	free(t);
}


/**
 * Add a data element to the hash table with the given key or replace an
 *  existing element with the same key.
 *
 * \param t      The hash table to insert the data in.
 * \param key    The key of the data.
 * \param value  The data to insert.
 *
 * \return      The original hash table, or ERROR_HT if the data could not be
 *               inserted.  Original hash table is still valid in case of error.
 */
ht
ht_insert(ht t, gendata key, gendata value)
{
	sll new_head;
	unsigned int bucket;
	sll ll;
	ht_entry e;
	gendata e_data;

	assert(t != ERROR_HT);
	assert(t != NULL);
	assert(t->hash != NULL);

	bucket = t->hash(key);

#ifdef BOUNDS_CHECKING
	if (bucket >= t->range)
		log_entry(WARN_ERROR, "Gune: ht_insert: Key hash (%u) out of range",
			  bucket);
#endif

	ll = *(t->table + bucket);

	/*
	 * We'll have to check if there's already a value with the same key.
	 * If it is, overwrite that value.
	 */
	while (!sll_empty(ll)) {
		e = sll_get_data(ll).ptr;
		if (t->eq(key, e->key)) {
			e->value = value;
			e_data.ptr = e;
			sll_set_data(ll, e_data);
			return t;
		}
	}

	/* If we got here, the key does not occur in the table yet */
	if ((e = malloc(sizeof(ht_entry_t))) == NULL)
		return ERROR_HT;

	e->key = key;
	e->value = value;
	e_data.ptr = e;

	new_head = sll_prepend_head(*(t->table + bucket), e_data);
	if (new_head == ERROR_SLL) {
		free(e);
		return ERROR_HT;
	}

	*(t->table + bucket) = new_head;
	return t;
}


/**
 * Lookup an element in the hash table.
 *
 * \param t     The hashtable which contains the element.
 * \param key   The key to the element.
 * \param data  A pointer to the location where the element is stored, if
 *               it was found.
 *
 * \return      0 if the element could not be found, nonzero if it could.
 */
int
ht_lookup(ht t, gendata key, gendata *data)
{
	sll l;
	ht_entry e;
	unsigned int bucket;

	assert(t != ERROR_HT);
	assert(t != NULL);
	assert(t->eq != NULL);
	assert(t->hash != NULL);

	bucket = t->hash(key);

#ifdef BOUNDS_CHECKING
	if (bucket >= t->range)
		log_entry(WARN_ERROR, "Gune: ht_lookup: Key hash (%u) out of range",
			  bucket);
#endif

	l = *(t->table + bucket);
	while (!sll_empty(l)) {
		e = sll_get_data(l).ptr;
		if (t->eq(key, e->key)) {
			*data = e->value;
			return 1;
		}
		l = sll_next(l);
	}

	return 0;
}
