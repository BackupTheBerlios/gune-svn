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
 * \brief Hash tables implementation.
 *
 * \file ht.c
 */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/ht.h>

static ht ht_insert_internal(ht, gendata, gendata, eq_func,
			     free_func, free_func, int);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a new empty hash table.
 *
 * \param range  The range of the key function (\f$ 0 \le x < range \f$).
 * \param hash   The hashing function to use on keys.
 *
 * \return  A new empty hash table object, or \c NULL if an error occurred.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
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
		return NULL;

	t->range = range;
	t->hash = hash;

	/*
	 * We could use a Gune array, but we're not resizing so we would just
	 * be introducing unnecessary overhead.
	 */
	if ((t->buckets = (alist *)malloc(range * sizeof(alist))) == NULL) {
		free(t);
		return NULL;
	}

	/*
	 * Slightly inefficient, we could also use memset, but that's
	 * making assumptions about the value of NULL.  Plus, this code should
	 * not be the bottleneck of any sane program.
	 */
	for (i = 0; i < range; ++i) {
		if ((*(t->buckets + i) = alist_create()) == NULL) {
			/* Creation went wrong halfway?  Destroy all previous */
			for (j = 0; j < i; ++j)
				alist_destroy(*(t->buckets + j), NULL, NULL);

			free(t);
			return NULL;
		}
	}

	return (ht)t;
}


/**
 * \brief Free all memory allocated for a hash table.
 *
 * The data within the table is freed by calling a user-supplied free function.
 *
 * \note
 * If the same data is included multiple times in the table, the free function
 * gets called that many times.
 * This is currently impossible, unless really strange things have been
 * going on. (like, say, a random hashing function)
 *
 * \param t           The hash table to destroy.
 * \param free_key    The function which is used to free the key data, or
 *			\c NULL if no action should be taken on the key data.
 * \param free_value  The function which is used to free the value data, or
 *			\c NULL if no action should be taken on the value data.
 *
 * \sa ht_create
 */
void
ht_destroy(ht t, free_func free_key, free_func free_value)
{
	alist *al;

	assert(t != NULL);

	for (al = t->buckets; al < (t->buckets + t->range); ++al)
		alist_destroy(*al, free_key, free_value);

	free(t->buckets);
	free(t);
}


/*
 * Internal function which ht_insert and ht_insert_uniq call.
 * Argument list is the same as these two functions, except for an extra
 * integer tacked onto the end.  This integer is nonzero if existing
 * key entries are not allowed.  If existing key entries are allowed, the
 * value of that key is overwritten.
 */
static ht
ht_insert_internal(ht t, gendata key, gendata value, eq_func eq,
		   free_func free_key, free_func free_value, int uniq)
{
	unsigned int bucketnr;
	alist al;

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
	if (uniq)
		al = alist_insert_uniq(al, key, value, eq);
	else
		al = alist_insert(al, key, value, eq, free_key, free_value);

	if (al == NULL)
		t = NULL;
	else
		/* Strictly not needed */
		*(t->buckets + bucketnr) = al;

	return t;
}


/**
 * \brief Add a (key, value) pair to a hash table (with replace)
 *
 * Add a data element to the hash table with the given key or replace an
 *  existing element with the same key.
 *
 * \param t           The hash table to insert the data in.
 * \param key         The key of the data.
 * \param value       The data to insert.
 * \param eq          The equals predicate for two keys.
 * \param free_key    The function used to free the old key's data if it
 *		       needs to be replaced, or \c NULL if the data does not
 *		       need to be freed.
 * \param free_value  The function used to free the old value's data if it
 *		       needs to be replaced, or \c NULL if the data does not
 *		       need to be freed.
 *
 * \return  The original hash table, or \c NULL if the data could not be
 *           inserted.  Original hash table is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa ht_insert_uniq, ht_delete
 */
ht
ht_insert(ht t, gendata key, gendata value, eq_func eq,
	  free_func free_key, free_func free_value)
{
	return ht_insert_internal(t, key, value, eq, free_key, free_value, 0);
}


/**
 * \brief Add a (key, value) pair to a hash table (no replace)
 *
 * Add a data element to the hash table with the given key.  If there already
 * is an element with the same key in the table, it is regarded as an error.
 *
 * \param t           The hash table to insert the data in.
 * \param key         The key of the data.
 * \param value       The data to insert.
 * \param eq          The equals predicate for two keys.
 *
 * \return  The original hash table, or \c NULL if the data could not be
 *           inserted.  Original hash table is still valid in case of error.
 *
 * \par Errno values:
 * - \b EINVAL if the key is already in the table.
 * - \b ENOMEM if out of memory.
 *
 * \sa ht_insert, ht_delete
 */
ht
ht_insert_uniq(ht t, gendata key, gendata value, eq_func eq)
{
	return ht_insert_internal(t, key, value, eq, NULL, NULL, 1);
}


/**
 * \brief Look up an element in a hash table.
 *
 * \param t     The hashtable which contains the element.
 * \param key   The key to the element.
 * \param eq    The equals predicate for two keys.
 * \param data  A pointer to the location where the element is stored, if
 *               it was found.
 *
 * \return  The hash table, or \c NULL if the key could not be found.
 *
 * \par Errno values:
 * - \b EINVAL if the key could not be found.
 */
ht
ht_lookup(ht t, gendata key, eq_func eq, gendata *data)
{
	unsigned int bucketnr;
	alist al;

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
	if (alist_lookup(al, key, eq, data) == NULL)
		return NULL;

	return t;
}


/**
 * \brief Delete an element from the hash table.
 *
 * \param t           The hashtable which contains the element to delete.
 * \param key         The key to the element to delete.
 * \param eq          The equals predicate for two keys.
 * \param free_key    The function which is used to free the key data, or
 *			\c NULL if no action should be taken on the key data.
 * \param free_value  The function which is used to free the value data, or
 *			\c NULL if no action should be taken on the value data.
 *
 * \return  The hash table, or \c NULL if the key could not be found.
 *
 * \par Errno values:
 * - \b EINVAL if the key could not be found.
 *
 * \sa ht_insert
 */
ht
ht_delete(ht t, gendata key, eq_func eq, free_func free_key,
	  free_func free_value)
{
	unsigned int bucketnr;
	alist al;

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
	if (alist_delete(al, key, eq, free_key, free_value) == NULL)
		return NULL;

	return t;
}


/**
 * \brief Return whether or not a hash table is empty.
 *
 * \param t  The hash table to check.
 *
 * \return  Non-zero if the hash table is empty, 0 if it is not.
 */
int
ht_empty(ht t)
{
	unsigned int i;

	assert(t != NULL);

	for (i = 0; i < t->range; ++i)
		if (!alist_empty(t->buckets[i]))
			return 0;

	return 1;
}


/**
 * \brief Walk through all elements of a hash table.
 *
 * Walk a hash table, using a user-specified function on the table's pairs.
 *
 * \attention
 * While using this function, it is not allowed to remove entries other than
 * the current entry.  It is allowed to change the contents of the key and
 * value, as long as the key's hash will not be affected.
 *
 * \param t     The hash table to walk
 * \param walk  The function which will process the hash pairs
 * \param data  Any data to pass to the function every time it is called.
 */
void
ht_walk(ht t, assoc_func walk, gendata data)
{
	unsigned int i;

	assert(t != NULL);
	assert(t->buckets != NULL);

	for (i = 0; i < t->range; ++i)
		alist_walk(t->buckets[i], walk, data);
}


/**
 * \brief Merge two hash tables together.
 *
 * Add all data elements from a hash table to another hash table,
 * replacing the value of all existing elements with the same key.
 *
 * \note
 * This function is \f$ O(n \cdot m) \f$ with \f$ n \f$ the length of
 * \p base and \f$ m \f$ the length of \p rest.
 *
 * \param base	      The hash table to insert the data in.
 * \param rest        The hash table to be merged into \p base.
 * \param eq          The equals predicate for two keys.
 * \param free_key    The function used to free the \p rest list's key data,
 *		       or \c NULL if the data does not need to be freed.
 * \param free_value  The function used to free the \p base value's data if it
 *		       needs to be replaced, or \c NULL if the data does not
 *		       need to be freed.
 *
 * \return  The \p base hash table, merged with \p rest, or NULL in case
 *	     of error.  If an error occurred, the \p base hash table is still
 *	     valid, but it is undefined which items from the \p rest table will
 *	     have been merged into the table and which haven't.  The \p rest
 *	     table will not be valid after the function has finished.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if the two hash tables do not have the same range.
 *
 * \sa ht_insert ht_delete ht_merge_uniq
 */
ht
ht_merge(ht base, ht rest, eq_func eq, free_func free_key, free_func free_value)
{
	unsigned int i;
	alist al_tmp;

	assert(base != NULL);
	assert(rest != NULL);
	assert(base->buckets != NULL);
	assert(rest->buckets != NULL);

	/* XXX: Maybe this requirement is too strict */
	if (base->range != rest->range) {
		errno = EINVAL;
		return NULL;
	}

	for (i = 0; i < base->range; ++i) {
		al_tmp = alist_merge(base->buckets[i], rest->buckets[i], eq,
				     free_key, free_value);
		if (al_tmp == NULL)
			return NULL;
		else
			base->buckets[i] = al_tmp;
	}

	/*
	 * NOTE: This is redundant in the current implementation of alist.
	 * Also, we can't `destroy' the alist in the above loop, because
	 * if we encounter an error, the hash table should still be valid.
	 */
	for (i = 0; i < rest->range; ++i)
		alist_destroy(rest->buckets[i], free_key, free_value);
     
	free(rest->buckets);
	free(rest);

	return base;
}


/**
 * \brief Merge two hash tables together uniquely.
 *
 * Add all data elements from a hash table to another hash table with the
 * given key.  If a duplicate key is encountered, the entry is not inserted
 * into the \p base hash table.
 *
 * \note
 * This function is \f$ O(n \cdot m) \f$ with \f$ n \f$ the length of
 * \p base and \f$ m \f$ the length of \p rest.
 *
 * \param base	      The hash table to insert the data in.
 * \param rest        The hash table to be merged into \p base.
 * \param eq          The equals predicate for two keys.
 *
 * \return  The \p base hash table, merged with \p rest, or NULL in case of
 *	     error.  If an error occurred, the \p base hash table is still
 *	     valid, but it is undefined which items from the \p rest hash table
 *	     will have been merged into the table and which haven't.
 *	    The \p rest table will have been modified so it still contains
 *	     the entries which had matching keys in the \p base table.
 *	    The \p rest hash table will thus still be valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if the two hash tables do not have the same range.
 *
 * \sa ht_insert_uniq ht_delete ht_merge
 */
ht
ht_merge_uniq(ht base, ht rest, eq_func eq)
{
	unsigned int i;
	alist al_tmp;

	assert(base != NULL);
	assert(rest != NULL);
	assert(base->buckets != NULL);
	assert(rest->buckets != NULL);

	/* XXX: Maybe this requirement is too strict */
	if (base->range != rest->range) {
		errno = EINVAL;
		return NULL;
	}

	for (i = 0; i < base->range; ++i) {
		al_tmp = alist_merge_uniq(base->buckets[i],
					  rest->buckets[i], eq);
		if (al_tmp == NULL)
			return NULL;
		else
			base->buckets[i] = al_tmp;
	}

	return base;
}
