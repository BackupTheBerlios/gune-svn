/*
 * $Id: string.c 60 2004-10-24 17:25:28Z sjamaan $
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
 * \file misc.c
 * Miscellaneous functions
 */

#include <gune/misc.h>

/* ``Hmm... Must have created this in my sleep!'' -- Gune, Titan AE
 *
 * No seriously, we need a constant pointer that can be used in many spots.
 * The problem here is, that we want to return a pointer to `const' memory
 * in some cases, while we want a nonconst pointer in most cases.
 * This is of course not possible in C.  Since in protected systems the
 * program text is marked read-only, we get an immediate segfault on write
 * to this memory space (with NULL too, but NULL is used to signal error).
 * This is the desired behaviour, since we do not want to erroneously
 * continue working with wrong pointers but get an error.
 */
void constant_ptr_dummy_func(void) { }

/**
 * Constant pointer.  This can be used when NULL already has another meaning
 * for a pointer.  For example, it is used as a list terminator, while NULL
 * means an error for the list.
 */
void * const CONST_PTR = (void *)constant_ptr_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Pointer comparison function for hash tables and association lists.
 *
 * \param p1  The pointer to compare to p2.
 * \param p2  The pointer to compare to p1.
 *
 * \return  0 if pointer string is not equal, 1 if it is equal.
 */
int
ptr_eq(gendata p1, gendata p2)
{
	return (p1.ptr == p2.ptr);
}


/**
 * Generate hash from a pointer, using just the pointer's raw integer value.
 * Use of this function is not recommended.  If anything more is known about
 *  the key's data it is highly recommended to write a more specific hashing
 *  function.
 *
 * \param key    The pointer to hash.
 * \param range  The range of the hash table.  This should be a prime value
 *		  for this function to ensure all hash table buckets are
 *		  used equally much.
 *
 * \return  The hash of the supplied pointer, in the range [0..range-1].
 *
 * \sa ptr_eq
 */
unsigned int
ptr_hash(gendata key, unsigned int range)
{
	/*
	 * We don't really care if pointers aren't the same size as integers,
	 * since we're counting modulo anyway.  So the pointers can get
	 * truncated all they want.
	 */
	unsigned int p = (unsigned int)key.ptr;

	return p % range;
}
