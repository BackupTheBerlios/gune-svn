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
 * \file lists.c
 * Linked list, queue and stack implementation
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <gune/lists.h>
#include <gune/error.h>

sll_t   * const ERROR_SLL   = (void *)error_dummy_func;
dll_t   * const ERROR_DLL   = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Creates a new empty singly linked list.
 *
 * \return  A new empty singly linked list.
 *
 * \sa sll_destroy dll_create
 */
sll
sll_create(void)
{
	return NULL;
}


/**
 * Destroy a singly linked list by deleting each element.
 *
 * \param ll  The list to destroy.
 *
 * \sa  sll_create dll_destroy
 */
void
sll_destroy(sll ll)
{
	assert(ll != ERROR_SLL);

	while (!sll_is_empty(ll))
		ll = sll_remove_head(ll);
}


/**
 * Returns the number of elements in the given singly linked list.
 *
 * \param ll The singly linked list to get the number of elements from.
 *
 * \return  The number of elements as an unsigned int.
 *
 * \sa  dll_count
 */
unsigned int
sll_count(sll ll)
{
	int count;

	assert(ll != ERROR_SLL);

	for (count = 0; ll != NULL; ++count, ll = ll->next)
		continue;
	return count;
}


/**
 * Returns whether or not the given linked list is an empty one.
 *
 * \param ll  The singly linked list to check.
 *
 * \return  A boolean indicating whether the linked list is empty.
 *
 * \sa  dll_is_empty
 */
bool
sll_is_empty(sll ll)
{
	assert(ll != ERROR_SLL);

	return ll == NULL;
}


/**
 * Removes the head from a singly linked list.
 *
 * \param ll  The singly linked list.
 *
 * \return  A pointer to the new singly linked list.
 *
 * \sa  dll_remove
 */
sll
sll_remove_head(sll ll)
{
	sll begin;
	
	assert(ll != NULL);
	assert(ll != ERROR_SLL);

	/* Move the ll pointer to the next element */
	begin = ll;
	ll = ll->next;

	/* Free up used space by the head element */
	free(begin);
	return ll;
}


/**
 * Prepends the given element to the head of the singly linked list.
 *
 * \param ll    The singly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or ERROR_SLL in case of error.
 *
 * \sa  sll_append_head dll_prepend_head
 */
sll
sll_prepend_head(sll ll, void *data)
{
	sll_t *new;

	assert(ll != ERROR_SLL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(sll_t))) == NULL)
		return (sll)ERROR_SLL;
	new->data = data;

	/* Prepend to the current list */
	new->next = ll;
	return (sll)new;
}


/**
 * Appends the given element at the head of the singly linked list.
 *
 * \param ll    The singly linked list to append the element at.
 * \param data  The element to append.
 *
 * \return  The old(!) linked list or ERROR_SLL in case of error.
 *
 * \sa  sll_prepend_head dll_append_head
 */
sll
sll_append_head(sll ll, void *data)
{
	sll_t *new;

	assert(ll != ERROR_SLL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(sll_t))) == NULL)
		return (sll)ERROR_SLL;
	new->data = data;
	if (ll != NULL)
		new->next = ll->next;
	else
		new->next = NULL;

	/* Append at the current list */
	ll->next = new;
	return (sll)new;
}


/**
 * Search forward in a singly linked list.
 *
 * \param ll     The singly linked list to search in.
 * \param nskip  The number of elements to search forward.
 *
 * \return The list at the indexed position, or ERROR_SLL if the index
 *          is out of bounds.
 *
 * \sa dll_forward
 */
sll
sll_forward(sll ll, unsigned int nskip)
{
	int i;

	assert(ll != NULL);
	assert(ll != ERROR_SLL);

	for (i = 0; i < nskip; ++i) {
		/*
		 * XXX: Maybe instead of this, we should do something with
		 *      bounds checking...
		 */
		if (ll == NULL)
			return (sll)ERROR_SLL;
		ll = ll->next;
	}

	return ll;
}


/**
 * Get the data at the current position from a singly linked list.
 *
 * \param ll     The singly linked list to look in.
 *
 * \return A pointer to the data
 *
 * \sa dll_get_data
 */
void *
sll_get_data(sll ll)
{
	assert(ll != NULL);
	assert(ll != ERROR_SLL);

	return ll->data;
}


#ifdef DEBUG
/**
 * Prints a dump of a singly linked list.
 * The element data is formatted according to the supplied format string.  This
 * function is intended for testing and debug purposes only.
 *
 * \param ll   The singly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa dll_dump
 */
void
sll_dump(sll ll, char *fmt)
{
	if (ll == ERROR_SLL) {
		printf("ERROR_SLL\n");
		return;
	}

	for (; ll != NULL; ll = ll->next) {
		printf(fmt, (int)ll->data);
		printf(" -> ");
	}
	printf("NULL\n");
}
#endif


/**
 * Creates a new empty doubly linked list.
 *
 * \return  A new empty doubly linked list.
 *
 * \sa dll_destroy sll_create
 */
dll
dll_create(void)
{
	return NULL;
}


/**
 * Destroy a doubly linked list by deleting each element.
 *
 * \param ll  The list to destroy.
 *
 * \sa  dll_create sll_destroy
 */
void
dll_destroy(dll ll)
{
	assert(ll != ERROR_DLL);

	while (!dll_is_empty(ll))
		ll = dll_remove_head(ll);
}


/**
 * Returns the number of elements in the given doubly linked list.
 *
 * \param ll The doubly linked list to get the number of elements from.
 *
 * \return  The number of elements as an unsigned int.
 *
 * \sa  sll_count
 */
unsigned int
dll_count(dll ll)
{
	int count;

	assert(ll != ERROR_DLL);

	for (count = 0; ll != NULL; ++count, ll = ll->next)
		continue;
	return count;
}


/**
 * Returns whether or not the given linked list is an empty one.
 *
 * \param ll  The doubly linked list to check.
 *
 * \return  A boolean indicating whether the linked list is empty.
 *
 * \sa  sll_is_empty
 */
bool
dll_is_empty(dll ll)
{
	assert(ll != ERROR_DLL);

	return ll == NULL;
}


/**
 * Removes the head from a doubly linked list.
 *
 * \param ll  The doubly linked list.
 *
 * \return  A pointer to the new doubly linked list.
 *
 * \sa  sll_remove
 */
dll
dll_remove_head(dll ll)
{
	dll begin;
	
	assert(ll != NULL);
	assert(ll != ERROR_DLL);

	/* Move the ll pointer to the next element */
	begin = ll;
	ll = ll->next;
	ll->prev = NULL;

	/* Free up used space by the head element */
	free(begin);
	return ll;
}


/**
 * Prepends the given element to the head of the doubly linked list, honouring
 *  the prev element of the list.
 *
 * \param ll    The doubly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or NULL in case of error.
 *
 * \sa  dll_append sll_prepend_head
 */
dll
dll_prepend_head(dll ll, void *data)
{
	dll_t *new;

	assert(ll != ERROR_DLL);

	/* Allocate the new element */
	if ((new = malloc(sizeof(dll_t))) == NULL)
		return (dll)ERROR_DLL;
	new->data = data;
	new->next = ll;

	/* Prepend to the current list */
	/* XXX: Test this on boundary values! */
	if (ll != NULL) {
		new->prev = ll->prev;
		ll->prev = new;

		if (new->prev != NULL)
			new->prev->next = new;
	} else {
		new->prev = NULL;
	}
	return (dll)new;
}


/**
 * Appends the given element at the head of the doubly linked list.
 *
 * \param ll    The doubly linked list to prepend the element at.
 * \param data  The element to append.
 *
 * \return  The old(!) linked list or NULL in case of error.
 *
 * \sa  dll_append sll_prepend_head
 */
dll
dll_append_head(dll ll, void *data)
{
	dll_t *new;

	assert(ll != ERROR_DLL);

	if ((new = malloc(sizeof(dll_t))) == NULL)
		return (dll)ERROR_DLL;
	new->data = data;
	new->prev = ll;

	/* XXX: Test this on boundary values! */
	if (ll != NULL) {
		new->next = ll->next;
		new->prev = ll;
		ll->next = new;

		if (new->next != NULL)
			new->next->prev = new;
	} else {
		new->next = NULL;
	}

	return ll;
}


/**
 * Search forward in a doubly linked list.
 *
 * \param ll     The doubly linked list to search in.
 * \param nskip  The number of elements to search forward.
 *
 * \return The list at the indexed position, or ERROR_DLL if the index
 *          is out of bounds.
 *
 * \sa sll_forward, dll_backward
 */
dll
dll_forward(dll ll, unsigned int nskip)
{
	int i;

	assert(ll != NULL);
	assert(ll != ERROR_DLL);

	for (i = 0; i < nskip; ++i) {
		/*
		 * XXX: Maybe instead of this, we should do something with
		 *      bounds checking...
		 */
		if (ll == NULL)
			return (dll)ERROR_DLL;
		ll = ll->next;
	}

	return ll;
}


/**
 * Search backward in a doubly linked list.
 *
 * \param ll     The doubly linked list to search in.
 * \param nskip  The number of elements to search backward.
 *
 * \return The list at the indexed position, or ERROR_DLL if the index
 *          is out of bounds.
 *
 * \sa dll_forward
 */
dll
dll_backward(dll ll, unsigned int nskip)
{
	int i;

	assert(ll != NULL);
	assert(ll != ERROR_DLL);

	for (i = 0; i < nskip; ++i) {
		/*
		 * XXX: Maybe instead of this, we should do something with
		 *      bounds checking...
		 */
		if (ll == NULL)
			return (dll)ERROR_DLL;
		ll = ll->prev;
	}

	return ll;
}


/**
 * Get the data at the current position from a doubly linked list.
 *
 * \param ll     The doubly linked list to look in.
 *
 * \return A pointer to the data
 *
 * \sa sll_get_data
 */
void *
dll_get_data(dll ll)
{
	assert(ll != NULL);
	assert(ll != ERROR_DLL);

	return ll->data;
}


#ifdef DEBUG
/**
 * Prints a dump of a doubly linked list.
 * The element data is formatted according to the supplied format string.  This
 * function is intended for testing and debug purposes only.
 *
 * \param ll   The doubly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa sll_dump
 */
void
dll_dump(dll ll, char *fmt)
{
	if (ll == ERROR_DLL) {
		printf("ERROR_DLL\n");
		return;
	}

#ifdef BOUNDS_CHECKING
	if (ll->prev != NULL)
		fprintf(stderr, "Gune: dll_dump: First element of doubly "
			"linked list has non-NULL prev pointer\n");
#endif

	for (; ll != NULL; ll = ll->next) {
#ifdef BOUNDS_CHECKING
		if (ll->prev != NULL && ll->prev->next != ll)
			fprintf(stderr, "Gune: dll_dump: Linked list is not "
				"consistent\n");
#endif
		printf(fmt, (int)ll->data);
		printf(" -> ");
	}
	printf("NULL\n");
}
#endif
