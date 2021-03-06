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
 * \brief Linked lists interface.
 *
 * \file lists.h
 */
#ifndef GUNE_LISTS_H
#define GUNE_LISTS_H

#include <gune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Singly linked list implementation */
typedef struct sll_t {
	gendata data;		/**< The data stored at the current entry */
	struct sll_t *next;	/**< Pointer to the next entry */
} sll_t, *sll;

/* SLL creation/deletion functions */
sll sll_create(void);
void sll_destroy(sll, free_func);
unsigned int sll_count(sll);
int sll_empty(sll);

/* SLL exceptions for head */
sll sll_remove_head(sll, free_func);
sll sll_remove_next(sll, free_func);
sll sll_prepend_head(sll, gendata);
sll sll_append_head(sll, gendata);

/* Accessor functions */
gendata sll_get_data(sll);
sll sll_set_data(sll, gendata);
sll sll_forward(sll, unsigned int);
void sll_dump(sll, const char *);

/** Quick macro to go forward one item in the list */
#define sll_next(l)	(sll_forward((l), 1))

/* Convenience functions */
sll sll_append(sll, sll);
sll sll_prepend(sll, sll);


/** \brief Doubly linked list implementation */
typedef struct dll_t {
	gendata data;		/**< The data stored at the current entry */
	struct dll_t *prev;	/**< Pointer to the previous entry */
	struct dll_t *next;	/**< Pointer to the next entry */
} dll_t, *dll;

/* DLL creation/deletion functions */
dll dll_create(void);
void dll_destroy(dll, free_func);
unsigned int dll_count(dll);
int dll_empty(dll);

/* DLL exceptions for head */
dll dll_remove_head(dll, free_func);
dll dll_prepend_head(dll, gendata);
dll dll_append_head(dll, gendata);

/* Accessor functions */
gendata dll_get_data(dll);
dll dll_set_data(dll, gendata);
dll dll_forward(dll, unsigned int);
dll dll_backward(dll, unsigned int);
void dll_dump(dll, const char *);

/** Quick macro to go forward one item in the list */
#define dll_next(l)	(dll_forward((l), 1))
/** Quick macro to go backward one item in the list */
#define dll_prev(l)	(dll_backward((l), 1))

/* Convenience functions */
dll dll_append(dll, dll);
dll dll_prepend(dll, dll);

#ifdef __cplusplus
}
#endif

#endif /* GUNE_LISTS_H */
