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
 * \brief Miscellaneous functionality interface.
 *
 * \file misc.h
 * Loose odds and ends which don't really belong anywhere.
 */
#ifndef GUNE_MISC_H
#define GUNE_MISC_H

#include <gune/types.h>

/**
 * Swap two variables' contents.
 * \note
 * This only works for basic types, and the variables must be of the same type.
 *
 * \param t  The type of the variables.
 * \param a  The first variable.
 * \param b  The second variable.
 *
 * \hideinitializer
 */
#define SWAP(t,a,b)	{ t tmp = (a); (a) = (b); (b) = tmp;}

/**
 * Determine the maximum of two variables.
 * \note
 * This only works for basic types.
 *
 * \param a  The first variable.
 * \param b  The second variable.
 *
 * \return   The maximum of \p a and \p b, or \p b if they're equal.
 *
 * \hideinitializer
 */
#define MAX(a,b)	((a) > (b) ? (a) : (b))

/**
 * Determine the minimum of two variables.
 * \note
 * This only works for basic types.
 *
 * \param a  The first variable.
 * \param b  The second variable.
 *
 * \return   The minimum of \p a and \p b, or \p b if they're equal.
 *
 * \hideinitializer
 */
#define MIN(a,b)	((a) < (b) ? (a) : (b))

int ptr_eq(gendata, gendata);
int num_eq(gendata, gendata);
int posnum_eq(gendata, gendata);
int sym_eq(gendata, gendata);
unsigned int ptr_hash(gendata, unsigned int);
unsigned int num_hash(gendata, unsigned int);
unsigned int posnum_hash(gendata, unsigned int);
unsigned int sym_hash(gendata, unsigned int);

extern void * const CONST_PTR;

#endif /* GUNE_MISC_H */
