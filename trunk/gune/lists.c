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
 * \brief Linked lists implementation.
 *
 * \file lists.c
 * Singly and doubly linked list implementations.
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/misc.h>
#include <gune/lists.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a new empty singly linked list.
 *
 * \return  A new empty singly linked list.
 *
 * \sa sll_destroy dll_create
 */
sll
sll_create(void)
{
	/* This represents the empty list. */
	return CONST_PTR;
}


/**
 * \brief Destroy a singly linked list.
 *
 * Destroy a singly linked list by deleting each element.  The data is freed
 * by calling the user-supplied function \p f on it.
 *
 * \attention
 * If the same data is included multiple times in the list, the free function
 * gets called that many times.
 *
 * \param ll  The list to destroy.
 * \param f   The function which is used to free the data, or \c NULL if no
 *		action should be taken to free the data.
 *
 * \sa  sll_create dll_destroy
 */
void
sll_destroy(sll ll, free_func f)
{
	assert(ll != NULL);

	while (!sll_empty(ll))
		ll = sll_remove_head(ll, f);
}


/**
 * \brief Return the number of elements in a singly linked list.
 *
 * \param ll The singly linked list to get the number of elements from.
 *
 * \return  The number of elements as an \c unsigned \c int.
 *
 * \sa  dll_count
 */
unsigned int
sll_count(sll ll)
{
	int count;

	assert(ll != NULL);

	for (count = 0; !sll_empty(ll); ++count, ll = ll->next);

	return count;
}


/**
 * \brief Return whether or not a singly linked list is empty.
 *
 * \param ll  The singly linked list to check.
 *
 * \return  Non-zero if the list is empty, 0 if it is not.
 *
 * \sa  dll_empty
 */
int
sll_empty(sll ll)
{
	assert(ll != NULL);

	return ll == CONST_PTR;
}


/**
 * \brief Remove the head from a singly linked list.
 *
 * \attention
 * The head actually has to be the real \e first item of a linked list, since
 * there is no way to re-link the \c next element of the item before the
 * head of the supplied list.
 *
 * \param ll  The singly linked list.
 * \param f   The function which is used to free the data elements of the
 *		list, or \c NULL if no action is to be taken to free the data.
 *
 * \return  A pointer to the new (headless) singly linked list.
 *
 * \sa  sll_remove_next, dll_remove_head, sll_empty
 */
sll
sll_remove_head(sll ll, free_func f)
{
	sll begin;

	assert(ll != NULL);
	assert(!sll_empty(ll));

	if (f != NULL)
		f(ll->data.ptr);

	/* Move the ll pointer to the next element */
	begin = ll;
	ll = ll->next;

	/* Free up used space by the head element */
	free(begin);
	return ll;
}


/**
 * \brief Remove the next item after the head from a singly linked list.
 *
 * \param ll  The singly linked list.
 * \param f   The function which is used to free the data elements of the
 *		list, or \c NULL if no action is to be taken to free the data.
 *
 * \return  A pointer to the new singly linked list, or \c NULL if the
 *	     next item does not exist.
 *
 * \par Errno values:
 * - \b EINVAL if the next item does not exist.
 *
 * \sa  sll_remove_head, sll_empty
 */
sll
sll_remove_next(sll ll, free_func f)
{
	sll begin;
	
	assert(ll != NULL);
	assert(!sll_empty(ll));

	if (f != NULL)
		f(ll->data.ptr);

	/* Move the ll pointer to the next element */
	begin = ll;
	ll = ll->next;

	if (sll_empty(ll)) {
		/* Should've checked with sll_empty before calling this func */
		errno = EINVAL;
		return NULL;
	}

	begin->next = ll->next;

	/* Free up used space by the removed element */
	free(ll);

	return ll;
}


/**
 * \brief Prepend an element to the head of a singly linked list.
 *
 * \param ll    The singly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or \c NULL in case of error.  The old linked
 *	     list is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa  sll_append_head, dll_prepend_head
 */
sll
sll_prepend_head(sll ll, gendata data)
{
	sll_t *new;

	assert(ll != NULL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(sll_t))) == NULL)
		return NULL;
	new->data = data;

	/* Prepend to the current list */
	new->next = ll;
	return (sll)new;
}


/**
 * \brief Append an element to the head of a singly linked list.
 *
 * \param ll    The singly linked list to append the element to.
 * \param data  The element to append.
 *
 * \return  The old(!) linked list or \c NULL in case of error.  The old
 *	      linked list is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa  sll_prepend_head dll_append_head
 */
sll
sll_append_head(sll ll, gendata data)
{
	sll_t *new;

	assert(ll != NULL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(sll_t))) == NULL)
		return NULL;

	if (!sll_empty(ll))
		new->next = ll->next;
	else
		new->next = CONST_PTR;

	new->data = data;

	/* Append at the current list */
	ll->next = new;
	return (sll)new;
}


/**
 * \brief Move forward in a singly linked list.
 *
 * \param ll     The singly linked list to search in.
 * \param nskip  The number of elements to search forward.
 *
 * \return The list at the indexed position, or \c NULL if the index
 *          is out of bounds.
 *
 * \par Errno values:
 * - \b EINVAL if the index is out of bounds.
 *
 * \sa dll_forward
 */
sll
sll_forward(sll ll, unsigned int nskip)
{
	unsigned int i;

	assert(ll != NULL);
	assert(!sll_empty(ll));

	for (i = 0; i < nskip; ++i) {
		if (ll == NULL) {
			errno = EINVAL;
			return NULL;
		}
		ll = ll->next;
	}

	return ll;
}


/**
 * \brief Get the data at the current position of a singly linked list.
 *
 * \param ll     The singly linked list to look in.
 *
 * \return       The data in the list.
 *
 * \sa sll_set_data dll_get_data
 */
gendata
sll_get_data(sll ll)
{
	assert(ll != NULL);
	assert(!sll_empty(ll));

	return ll->data;
}


/**
 * \brief Set the data at the current position of a singly linked list.
 *
 * \param ll     The singly linked list to store the data in.
 * \param data   The data to store.
 *
 * \return       The supplied list, with modified data.
 *
 * \sa sll_get_data dll_set_data
 */
sll
sll_set_data(sll ll, gendata data)
{
	assert(ll != NULL);
	assert(!sll_empty(ll));

	ll->data = data;

	return ll;
}


/*
 * FIXME Should we really do this on DEBUG only?  This is not only useful for
 *   debugging gune itself, but also for those quick debug dumps in programs,
 *   or calling from GDB, even if gune itself hasn't been built with -g.
 */
#ifdef DEBUG
/**
 * \brief Print a dump of a singly linked list.
 *
 * The element data is formatted according to the supplied format string.
 *
 * \note
 * This function is intended for testing and debugging purposes only.
 *
 * \param ll   The singly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa dll_dump
 */
void
sll_dump(sll ll, const char *fmt)
{
	if (ll == NULL) {
		printf("NULL\n");
		return;
	}

	for (; !sll_empty(ll); ll = ll->next) {
		/* Just dump the integer value of the ptr */
		printf(fmt, ll->data.posnum);
		printf(" -> ");
	}
	printf("(EOL)\n");
}
#endif /* DEBUG */


/**
 * \brief Create a new empty doubly linked list.
 *
 * \return  A new empty doubly linked list.
 *
 * \sa dll_destroy sll_create
 */
dll
dll_create(void)
{
	/* This represents the empty list */
	return CONST_PTR;
}


/**
 * \brief Destroy a doubly linked list.
 *
 * The data stored in the linked list is freed by calling the user-supplied
 * function \p f on it.
 *
 * \attention
 * If the same data is included multiple times in the list, the free function
 * gets called that many times.
 *
 * \param ll  The list to destroy.
 * \param f   The function which is used to free the data, or \c NULL if no
 *		action should be taken to free the data.
 *
 * \sa  dll_create sll_destroy
 */
void
dll_destroy(dll ll, free_func f)
{
	assert(ll != NULL);

	while (!dll_empty(ll))
		ll = dll_remove_head(ll, f);
}


/**
 * \brief Return the number of elements in a doubly linked list.
 *
 * \param ll The doubly linked list to get the number of elements from.
 *
 * \return  The number of elements as an \c unsigned \c int.
 *
 * \sa  sll_count
 */
unsigned int
dll_count(dll ll)
{
	int count;

	assert(ll != NULL);

	for (count = 0; !dll_empty(ll); ++count, ll = ll->next);

	return count;
}


/**
 * \brief Return whether or not a linked list is an empty one.
 *
 * \param ll  The doubly linked list to check.
 *
 * \return    Non-zero if the list is empty, 0 if it is not.
 *
 * \sa  sll_empty
 */
int
dll_empty(dll ll)
{
	assert(ll != NULL);

	return ll == CONST_PTR;
}


/**
 * \brief Remove the head from a doubly linked list.
 *
 * This function honours the prev element of the list, so the `head' does
 * not necessarily have to be the first element of a total list.
 *
 * \param ll  The doubly linked list.
 * \param f   The function which is used to free the data elements of the
 *		list, or \c NULL if no action is to be taken to free the data.
 *
 * \return  A pointer to the new doubly linked list.
 *
 * \sa  sll_remove
 */
dll
dll_remove_head(dll ll, free_func f)
{
	dll begin;
	
	assert(ll != NULL);
	assert(!dll_empty(ll));

	if (f != NULL)
		f(ll->data.ptr);

	/* Move the ll pointer to the next element */
	begin = ll;
	ll = ll->next;
	ll->prev = begin->prev;

	/* Free up used space by the head element */
	free(begin);
	return ll;
}


/**
 * \brief Prepend an element to the head of a doubly linked list.
 *
 * This function honours the (old) prev element of the list, so the `head'
 * does not necessarily have to be the first element of a total list.
 *
 * \param ll    The doubly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or \c NULL in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa  dll_append_head sll_prepend_head
 */
dll
dll_prepend_head(dll ll, gendata data)
{
	dll_t *new;

	assert(ll != NULL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(dll_t))) == NULL)
		return NULL;
	new->data = data;
	new->next = ll;

	/* Prepend to the current list */
	if (!dll_empty(ll)) {
		new->prev = ll->prev;
		ll->prev = new;

		if (!dll_empty(new->prev))
			new->prev->next = new;
	} else {
		new->prev = CONST_PTR;
	}
	return (dll)new;
}


/**
 * \brief Append an element at the head of a doubly linked list.
 *
 * \param ll    The doubly linked list to prepend the element to.
 * \param data  The element to append.
 *
 * \return  The old(!) linked list or \c NULL in case of error.  The old
 *	     list is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa  sll_append_head dll_prepend_head
 */
dll
dll_append_head(dll ll, gendata data)
{
	dll_t *new;

	assert(ll != NULL);

	if ((new = malloc(sizeof(dll_t))) == NULL)
		return NULL;
	new->data = data;
	new->prev = ll;

	if (!dll_empty(ll)) {
		new->next = ll->next;
		new->prev = ll;
		ll->next = new;

		if (!dll_empty(new->next))
			new->next->prev = new;
	} else {
		new->next = CONST_PTR;
	}

	return ll;
}


/**
 * \brief Move forward in a doubly linked list.
 *
 * \param ll     The doubly linked list to search in.
 * \param nskip  The number of elements to search forward.
 *
 * \return The list at the indexed position, or \c NULL if the index
 *          is out of bounds.
 *
 * \par Errno values:
 * - \b EINVAL if the index is out of bounds.
 *
 * \sa sll_forward dll_backward
 */
dll
dll_forward(dll ll, unsigned int nskip)
{
	unsigned int i;

	assert(ll != NULL);
	assert(!dll_empty(ll));

	for (i = 0; i < nskip; ++i) {
		if (ll == NULL) {
			errno = EINVAL;
			return NULL;
		}
		ll = ll->next;
	}

	return ll;
}


/**
 * \brief Move backward in a doubly linked list.
 *
 * \param ll     The doubly linked list to search in.
 * \param nskip  The number of elements to search backward.
 *
 * \return The list at the indexed position, or \c NULL if the index
 *          is out of bounds.
 *
 * \par Errno values:
 * - \b EINVAL if the index is out of bounds.
 *
 * \sa dll_forward
 */
dll
dll_backward(dll ll, unsigned int nskip)
{
	unsigned int i;

	assert(ll != NULL);
	assert(!dll_empty(ll));

	for (i = 0; i < nskip; ++i) {
		if (ll == NULL) {
			errno = EINVAL;
			return NULL;
		}
		ll = ll->prev;
	}

	return ll;
}


/**
 * \brief Get the data at the current position of a doubly linked list.
 *
 * \param ll     The doubly linked list to look in.
 *
 * \return       The data in the list.
 *
 * \sa dll_set_data sll_get_data
 */
gendata
dll_get_data(dll ll)
{
	assert(ll != NULL);
	assert(!dll_empty(ll));

	return ll->data;
}


/**
 * \brief Set the data at the current position of a doubly linked list.
 *
 * \param ll     The doubly linked list to store the data in.
 * \param data   The data to store.
 *
 * \return       The supplied list, with modified data.
 *
 * \sa dll_get_data sll_set_data
 */
dll
dll_set_data(dll ll, gendata data)
{
	assert(ll != NULL);
	assert(!dll_empty(ll));

	ll->data = data;

	return ll;
}


#ifdef DEBUG
/**
 * \brief Print a dump of a doubly linked list.
 *
 * The element data is formatted according to the supplied format string.
 *
 * \note
 * This function is intended for testing and debug purposes only.
 *
 * \param ll   The doubly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa sll_dump
 */
void
dll_dump(dll ll, const char *fmt)
{
	if (ll == NULL) {
		printf("NULL\n");
		return;
	}

#ifdef BOUNDS_CHECKING
	if (ll->prev != CONST_PTR)
		log_entry(WARN_ERROR, "Gune: dll_dump: First element of doubly "
			  "linked list has non-CONST_PTR prev pointer");
#endif

	for (; !dll_empty(ll); ll = ll->next) {
#ifdef BOUNDS_CHECKING
		if (ll->prev != NULL && ll->prev->next != ll)
			log_entry(WARN_ERROR, "Gune: dll_dump: Linked list is not "
				  "consistent");
#endif
		/* Just dump the integer value of the ptr */
		printf(fmt, ll->data.posnum);
		printf(" -> ");
	}
	printf("(EOL)\n");
}
#endif /* DEBUG */
