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

#include "lists.h"
#include "types.h"
#include "error.h"

/* Hmm... Must have created this in my sleep! */
const int list_error_ptr;

/*
 * No seriously, we need some constant pointer for defining error values
 * and allocating memory on the heap for each new type we invent.  This
 * workaround may seem ugly, but it's the only way to get the desired effect.
 */
const sll_t   * const ERROR_SLL   = (const void *)&list_error_ptr;
const dll_t   * const ERROR_DLL   = (const void *)&list_error_ptr;
const stack_t * const ERROR_STACK = (const void *)&list_error_ptr;
const queue_t * const ERROR_QUEUE = (const void *)&list_error_ptr;

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
	assert(ll != ERROR_SLL && ll != NULL);

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

	assert(ll != ERROR_SLL && ll != NULL);

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
	sll begin = ll;

	ll = ll->next;
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

	/* Allocate the new element */
	if ((new = malloc(sizeof(sll_t))) == NULL)
		return (sll)ERROR_SLL;
	new->data = data;
	new->next = ll->next;

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

	assert(ll != ERROR_SLL && ll != NULL);

	for (i = 0; i < nskip; ++i) {
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
	assert(ll != ERROR_SLL && ll != NULL);

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
	dll begin = ll;

	ll = ll->next;
	ll->prev = NULL;
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

	if ((new = malloc(sizeof(dll_t))) == NULL)
		return NULL;
	new->data = data;
	new->next = ll;
	new->prev = ll->prev;
	ll->prev = new;
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

	if ((new = malloc(sizeof(dll_t))) == NULL)
		return NULL;
	new->data = data;
	new->next = ll->next;
	new->next->prev = new;
	new->prev = ll;
	ll->next = new;
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

	assert(ll != ERROR_DLL && ll != NULL);

	for (i = 0; i < nskip; ++i) {
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

	assert(ll != ERROR_DLL && ll != NULL);

	for (i = 0; i < nskip; ++i) {
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
	assert(ll != ERROR_DLL && ll != NULL);

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new, empty, stack.
 *
 * \return  A new empty stack object, or ERROR_STACK if out of memory.
 */
stack
stack_create(void)
{
	stack_t *s;
	
	if ((s = malloc(sizeof(stack_t))) == NULL)
		return (stack)ERROR_STACK;

	/* XXX: This test is overkill, sll_create always returns NULL.  */
	if ((s->top = sll_create()) == ERROR_SLL) {
		free(s);
		return (stack)ERROR_STACK;
	}

	return (stack)s;
}


/**
 * Pop the top element off the stack and return it.
 *
 * \param s  The stack object to pop the element off.
 *
 * \return   The element that was popped off.
 *
 * \sa stack_peek, stack_push
 */
void *
stack_pop(stack s)
{
	void *res;

	assert(s != ERROR_STACK && s != NULL);
	assert(s->top != ERROR_SLL);

	if (stack_is_empty(s))
		log_entry(WARN_ERROR, "Cannot pop from an empty stack.");

	res = sll_get_data(s->top);
	s->top = sll_remove_head(s->top);

	return res;
}


/**
 * Peek at the top element on the stack and return it, without actually
 * popping it.
 *
 * \param s  The stack object to peek at.
 *
 * \return   The element that is on top of the stack.
 *
 * \sa stack_pop
 */
void *
stack_peek(stack s)
{
	assert(s != ERROR_STACK && s != NULL);

	/*
	 * NOTE: Should we return NULL?  Pop is an illegal operation on
	 * an empty stack, but peeking isnt (?).
	 * Also, it currently is possible to push NULL pointers as data,
	 * so returning NULL on an empty stack would be ambiguous.
	 */

	if (stack_is_empty(s))
		log_entry(WARN_ERROR, "Cannot peek at top element of an"
			   " empty stack.");

	return sll_get_data(s->top);
}


/**
 * Push data onto a stack.
 *
 * \param s     The stack object to push onto.
 * \param data  The data to push onto the stack.
 * 
 * \return      The given stack s.
 *
 * \sa stack_pop
 */
void
stack_push(stack s, void *data)
{
	/* XXX Must be able to signal an error if sll_prepend_head dies. */

	s->top = sll_prepend_head(s->top, data);
}


/**
 * Check whether a stack is empty.
 *
 * \param s  The stack to check.
 *
 * \return   Whether or not the stack is empty.
 */
bool
stack_is_empty(stack s)
{
	assert(s != NULL && s != ERROR_STACK);

	return sll_is_empty(s->top);
}


/**
 * Free all memory allocated for a stack. Note that the data stored
 * within the stack is NOT freed.
 *
 * \param s  The stack to destroy.
 */
void
stack_destroy(stack s)
{
	assert(s != ERROR_STACK && s != NULL);

	sll_destroy(s->top);
	free((stack_t *)s);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new, empty, queue.
 *
 * \return  A new empty queue object, or ERROR_QUEUE if out of memory.
 */
queue
queue_create(void)
{
	queue_t *q;

	if ((q = malloc(sizeof(queue_t))) == NULL)
		return (queue)ERROR_QUEUE;

	q->head = sll_create();
	if (q->head == ERROR_SLL) {
		free(q);
		return (queue)ERROR_QUEUE;
	}
		
	q->tail = q->head;
	return (queue)q;
}


/**
 * Enqueue data in the queue. Data is put as the last element in the row
 * following the FIFO principle.
 *
 * \param q     The given queue.
 * \param data  The data to add to the tail of the queue.
 *
 * \return	The queue given as input, or ERROR_QUEUE if out of memory.
 */
queue
queue_enqueue(queue q, void *data)
{
	sll new;

	assert(q != ERROR_QUEUE && q != NULL);
	assert(q->head != ERROR_SLL && q->tail != ERROR_SLL);

	new = sll_append_head(q->tail, data);

	if (new == ERROR_SLL)
		return (queue)ERROR_QUEUE;

	q->tail = new;

	/* If we were empty before, we now have a head again */
	if (sll_is_empty(q->head))
		q->head = q->tail;

	return q;
}


/**
 * Remove the element at the head of the queue from the queue. This function
 * throws an error if the queue is empty. Therefore, always check with the
 * queue_empty() function to see whether a queue is empty or not.
 *
 * \param q  The queue to dequeue the data from.
 *
 * \return   The data dequeued from the queue.
 *
 * \sa queue_empty, queue_peek
 */
void *
queue_dequeue(queue q)
{
	void *res;

	assert(q != ERROR_QUEUE && q != NULL);

	if (queue_is_empty(q))
		log_entry(WARN_ERROR, "Cannot dequeue from an empty queue.");

	res = (sll_get_data(q->head));

	q->head = sll_remove_head(q->head);

	/*
	 * Make sure our tail pointer doesn't cling to some old list that
	 * doesn't exist after we remove the last element from the list
	 */
	if (sll_is_empty(q->head))
		q->tail = q->head;

	return res;
}


/**
 * Look at the element at the head of the queue without dequeueing it.
 * This function throws an error if the queue is empty. Therefore, always
 * check with the queue_empty() function to see whether a queue is empty or
 * not.
 *
 * \param q  The queue to peek at.
 *
 * \return   The data at the head of the queue.
 *
 * \sa queue_empty, queue_dequeue
 */
void *
queue_peek(queue q)
{
	/*
	 * NOTE: Should we return NULL?  Dequeue is an illegal operation on
	 * an empty queue, but peeking isnt (?).
	 * Also, it currently is possible to enqueue NULL pointers as data,
	 * so returning NULL on an empty queue would be ambiguous.
	 */
	assert(q != ERROR_QUEUE && q != NULL);

	if (queue_is_empty(q))
		log_entry(WARN_ERROR, "Cannot peek at the head of an "
			   "empty queue.");

	assert(q->head != NULL);

	return sll_get_data(q->head);
}


/**
 * Check whether the queue is empty.
 *
 * \param q  The queue to check for emptiness.
 *
 * \return   Whether the queue is empty.
 */
bool
queue_is_empty(queue q)
{
	assert(q != ERROR_QUEUE && q != NULL);

	/*
	 * If the queue's head is an empty list, we sure as hell don't have
	 * a tail, so the queue is empty.  Otherwise we're not empty.
	 */
	return sll_is_empty(q->head);
}


/**
 * Free all memory allocated for the queue. Note that the data stored
 * within the queue is NOT freed.
 *
 * \param q  The queue to free.
 */
void
queue_destroy(queue q)
{
	assert(q != ERROR_QUEUE && q != NULL);

	sll_destroy(q->head);

	free((queue_t *)q);
}
