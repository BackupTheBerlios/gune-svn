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
 * \file error.c
 * Provisions for error reporting
 */

#include <stdarg.h>
#include <stdlib.h>
#include <gune/types.h>
#include <gune/error.h>

static FILE *logfile = stderr;

/**
 * Description strings for the different warning levels
 *
 * \sa warnlvl
 */
static const char *warnlvl_descr[NUM_WARNLVLS] = {
	"DEBUG",
	"Note",
	"Warning",
	"Critical error",
};

/* ``Hmm... Must have created this in my sleep!'' -- Gune, Titan AE
 *
 * No seriously, we need some constant pointer for defining error values.
 * The problem here is, that we want to return a pointer to `const' memory
 * in case of error, but not in the normal case.  This is of course not
 * possible in C.  Since in protected systems the program text is marked
 * read-only, we get an immediate segfault on write to this memory space.
 * This is the desired behaviour, since we do not want to erroneously
 * continue working with wrong pointers but get an error.
 */
void error_dummy_func(void) { }

/**
 * Error value for objects.  This is used when NULL is a valid value for
 * a pointer on function returns.  Used in Gune even if NULL is not a
 * valid value for consistency reasons.
 */
void * const ERROR_PTR = (void *)error_dummy_func;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Specify which file is used to send log messages to. Defaults to stderr.
 *
 * \param log  The log file to write to.  If this is NULL, nothing is changed.
 *
 * \sa log_entry
 */
void
set_logfile(FILE *log)
{
	if (log != NULL)
		logfile = log;
}


/**
 * Write a message of the specified level to the log file.
 * The default logfile is standard error (stderr).
 * This function does not return in case of an WARN_ERROR
 * it exit()s with an exit status of 1.
 *
 * \param lvl  The warning level of the log message.
 * \param msg  The message to log (no carriage return needed).
 *
 * \return 0 if everything went allright, -1 if an error occurred.
 *
 * \sa warnlvl set_logfile
 */
int
log_entry(warnlvl lvl, const char *msg, ...)
{
	int ret = 0;
	va_list ap;
	va_start(ap, msg);

	if (fprintf(logfile, warnlvl_descr[lvl]) == -1 ||
	  fprintf(logfile, ": ") == -1 || vfprintf(logfile, msg, ap) == -1 ||
	  fprintf(logfile, "\n") == -1)
		ret = -1;

	va_end(ap);

	/* Quit if the warning level is an error */
	if (lvl >= WARN_ERROR)
		exit(1);

	return ret;
}
