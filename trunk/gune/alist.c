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
 * \file alist.c
 * Association list implementation
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <gune/error.h>
#include <gune/alist.h>

/** Association list entry (stored in the linked list) */
typedef struct alist_entry {
	gendata key;
	gendata value;
} alist_entry_t, * alist_entry;

alist_t * const ERROR_ALIST = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Create a new, empty, association list.
 *
 * \param eq     The equals predicate for two keys.
 *
 * \return       A new empty alist object, or ERROR_ALIST if out of memory.
 */
alist
alist_create(eq_func eq)
{
	alist_t *al;

	assert(eq != NULL);

	if ((al = malloc(sizeof(alist_t))) == NULL)
		return (alist)ERROR_ALIST;

	al->eq = eq;
	al->list = sll_create();

	return (alist)al;
}


/**
 * Free all memory allocated for an association list.  The data within the
 * list is freed by calling a user-supplied free function.
 * If the same data is included multiple times in the list, the free function
 * gets called that many times. <- XXX This is impossible, currently
 *
 * \param al          The association list to destroy.
 * \param key_free    The function which is used to free the key data, or
 *			NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			NULL if no action should be taken on the value data.
 */
void
alist_destroy(alist al, free_func key_free, free_func value_free)
{
	alist_entry e;

	assert(al != ERROR_ALIST);
	assert(al != NULL);

	/*
	 * XXX: What we would really like to do is just call
	 * sll_destroy(*p, f);
	 * Problem is we can't pass a function which knows
	 * about the {key, value}_free functions.
	 */
	while (!sll_empty(al->list)) {
		e = sll_get_data(al->list).ptr;
		if (key_free != NULL)
			key_free(e->key.ptr);
		if (value_free != NULL)
			value_free(e->value.ptr);
		free(e);
		al->list = sll_remove_head(al->list);
	}

	free(al);
}


/**
 * Add a data element to the association list with the given key or replace
 * an existing element with the same key.
 *
 * \param al	      The association list to insert the data in.
 * \param key	      The key of the data.
 * \param value	      The data to insert.
 * \param free_value  The function used to free the old value's data if it
 *		       needs to be replaced, or NULL if the data does not
 *		       need to be freed.
 *
 * \return      The original alist, or ERROR_ALIST if the data could not be
 *               inserted.  Original alist is still valid in case of error.
 */
alist
alist_insert(alist al, gendata key, gendata value, free_func free_value)
{
	sll new_head;
	alist_entry e;
	gendata e_data;
	sll l;

	assert(al != ERROR_ALIST);
	assert(al != NULL);

	/*
	 * We'll have to check if there's already a value with the same key.
	 * If there is, overwrite that value.
	 */
	while (!sll_empty(l = al->list)) {
		e = sll_get_data(l).ptr;
		if (al->eq(key, e->key)) {
			/* Free old data */
			if (free_value != NULL)
				free_value(sll_get_data(l).ptr);
			e->value = value;
			e_data.ptr = e;
			sll_set_data(l, e_data);
			return al;
		}
		l = sll_next(l);
	}

	/* If we got here, the key does not occur in the table yet */
	if ((e = malloc(sizeof(alist_entry_t))) == NULL)
		return ERROR_ALIST;

	e->key = key;
	e->value = value;
	e_data.ptr = e;

	new_head = sll_prepend_head(al->list, e_data);
	if (new_head == ERROR_SLL) {
		free(e);
		return ERROR_ALIST;
	}

	al->list = new_head;
	return al;
}


/**
 * Lookup an element in the association list.
 *
 * \param al    The association list which contains the element.
 * \param key   The key to the element.
 * \param data  A pointer to the location where the element is stored, if
 *               it was found.
 *
 * \return      0 if the element could not be found, nonzero if it could.
 */
int
alist_lookup(alist al, gendata key, gendata *data)
{
	sll l;
	alist_entry e;

	assert(al != ERROR_ALIST);
	assert(al != NULL);
	assert(al->eq != NULL);

	l = al->list;

	while (!sll_empty(l)) {
		e = sll_get_data(l).ptr;
		if (al->eq(key, e->key)) {
			*data = e->value;
			return 1;
		}
		l = sll_next(l);
	}

	return 0;
}


/**
 * Delete an element from the association list.
 *
 * \param al          The association list which contains the element to delete.
 * \param key         The key to the element to delete.
 * \param key_free    The function which is used to free the key data, or
 *			NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			NULL if no action should be taken on the value data.
 *
 * \return     0 if the element could not be found, nonzero if it was deleted.
 */
int
alist_delete(alist al, gendata key, free_func key_free, free_func value_free)
{
	sll l, n;
	alist_entry e;

	assert(al != ERROR_ALIST);
	assert(al != NULL);
	assert(al->eq != NULL);

	l = al->list;

	if (sll_empty(l))
		return 0;

	/*
	 * The first entry is a special case.  Doubly linked lists might
	 *  be easier, but it's not convenience we are after in here.
	 */
	e = sll_get_data(l).ptr;
	if (al->eq(key, e->key)) {
		if (key_free != NULL)
			key_free(e->key.ptr);
		if (value_free != NULL)
			value_free(e->value.ptr);
		sll_remove_head(l);
		return 1;
	}
	n = sll_next(l);

	while (!sll_empty(n)) {
		e = sll_get_data(n).ptr;
		if (al->eq(key, e->key)) {
			sll_remove_next(l);
			return 1;
		}
		l = sll_next(l);
		n = sll_next(l);
	}

	return 0;
}
