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
 * \brief Miscelleaneous functionality implementation.
 *
 * \file misc.c
 * Loose odds and ends which don't really belong anywhere.
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
 * \brief Pointer comparison function for hash tables and association lists.
 *
 * \param p1  The pointer to compare to \p p2.
 * \param p2  The pointer to compare to \p p1.
 *
 * \return  0 if pointer values are not equal, nonzero if they are equal.
 *
 * \sa ptr_hash, num_eq, posnum_eq, sym_eq
 */
int
ptr_eq(gendata p1, gendata p2)
{
	return (p1.ptr == p2.ptr);
}


/**
 * \brief Signed integer comparison function for hash tables and
 *	   association lists.
 *
 * \param n1  The number to compare to \p n2.
 * \param n2  The number to compare to \p n1.
 *
 * \return  0 if numbers are not equal, nonzero if they are equal.
 *
 * \sa num_hash, ptr_eq, posnum_eq, sym_eq
 */
int
num_eq(gendata n1, gendata n2)
{
	return (n1.num == n2.num);
}


/**
 * \brief Unsigned integer comparison function for hash tables and
 *	   association lists.
 *
 * \param n1  The number to compare to \p n2.
 * \param n2  The number to compare to \p n1.
 *
 * \return  0 if numbers are not equal, nonzero if they are equal.
 *
 * \sa posnum_hash, ptr_eq, num_eq, sym_eq
 */
int
posnum_eq(gendata n1, gendata n2)
{
	return (n1.posnum == n2.posnum);
}


/**
 * \brief Character comparison function for hash tables and association lists.
 *
 * \param c1  The character to compare to \p c2.
 * \param c2  The character to compare to \p c1.
 *
 * \return  0 if characters are not equal, nonzero if they are equal.
 *
 * \sa sym_hash, ptr_eq, num_eq, posnum_eq
 */
int
sym_eq(gendata c1, gendata c2)
{
	return (c1.sym == c2.sym);
}


/**
 * \brief Calculate hash from a pointer.
 *
 * The hash is calculated using just the pointer's raw integer value.
 *
 * \attention
 * Use of this function is not recommended.  If anything more is known about
 * the key's data it is highly recommended to write a more specific hashing
 * function.
 *
 * Also, the range may \b not be larger than the maximum value of a pointer
 * on the target machine.  In practice, this restriction will rarely matter.
 *
 * \param key    The pointer to hash.
 * \param range  The range of the hash table.  This should be a prime value
 *		  for this function to ensure all hash table buckets are
 *		  used equally much.
 *
 * \return  The hash of the supplied pointer, in the range
 *           \f$ [0..range-1] \f$.
 *
 * \sa ptr_eq, num_hash, posnum_hash, sym_hash
 */
unsigned int
ptr_hash(gendata key, unsigned int range)
{
	/*
	 * We don't really care if pointers aren't the same size as integers,
	 * since we're counting modulo anyway.  So the pointers can get
	 * truncated all they want.
	 */
	/* LINTED */
	unsigned int p = (unsigned int)key.ptr;

	return p % range;
}


/**
 * \brief Calculate hash from a signed integer.
 *
 * \attention
 * The range may \b not be larger than the maximum value of a signed
 * integer on the target machine.  In practice, this restriction will
 * rarely matter.
 *
 * \param key    The number to hash.
 * \param range  The range of the hash table.  This should be a prime value
 *		  for this function to ensure all hash table buckets are
 *		  used equally much.
 *
 * \return  The hash of the supplied number, in the range
 *           \f$ [0..range-1] \f$.
 *
 * \sa num_eq, ptr_hash, posnum_hash, sym_hash
 */
unsigned int
num_hash(gendata key, unsigned int range)
{
	return (unsigned int)(key.num % range);
}


/**
 * \brief Calculate hash from an unsigned integer.
 *
 * \attention
 * The range may \b not be larger than the maximum value of an unsigned
 * integer on the target machine.  In practice, this restriction will
 * rarely matter.
 *
 * \param key    The number to hash.
 * \param range  The range of the hash table.  This should be a prime value
 *		  for this function to ensure all hash table buckets are
 *		  used equally much.
 *
 * \return  The hash of the supplied number, in the range
 *           \f$ [0..range-1] \f$.
 *
 * \sa posnum_eq, ptr_hash, num_hash, sym_hash
 */
unsigned int
posnum_hash(gendata key, unsigned int range)
{
	return key.posnum % range;
}


/**
 * \brief Calculate hash from a character (symbol).
 *
 * \attention
 * The range may \b not be larger than the maximum value of a character
 * on the target machine.  In general, characters are between 0 and 255.
 *
 * \param key    The number to hash.
 * \param range  The range of the hash table.  This should be a prime value
 *		  for this function to ensure all hash table buckets are
 *		  used equally much.
 *
 * \return  The hash of the supplied number, in the range
 *           \f$ [0..range-1] \f$.
 *
 * \sa sym_eq, ptr_hash, num_hash, posnum_hash
 */
unsigned int
sym_hash(gendata key, unsigned int range)
{
	return (unsigned int)key.sym % range;
}
