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
 * \brief Association lists implementation.
 *
 * \file alist.c
 */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <gune/alist.h>

/** \brief Association list entry (stored in the linked list) */
typedef struct alist_entry {
	gendata key;		/**< Key, used for identification of entries */
	gendata value;		/**< The entry itself */
} alist_entry_t, * alist_entry;

static alist alist_insert_internal(alist, gendata, gendata, eq_func,
				   free_func, int);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * \brief Create a new empty association list.
 *
 * \return  A new empty alist object, or \c NULL in case of an error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa alist_destroy
 */
alist
alist_create(void)
{
	alist_t *al;

	if ((al = malloc(sizeof(alist_t))) != NULL)
		al->list = sll_create();

	return (alist)al;
}


/**
 * \brief Free all memory allocated for an association list.
 *
 * The data within the list is freed by calling the user-supplied
 * function \p value_free.  The key used to identify the entry can also
 * be freed by the user-supplied \p key_free function.
 *
 * \attention
 * If the same data or key is included multiple times in the list, the free
 * functions get called that many times on the data or key.
 *
 * \param al          The association list to destroy.
 * \param key_free    The function which is used to free the key data, or
 *			\c NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			\c NULL if no action should be taken on the value data.
 *
 * \sa alist_create
 */
void
alist_destroy(alist al, free_func key_free, free_func value_free)
{
	alist_entry e;

	assert(al != NULL);

	/*
	 * What we would really like to do is just call
	 * sll_destroy(*p, f);
	 * Problem is we can't pass a function which knows
	 * about the * {key, value}_free functions. (you can't do
	 * currying in C)
	 */
	while (!sll_empty(al->list)) {
		e = sll_get_data(al->list).ptr;
		if (key_free != NULL)
			key_free(e->key.ptr);
		if (value_free != NULL)
			value_free(e->value.ptr);
		al->list = sll_remove_head(al->list, free);
	}

	free(al);
}


/*
 * Internal function which alist_insert and alist_insert_uniq call.
 * Argument list is the same as these two functions, except for an extra
 * integer tacked onto the end.  This integer is nonzero if existing
 * key entries are not allowed.  If existing key entries are allowed, the
 * value of that key is overwritten.
 */
static alist
alist_insert_internal(alist al, gendata key, gendata value, eq_func eq,
		      free_func free_value, int uniq)
{
	sll new_head;
	alist_entry e;
	gendata e_data;
	sll l;

	assert(al != NULL);
	assert(eq != NULL);

	l = al->list;

	/*
	 * We'll have to check if there's already a value with the same key.
	 * If there is, overwrite that value unless uniq.
	 */
	while (!sll_empty(l)) {
		e = sll_get_data(l).ptr;
		if (eq(key, e->key)) {
			/* Duplicates not allowed? */
			if (uniq) {
				errno = EINVAL;
				return NULL;
			}

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
		return NULL;

	e->key = key;
	e->value = value;
	e_data.ptr = e;

	new_head = sll_prepend_head(al->list, e_data);
	if (new_head == NULL) {
		free(e);
		return NULL;
	}

	al->list = new_head;
	return al;
}


/**
 * \brief Add a (key, value) pair to a linked list (with replace)
 *
 * Add a data element to the association list with the given key or replace
 * an existing element with the same key.
 *
 * \param al	      The association list to insert the data in.
 * \param key	      The key of the data.
 * \param value	      The data to insert.
 * \param eq          The equals predicate for two keys.
 * \param free_value  The function used to free the old value's data if it
 *		       needs to be replaced, or \c NULL if the data does not
 *		       need to be freed.
 *
 * \return  The original alist, or \c NULL if the data could not be inserted.
 *          Original alist is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa alist_insert_uniq, alist_delete
 */
alist
alist_insert(alist al, gendata key, gendata value, eq_func eq,
	     free_func free_value)
{
	return alist_insert_internal(al, key, value, eq, free_value, 0);
}


/**
 * \brief Add a (key, value) pair to a linked list (no replace)
 *
 * Add a data element to the association list with the given key.  If there
 * already is an element with the same key in the list, it is regarded
 * as an error.
 *
 * \param al	      The association list to insert the data in.
 * \param key	      The key of the data.
 * \param value	      The data to insert.
 * \param eq          The equals predicate for two keys.
 *
 * \return  The original alist, or \p NULL if the data could not be inserted.
 *          Original alist is still valid in case of error.
 *
 * \par Errno values:
 * - \b EINVAL if the key is already in the list.
 * - \b ENOMEM if out of memory.
 *
 * \sa alist_insert, alist_delete
 */
alist
alist_insert_uniq(alist al, gendata key, gendata value, eq_func eq)
{
	return alist_insert_internal(al, key, value, eq, NULL, 1);
}


/**
 * \brief Look up an element in the association list.
 *
 * \param al    The association list which contains the element.
 * \param key   The key to the element.
 * \param eq    The equals predicate for two keys.
 * \param data  A pointer to the location where the element is stored, if
 *               it was found.
 *
 * \return  The alist if the key was found, or \c NULL if the key could not be
 *          found.
 *
 * \par Errno values:
 * - \b EINVAL if the key could not be found.
 */
alist
alist_lookup(alist al, gendata key, eq_func eq, gendata *data)
{
	sll l;
	alist_entry e;

	assert(al != NULL);
	assert(eq != NULL);

	l = al->list;

	while (!sll_empty(l)) {
		e = sll_get_data(l).ptr;
		if (eq(key, e->key)) {
			*data = e->value;
			return al;
		}
		l = sll_next(l);
	}

	/* If we got here, we obviously didn't find the entry. */
	errno = EINVAL;
	return NULL;
}


/**
 * \brief Delete an element from the association list.
 *
 * \param al          The association list which contains the element to delete.
 * \param key         The key to the element to delete.
 * \param eq          The equals predicate for two keys.
 * \param key_free    The function which is used to free the key data, or
 *			\c NULL if no action should be taken on the key data.
 * \param value_free  The function which is used to free the value data, or
 *			\c NULL if no action should be taken on the value data.
 *
 * \return  The alist, or \c NULL if the key could not be found.
 *
 * \par Errno values:
 * - \b EINVAL if the key could not be found.
 *
 * \sa alist_insert
 */
alist
alist_delete(alist al, gendata key, eq_func eq, free_func key_free,
	     free_func value_free)
{
	sll l, n;
	alist_entry e;

	assert(al != NULL);
	assert(eq != NULL);

	l = al->list;

	if (sll_empty(l)) {
		errno = EINVAL;
		return NULL;
	}

	/*
	 * The first entry is a special case.  Doubly linked lists might
	 *  be easier, but it's not convenience we are after in here.
	 */
	e = sll_get_data(l).ptr;
	if (eq(key, e->key)) {
		if (key_free != NULL)
			key_free(e->key.ptr);
		if (value_free != NULL)
			value_free(e->value.ptr);
		/* We are removing the first entry from the list! */
		al->list = sll_remove_head(l, free);
		return al;
	}

	/* n is always one ahead of l (so, n == l->next) */
	n = sll_next(l);

	while (!sll_empty(n)) {
		e = sll_get_data(n).ptr;
		if (eq(key, e->key)) {
			if (key_free != NULL)
				key_free(e->key.ptr);
			if (value_free != NULL)
				value_free(e->value.ptr);
			sll_remove_next(l, free);
			return al;
		}
		l = sll_next(l);
		n = sll_next(n);
	}

	/* If we got here, we obviously didn't find the entry. */
	errno = EINVAL;
	return NULL;
}


/**
 * \brief Return whether or not an association list is empty.
 *
 * \param al  The association list to check.
 *
 * \return  Non-zero if the list is empty, 0 if it is not.
 */
int
alist_empty(alist al)
{
	assert(al != NULL);

	return sll_empty(al->list);
}


/**
 * \brief Walk through all elements of an alist.
 *
 * Walk an alist, using a user-specified function on the list's pairs.
 *
 * \attention
 * While using this function, it is not allowed to remove entries other than
 * the current entry.  It is allowed to change the contents of the key and
 * value.
 *
 * \param al    The alist to walk.
 * \param walk  The function which will process the pairs.
 * \param data  Any data to pass to the function every time it is called.
 */
void
alist_walk(alist al, assoc_func walk, gendata data)
{
	alist_entry e;
	sll l = al->list;
	sll n = NULL;

	assert(al != NULL);

	while(!sll_empty(l)) {
		/* n is stored in case user deletes the current entry */
		n = sll_next(l);
		e = sll_get_data(l).ptr;
		walk(&e->key, &e->value, data);
		l = n;
	}
}
