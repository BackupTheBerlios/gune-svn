/*
 * $Id$
 *
 * Copyright (c) 2003 Peter Bex and Vincent Driessen
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
 * THIS SOFTWARE IS PROVIDED BY PETER BEX AND VINCENT DRESSEN AND CONTRIBUTORS
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gune/gune.h>

#define DEFNUM			100
#define DEFLOOPCOUNT		10
#define COMPACTISE_MODULO	7

void
strcat_tester(char *s)
{
	/* Try it */
	printf("'%s'\n", str_cat(s, s));
}


void
err_tester(warnlvl wrn)
{
	/* How lame, eh? */
	log_entry(wrn, "This is a test message...");
}


void
stress_test_array(int amt)
{
	array arr; /* matey! */
	gendata x;
	int i;

	arr = array_create();
	assert(array_size(arr) == 0);

	printf("Adding %d items to an array...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		arr = array_add(arr, x);
		assert(x.num == array_get_data(arr, (unsigned int)i).num);
		assert(array_size(arr) == (unsigned int)(i + 1));
	}

	printf("Shrinking an array of %d items...\n", amt);
	for (i = amt - 1; i >= 0; --i) {
		assert(array_get_data(arr, (unsigned int)i).num == i);
		arr = array_remove(arr);
		assert(array_size(arr) == (unsigned int)i);
	}

	printf("Filling a pre-grown array of %d items...\n", amt);
	array_grow(arr, amt);
	assert(array_size(arr) == (unsigned int)amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		arr = array_set_data(arr, (unsigned int)i, x);
		assert(x.num == array_get_data(arr, (unsigned int)i).num);
	}

	printf("Shrinking an array of %d items...\n", amt);
	for (i = amt - 1; i >= 0; --i) {
		assert(array_get_data(arr, (unsigned int)i).num == i);
		arr = array_shrink(arr, 1);

		assert(array_size(arr) == (unsigned int)i);
		/* Compactise at funky points */
		if ((i % COMPACTISE_MODULO) == 0) {
			arr = array_compact(arr);
			/* Compactising should not change size */
			assert(array_size(arr) == (unsigned int)i);
		}
	}

	array_destroy(arr, NULL);
}


void
stress_test_sll(int amt)
{
	sll l;
	gendata x, y;
	int i;

	l = sll_create();
	printf("Filling a sll with 2 * %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		l = sll_prepend_head(l, x);
		assert(!sll_empty(l));
		l = sll_append_head(l, x);
		assert(!sll_empty(l));
	}
	assert(sll_count(l) == (unsigned int)(2 * amt));

	printf("Removing 2 * %d items from the sll...\n", amt);
	for (i = 0; i < amt; ++i) {
		x = sll_get_data(sll_next(l));
		assert(!sll_empty(l));
		l = sll_remove_next(l, NULL);
		y = sll_get_data(l);
		assert(!sll_empty(l));
		l = sll_remove_head(l, NULL);

		/*
		 * We have to count backwards in this check, since we're
		 * using the list like a stack, prepending all the time.
		 */
		assert(x.num == amt - i - 1);
		assert(x.num == y.num);
	}
	assert(sll_empty(l));
	sll_destroy(l, NULL);
}


void
stress_test_dll(int amt)
{
	dll l;
	gendata x, y;
	int i;

	l = dll_create();
	assert(dll_empty(l));

	printf("Filling a dll with 2 * %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		l = dll_prepend_head(l, x);
		assert(!dll_empty(l));
		l = dll_append_head(l, x);
		assert(!dll_empty(l));
	}

	/* Do some funky inserting at a `random' position. */
	dll_append_head(dll_forward(l, 1), x);
	/* assert(x.num == dll_get_data(dll_forward(l, 1)).num); */
	dll_remove_head(dll_forward(l, 2), NULL);

	assert(dll_count(l) == (unsigned int)(2 * amt));

	printf("Removing 2 * %d items from the dll...\n", amt);
	for (i = 0; i < amt; ++i) {
		assert(!dll_empty(l));
		x = dll_get_data(l);
		l = dll_remove_head(l, NULL);
		assert(!dll_empty(l));
		y = dll_get_data(l);
		l = dll_remove_head(l, NULL);

		/*
		 * We have to count backwards in this check, since we're
		 * using the list like a stack, prepending all the time.
		 */
		assert(x.num == amt - i - 1);
		assert(x.num == y.num);
	}
	assert(dll_empty(l));
	dll_destroy(l, NULL);
}


void
stress_test_stack(int amt)
{
	int i;
	gendata x, y;
	stack s;
	
	s = stack_create();
	assert(stack_empty(s));

	/* Fill it up, and remove items again */
	printf("Filling a stack with %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		stack_push(s, x);
		assert(x.num == stack_peek(s).num);
		assert(!stack_empty(s));
	}
	printf("Popping/peeking stack of %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		assert(!stack_empty(s));
		x = stack_peek(s);
		y = stack_pop(s);

		assert(x.num == amt - i - 1);
		assert(x.num == y.num);
	}

	assert(stack_empty(s));

	/* Now, again, fill up the stack... */
	printf("Refilling a stack with %d items...\n", amt);
	for (i = 0; i < amt; ++i)
		stack_push(s, x);

	assert(!stack_empty(s));

	printf("Destroying a stack with %d items...\n", amt);
	/* ...and free it */
	stack_destroy(s, NULL);
}


void
stress_test_queue(int amt)
{
	int i;
	queue q;
	gendata x, y;

	q = queue_create();
	assert(queue_empty(q));

	/* Fill the queue */
	printf("Enqueueing a queue with %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		q = queue_enqueue(q, x);
		assert(!queue_empty(q));
	}
	printf("Dequeueing a queue with %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		assert(!queue_empty(q));
		x = queue_peek(q);
		y = queue_dequeue(q);
		assert(x.num == y.num);
		assert(x.num == i);
	}
	assert(queue_empty(q));

	printf("Re-enqueueing a queue with %d items...\n", amt);
	for (i = 0; i < amt; ++i) {
		x.num = i;
		q = queue_enqueue(q, x);
		assert(!queue_empty(q));
	}
	printf("Destroying a queue with %d items...\n", amt);
	queue_destroy(q, NULL);
}


void
usage(void)
{
	printf("usage: test [-a] [-n num] [-l log] [-s amt | -e lvl | "
		"-d amt | -q amt | -r amt | -S amt]\n");
	printf("-n num	Perform selected tests `num' times. Default is 10.\n");
	printf("-a      Perform every test, using %i for `amt'\n", DEFNUM);
	printf("-s amt	Do a stack stress test on `amt' stack items.\n");
	printf("-d amt  Do a Doubly Linked List (dll) stress test.\n");
	printf("-q amt  Do a queue stress test.\n");
	printf("-r amt  Do an array stress test.\n");
	printf("-S amt  Do a Singly Linked List (sll) stress test.\n");
	printf("-e lvl  Print an error on the specified level (0-3).\n");
	printf("-l log  Use log as a file to write messages to.\n");
	printf("-c str  Test string copy routines.\n");
	printf("-v      Gune version we are testing.\n");
}


int
main(int argc, char **argv)
{
	extern char *optarg;
	char *str = NULL;
	int ch;
	extern char *malloc_options;
	int i, loop, stack_test, queue_test, dll_test, sll_test, err_test;
	int strcat_test, array_test, idle;

	warnlvl wrn = WARN_NOTIFY;

	/*
	 * Set malloc options to init memory to zero and always have realloc
	 * actually reallocate memory.  Also, all warnings become fatal.
	 * Alloc of size 0 will always return NULL.  Always dump core on
	 * errors in malloc family functions.
	 */
	malloc_options = "ZAVX";

	/* Default options */
	stack_test = dll_test = err_test = strcat_test = queue_test = 0;
	array_test = sll_test = 0;
	loop = DEFLOOPCOUNT;
	idle = 1;	/* Set idle, unless a test should be run */

	if (argc <= 1) {
		usage();
		return 1;
	}

	while ((ch = getopt(argc, argv, "ac:d:e:l:n:q:r:s:S:v")) != -1)
		switch ((char)ch) {
			case 'a':
				dll_test = stack_test = queue_test = DEFNUM;
				array_test = sll_test = DEFNUM;
				idle = 0;
				break;
			case 'c':
				strcat_test = 1;
				str = optarg;
				idle = 0;
				break;
			case 'd':
				dll_test = atoi(optarg);
				idle = 0;
				break;
			case 'e':
				if (atoi(optarg) >= 0 &&
				    atoi(optarg) < NUM_WARNLVLS)
					wrn = atoi(optarg);
				else {
					fprintf(stderr,
						"Please specifify a "
						"number between 0 and 4.\n");
					exit(1);
				}
				idle = 0;
				err_test = 1;
				break;
			case 'l':
				set_logfile(fopen(optarg, "a"));
				break;
			case 'n':
				loop = atoi(optarg);
				break;
			case 'q':
				queue_test = atoi(optarg);
				idle = 0;
				break;
			case 'r':
				array_test = atoi(optarg);
				idle = 0;
				break;
			case 's':
				stack_test = atoi(optarg);
				idle = 0;
				break;
			case 'S':
				sll_test = atoi(optarg);
				idle = 0;
				break;
			case 'v':
				printf("Using the following Gune version...\n");
				printf("Preprocessor value:     \t%s\n",
					PP_STR(GUNE_VERSION));
				printf("Human-readable notation:\t%s\n",
					GUNE_VERSION_STRING);
				return 0;
			default:
				usage();
				return 1;
		}

	/* Perform `n' tests of each kind */
	if (idle == 0)
		for (i = 0; i < loop; ++i) {
			printf("========= RUNNING TEST %d =========\n", i + 1);
			if (dll_test > 0) {
				printf("\n----> DLL <----\n");
				stress_test_dll(dll_test);
			}
			if (stack_test > 0) {
				printf("\n----> STACK <----\n");
				stress_test_stack(stack_test);
			}
			if (err_test > 0) {
				printf("\n----> ERR <----\n");
				err_tester(wrn);
			}
			if (queue_test > 0) {
				printf("\n----> QUEUE <----\n");
				stress_test_queue(queue_test);
			}
			if (array_test > 0) {
				printf("\n----> ARRAY <----\n");
				stress_test_array(array_test);
			}
			if (strcat_test > 0) {
				printf("\n----> STR_CAT <----\n");
				strcat_tester(str);
			}
			if (sll_test > 0) {
				printf("\n----> SLL <----\n");
				stress_test_sll(sll_test);
			}
			printf("\n");
		}

	printf("Done\n");

	return 0;
}
