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
 * \file queue.c
 * Queue implementation
 */
#include <assert.h>
#include <stdlib.h>

#include <gune/queue.h>
#include <gune/error.h>

queue_t * const ERROR_QUEUE = (void *)error_dummy_func;

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
queue_enqueue(queue q, gendata data)
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
gendata
queue_dequeue(queue q)
{
	gendata res;

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
gendata
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
