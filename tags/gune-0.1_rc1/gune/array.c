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
 * \brief Dynamic arrays implementation.
 *
 * \file array.c
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/array.h>

/** Compile-time option of initial array size */
#define ARRAY_INITIAL_SIZE	16

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a new empty array.
 *
 * \return  A new empty array, or \c NULL if out of memory.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_destroy
 */
array
array_create(void)
{
	array_t *ar;

	if ((ar = malloc(sizeof(array_t))) == NULL)
		return NULL;

	if ((ar->data = malloc(ARRAY_INITIAL_SIZE * sizeof(gendata))) == NULL) {
		free(ar);
		return NULL;
	}

	ar->size = 0;
	ar->capacity = ARRAY_INITIAL_SIZE;

	return (array)ar;
}


/**
 * \brief Free all memory allocated for an array.
 *
 * The data stored within the array is freed by calling the user-supplied
 * function \p f on it.
 *
 * \attention
 * If the same data is included multiple times in the list, the free function
 * gets called that many times.
 *
 * \param ar  The array to destroy.
 * \param f   The function which is used to free the data, or \c NULL if no
 *		action	should be taken to free the data.
 *
 * \sa  array_create
 */
void
array_destroy(array ar, free_func f)
{
	gendata *p;

	assert(ar != NULL);
	assert(ar->data != NULL);

	if (f != NULL) {
		for(p = ar->data; p < (ar->data + ar->size); ++p)
			f(p->ptr);
	}

	free(ar->data);
	free(ar);
}


/**
 * \brief Get the number of elements in the array.
 *
 * \param ar  The array to get the size of.
 */
unsigned int
array_size(array ar)
{
	return ar->size;
}


/**
 * \brief Resize an array.
 *
 * \note
 * This does not actually free any memory if the array is made smaller.
 * Use array_compact to accomplish this.
 *
 * \param ar    The array to resize.
 * \param size  The new (absolute) size of the array.
 *
 * \return  The array given as input, or \c NULL in case of error.
 *           The old array is still valid if an error occurred.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_grow, array_shrink, array_compact
 */
array
array_resize(array ar, unsigned int size)
{
	int newsize;		/* Do not corrupt old array in case of error */
	gendata *newptr;

	assert(ar != NULL);
	assert(ar->data != NULL);

	/*
	 * Use buddy algorithm to resize data buffer if we resized beyond old
	 *  size.
	 */
	if (size > ar->capacity) {
		newsize = ar->capacity * 2;
		if ((newptr = realloc(ar->data, newsize * sizeof(gendata)))
		    == NULL)
			return NULL;
		ar->data = newptr;
		ar->capacity = newsize;
	}

	ar->size = size;

	return ar;
}


/**
 * \brief Get the value at an index in an array.
 *
 * \param ar     The array to get the value from.
 * \param index  The index of the value to get.
 *
 * \return       The value at the specified index in the array.
 */
gendata
array_get_data(array ar, unsigned int index)
{
	assert(ar != NULL);
	assert(ar->data != NULL);

#ifdef BOUNDS_CHECKING
	if (index > ar->size)
		log_entry(WARN_ERROR, "Gune: array_get_data: Index (%u) "
			  "out of bounds", index);
#endif

	return *(ar->data + index);
}


/**
 * \brief Set the value at an index in an array.
 *
 * \param ar     The array to set the value in.
 * \param index  The index of the value to set.
 * \param value  The value to set at the index.
 *
 * \return       The supplied array.
 */
array
array_set_data(array ar, unsigned int index, gendata value)
{
	assert(ar != NULL);
	assert(ar->data != NULL);


#ifdef BOUNDS_CHECKING
	if (index > ar->size)
		log_entry(WARN_ERROR, "Gune: array_get_data: Index (%u) "
			  "out of bounds", index);
#endif

	*(ar->data + index) = value;

	return ar;
}


/**
 * \brief Compactise an array.
 *
 * Use this function when the array has been very big in the past, and you
 * wish to make sure the memory used is freed to fit the new array more
 * tightly.
 *
 * \param ar  The array to compact.
 *
 * \return    The supplied array, or \c NULL if an error occurred during
 *            resize.  The old array is still valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 */
array
array_compact(array ar)
{
	gendata *newptr;
	unsigned int newsize = 1;

	assert(ar != NULL);
	assert(ar->data != NULL);

	/* NOTE: This can probably be sped up quite a bit */
	for (; newsize < ar->size; newsize *= 2);

	if ((newptr = realloc(ar->data, newsize * sizeof(gendata))) != NULL) {
		ar->data = newptr;
		ar->capacity = newsize;
	}

	return ar;
}


/**
 * \brief Grow an array by \e n items.
 *
 * \param ar      The array to grow.
 * \param amount  The amount to grow.
 *
 * \return    The supplied array, or \c NULL if an error occurred during
 *            resize.  The old array is still valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_shrink, array_resize
 */
array
array_grow(array ar, int amount)
{
	if (amount < 0)
		return array_shrink(ar, 0 - amount);

	assert(ar != NULL);

	return array_resize(ar, ar->size + (unsigned int)amount);
}


/**
 * \brief Shrink an array by n items.
 *
 * This does not actually free any memory.  Use array_compact to accomplish
 *  this.
 *
 * \param ar      The array to shrink.
 * \param amount  The amount to shrink.
 *
 * \return    The supplied array, or \c NULL if an error occurred during
 *            resize.  The old array is still valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_grow, array_resize
 */
array
array_shrink(array ar, int amount)
{
	if (amount < 0)
		return array_grow(ar, 0 - amount);

	assert(ar != NULL);

#ifdef BOUNDS_CHECKING
	if (((int)ar->size - amount) < 0)
		log_entry(WARN_ERROR, "Gune: array_shrink: Can not shrink to"
			  " size smaller than zero (%i)", ar->size - amount);
#endif
	return array_resize(ar, (unsigned int)(ar->size - amount));
}


/**
 * \brief Add an element to the end of an array.
 *
 * This function increases the array's size by one and places the \p value
 * at the newly allocated spot at the end.
 *
 * \param ar     The array to add an element to.
 * \param value  The value to add to the array.
 *
 * \return    The supplied array, or \c NULL if an error occurred during
 *            resize.  The old array is still valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_remove
 */
array
array_add(array ar, gendata value)
{
	ar = array_grow(ar, 1);
	if (ar != NULL) {
		assert(ar->data != NULL);
		*(ar->data + ar->size - 1) = value;
	}

	return ar;
}


/**
 * \brief Remove the last element from an array.
 *
 * This function decreases the array's size by one, thereby removing the
 * element at the end.
 *
 * \param ar  The array to remove the element from.
 *
 * \return    The supplied array, or \c NULL if an error occurred during
 *            resize.  The old array is still valid.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa array_add
 */
array
array_remove(array ar)
{
	return array_shrink(ar, 1);
}
