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
 * \brief Stacks implementation.
 *
 * \file stack.c
 * Stacks implementation.
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/stack.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a new empty stack.
 *
 * \return  A new empty stack object, or \c NULL if an error occurred.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 */
stack
stack_create(void)
{
	stack_t *s;
	
	if ((s = malloc(sizeof(stack_t))) == NULL)
		return NULL;

	/* NOTE: This test is overkill, sll_create never returns NULL.  */
	if ((s->top = sll_create()) == NULL) {
		free(s);
		return NULL;
	}

	return (stack)s;
}


/**
 * \brief Pop the top element off a stack.
 *
 * \param s  The stack object to pop the element off.
 *
 * \return   The element that was popped off.
 *
 * \sa stack_peek, stack_push
 */
gendata
stack_pop(stack s)
{
	gendata res;

	assert(s != NULL);
	assert(s->top != NULL);

	if (stack_empty(s))
		log_entry(WARN_ERROR, "Cannot pop from an empty stack.");

	res = sll_get_data(s->top);
	s->top = sll_remove_head(s->top, NULL);

	return res;
}


/**
 * \brief Peek at the top element on a stack, without actually popping it.
 *
 * \param s  The stack object to peek at.
 *
 * \return   The element that is on top of the stack.
 *
 * \sa stack_pop
 */
gendata
stack_peek(stack s)
{
	assert(s != NULL);

	/*
	 * NOTE: Should we return NULL?  Pop is an illegal operation on
	 * an empty stack, but peeking isnt (?).
	 * Also, it currently is possible to push NULL pointers as data,
	 * so returning NULL on an empty stack would be ambiguous.
	 * Perhaps we should do the same as with alists.
	 */

	if (stack_empty(s))
		log_entry(WARN_ERROR, "Cannot peek at top element of an"
			   " empty stack.");

	return sll_get_data(s->top);
}


/**
 * \brief Push data onto a stack.
 *
 * \param s     The stack object to push onto.
 * \param data  The data to push onto the stack.
 * 
 * \return  The given stack s, or \c NULL in case of an error.  The old
 *	     stack is still valid in case of an error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa stack_pop
 */
stack
stack_push(stack s, gendata data)
{
	sll l;

	if ((l = sll_prepend_head(s->top, data)) == NULL)
		return NULL;

	s->top = l;
	return s;
}


/**
 * \brief Check whether a stack is empty.
 *
 * \param s  The stack to check.
 *
 * \return   Non-zero if the stack is empty, 0 if it is not.
 */
int
stack_empty(stack s)
{
	assert(s != NULL);

	return sll_empty(s->top);
}


/**
 * \brief Destroy a stack by deleting each element.
 *
 * The data is freed by calling the user-supplied function \p f on it.
 *
 * \attention
 * If the same data is included multiple times in the stack, the free function
 * gets called that many times.
 *
 * \param s  The stack to destroy.
 * \param f  The function which is used to free the data, or NULL if no action
 *	      should be taken to free the data.
 *
 * \sa stack_create
 */
void
stack_destroy(stack s, free_func f)
{
	assert(s != NULL);

	sll_destroy(s->top, f);
	free((stack_t *)s);
}
