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
 * \brief Queues implementation.
 *
 * \file queue.c
 */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/queue.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a new empty queue.
 *
 * \return  A new empty queue object, or \c NULL if an error occurred.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa queue_destroy
 */
queue
queue_create(void)
{
	queue_t *q;

	if ((q = malloc(sizeof(queue_t))) == NULL)
		return NULL;

	q->head = sll_create();
	/* NOTE: This test is overkill, sll_create never returns NULL.  */
	if (q->head == NULL) {
		free(q);
		return NULL;
	}

	q->tail = q->head;
	return (queue)q;
}


/**
 * \brief Enqueue data in a queue.
 *
 * Data is put as the last element in the queue following the FIFO principle.
 *
 * \param q     The given queue.
 * \param data  The data to add to the tail of the queue.
 *
 * \return  The queue given as input, or \c NULL if out of memory.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa queue_dequeue queue_append
 */
queue
queue_enqueue(queue q, gendata data)
{
	sll new;

	assert(q != NULL);
	assert(q->head != NULL);
	assert(q->tail != NULL);

	new = sll_append_head(q->tail, data);

	if (new == NULL)
		return NULL;

	/*
	 * If the input list is not empty, sll_append_head returns the old
	 * head of the list.  Select the newly created item.
	 */
	if (!sll_empty(q->tail))
		q->tail = sll_next(new);
	else
		q->tail = new;

	/* If we were empty before, we now have a head again */
	if (sll_empty(q->head))
		q->head = q->tail;

	return q;
}


/**
 * \brief Remove the element at the head of a queue from the queue.
 *
 * \attention
 * This function logs an error at WARN_ERROR level if the queue is empty
 * (which will exit the application).
 * Therefore, always check with the queue_empty() function to see whether
 * a queue is empty or not.
 *
 * \param q  The queue to dequeue the data from.
 *
 * \return   The data dequeued from the queue.
 *
 * \sa queue_empty queue_enqueue queue_peek
 */
gendata
queue_dequeue(queue q)
{
	gendata res;

	assert(q != NULL);

	if (queue_empty(q))
		log_entry(WARN_ERROR, "Cannot dequeue from an empty queue.");

	res = (sll_get_data(q->head));

	q->head = sll_remove_head(q->head, NULL);

	/*
	 * Make sure our tail pointer doesn't cling to some old list that
	 * doesn't exist after we remove the last element from the list
	 */
	if (sll_empty(q->head))
		q->tail = q->head;

	return res;
}


/**
 * \brief Look at the element at the head of a queue without dequeueing it.
 *
 * \attention
 * This function logs an error at WARN_ERROR level if the queue is empty
 * (which will exit the application).
 * Therefore, always check with the queue_empty() function to see whether
 * a queue is empty or not.
 *
 * \param q  The queue to peek at.
 *
 * \return   The data at the head of the queue.
 *
 * \sa queue_empty queue_dequeue
 */
gendata
queue_peek(queue q)
{
	/*
	 * NOTE: Should we return NULL?  Dequeue is an illegal operation on
	 * an empty queue, but peeking isnt (?).
	 * Also, it currently is possible to enqueue NULL pointers as data,
	 * so returning NULL on an empty queue would be ambiguous.
	 * Perhaps we should do the same as with alists.
	 */
	assert(q != NULL);

	if (queue_empty(q))
		log_entry(WARN_ERROR, "Cannot peek at the head of an "
			   "empty queue.");

	assert(q->head != NULL);

	return sll_get_data(q->head);
}


/**
 * \brief Check whether a queue is empty.
 *
 * \param q  The queue to check for emptiness.
 *
 * \return   Non-zero if the queue is empty, 0 if it is not.
 */
int
queue_empty(queue q)
{
	assert(q != NULL);

	/*
	 * If the queue's head is an empty list, we sure as hell don't have
	 * a tail, so the queue is empty.  Otherwise we're not empty.
	 */
	return sll_empty(q->head);
}


/**
 * \brief Destroy a queue by deleting each element.
 *
 * The data is freed by calling the user-supplied function \p f on it.
 *
 * \attention
 * If the same data is included multiple times in the queue, the free function
 * gets called that many times.
 *
 * \param q  The queue to destroy.
 * \param f  The function which is used to free the data, or \c NULL if no
 *	       action should be taken to free the data.
 *
 * \sa queue_create
 */
void
queue_destroy(queue q, free_func f)
{
	assert(q != NULL);

	sll_destroy(q->head, f);

	free((queue_t *)q);
}


/**
 * \brief Append a queue to another queue.
 *
 * The \p base queue will be augmented with the \p rest queue, maintaining the
 * same order of base, and the same queue head.  The elements of the \p rest
 * queue maintain the same order as well.  If the last element of \p base
 * is dequeued from the new queue, the new queue head will be the current
 * queue head of \p rest.
 *
 * \param base  The queue to which the \p rest queue should be appended.
 * \param rest  The queue to append to \p base.
 *
 * \return  The new queue.
 *
 * \sa queue_enqueue
 */
queue
queue_append(queue base, queue rest)
{
	assert(base != NULL);
	assert(rest != NULL);
	assert(base->head != NULL);
	assert(rest->head != NULL);
	assert(base->tail != NULL);
	assert(rest->tail != NULL);

	/*
	 * This is necessary if rest->tail is rest->head and that would be
	 * the empty list.  We can't set base->tail to empty list if
	 * base->tail points to a nonempty list.
	 */
	if (queue_empty(rest)) {
		free(rest);
		return base;
	}

	base->head = sll_append(base->head, rest->head);
	base->tail = rest->tail;
	free(rest);

	return base;
}
