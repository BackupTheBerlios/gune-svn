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
 * \file string.c
 * String manipulation
 */

#include <stdlib.h>
#include <assert.h>
#include <gune/string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Concatenate two strings in a newly allocated string.
 * Original strings are left intact and a new string is malloc()ed.
 *
 * \param a  The first part of the new string.
 * \param b  The second part of the new string.
 *
 * \return  If all went right, a pointer to the newly allocated string.
 *          Otherwise, NULL.
 */
char *
str_cat(const char *a, const char *b)
{
	size_t l;
	char *x;

	if ((x = malloc(strlen(a) + strlen(b) + 1)) == NULL)
		return NULL;
	l = strlen(a);

	/*
	 * Use memcpy for first half to avoid unnecessary checks for \0
	 * characters.
	 */
	memcpy(x, a, l);
	strcpy(x + l, b);
	return x;
}


/**
 * Copy a string, with a maximum of n characters.  Like C's strncpy, except
 *  str_n_cpy guarantees a \0 at the end of the string.
 *
 * \param src  The source string (to copy).
 * \param dst  The destination buffer (of at least len bytes).
 * \param len  The number of bytes to copy, maximum.
 *
 * \return     The destination buffer.
 *
 * \sa str_cpy
 */
char *
str_n_cpy(char *dst, const char *src, size_t len)
{
	strncpy(dst, src, len);
	*(dst + len - 1) = '\0';

	return dst;
}


/**
 * Copy a string, allocating memory for the new string.
 *
 * \param s  The string to copy.
 *
 * \return   The copy of the original string, or NULL if out of memory.
 *
 * \sa str_n_cpy
 */
char *
str_cpy(const char *s)
{
	char *x;

	if ((x = malloc(strlen(s) + 1)) == NULL)
		return NULL;
	return strcpy(x, s);
}


/**
 * Generate hash from a string.
 *
 * \param key    The string to hash.
 * \param range  The range of the hash table.
 *
 * \return  The hash of the supplied string, in the range [0..range-1].
 *
 * \sa str_eq
 */
unsigned int
str_hash(gendata key, unsigned int range)
{
	char *s = (char *)key.ptr;
	unsigned int h = 0;				/* This is variable */

	assert(s != NULL);

	/*
	 * Refer to "Performance in Practice of String Hashing Functions"
	 * by M.V. Ramakrishna & Justin Zobel for rationale on this particular
	 * hashing function.
	 */
	while (*s != '\0')
		h ^= ((h << 5) + (h >> 2) + *s++);	/* 5, 2 are variable */

	return h % range;
}


/**
 * String comparison function for hash tables.  Simply a wrapper for strcmp(3),
 *  to pick the ptr from gendata.
 *
 * \param s1  The string to compare to s2.
 * \param s2  The string to compare to s1.
 *
 * \return  An integer greater than, equal to, or less than 0, according to
 *	      whether s1 is greater than, equal to, or less than s2.  Exact
 *	      values depend on your C library's strcmp implementation.
 *
 * \sa str_hash
 */
int
str_eq(gendata s1, gendata s2)
{
	return strcmp(s1.ptr, s2.ptr);
}
