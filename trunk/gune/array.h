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

/*
 * Arrays
 */
#ifndef GUNE_ARRAY_H
#define GUNE_ARRAY_H

#include <gune/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Array implementation */
typedef struct array_t {
	gendata *data;
	unsigned int size;
	unsigned int capacity;
} array_t, *array;

/** Invalid array, used as error return value */
extern array_t * const ERROR_ARRAY;

array array_create(void);
void array_destroy(array);
unsigned int array_size(array);
array array_resize(array, unsigned int);
gendata array_get_data(array, unsigned int);
array array_set_data(array, unsigned int, gendata);
array array_compact(array);
array array_grow(array, int);
array array_shrink(array, int);
array array_add(array, gendata);
array array_remove(array);

#ifdef __cplusplus
}
#endif

#endif /* GUNE_ARRAY_H */