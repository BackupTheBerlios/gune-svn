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
 * \brief Hash tables interface.
 *
 * \file ht.h
 */
#ifndef GUNE_HT_H
#define GUNE_HT_H

#include <gune/lists.h>
#include <gune/alist.h>
#include <gune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Hashing function type */
typedef unsigned int (* hash_func) (gendata, unsigned int);

/** \brief Hash table implementation */
typedef struct ht_t {
	alist *buckets;		/**< The buckets to which hash values map */
	unsigned int range;	/**< The number of buckets */
	hash_func hash;		/**< The hashing function to use */
} ht_t, *ht;

ht ht_create(unsigned int, hash_func);
void ht_destroy(ht, free_func, free_func);
ht ht_insert(ht, gendata, gendata, eq_func, free_func, free_func);
ht ht_insert_uniq(ht, gendata, gendata, eq_func);
ht ht_lookup(ht, gendata, eq_func, gendata *);
ht ht_delete(ht, gendata, eq_func, free_func, free_func);
int ht_empty(ht);
void ht_walk(ht, assoc_func, gendata);

/* Convenience functions */
ht ht_merge(ht, ht, eq_func, free_func, free_func);
ht ht_merge_uniq(ht, ht, eq_func);

#ifdef __cplusplus
}
#endif

#endif /* GUNE_HT_H */
