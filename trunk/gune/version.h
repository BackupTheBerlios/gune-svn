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
 * \brief Version macros.
 *
 * \file version.h
 * Definitions for Gune's version number and version convenience macros.
 */
#ifndef GUNE_VERSION_H
#define GUNE_VERSION_H

/** \brief Gune's minor version number */
#define GUNE_MINOR_VERSION	

/** \brief Gune's major version number
 */
#define GUNE_MAJOR_VERSION	

/**
 * \brief Gune's long version number
 *
 * In the format major, underscore, minor.  Example: 1_0.
 *
 * \hideinitializer
 */
#define GUNE_VERSION		GEN_VERSION(GUNE_MAJOR_VERSION, GUNE_MINOR_VERSION)

/**
 * \brief Gune's version, as a string.
 *
 * \hideinitializer
 */
#define GUNE_VERSION_STRING	\
		GEN_VERSION_STR(GUNE_MAJOR_VERSION, GUNE_MINOR_VERSION)

/**
 * \brief Generate a preprocessor definition from the major version number
 * and the minor version number.
 *
 * The major/minor version numbers can both be proprocessor definitions.
 *
 * Example:
 * \code
 * #define MYAPP_MAJOR		1
 * #define MYAPP_MINOR		0
 * #define MYAPP_VERSION		GEN_VERSION(MYAPP_MAJOR, MYAPP_MINOR)
 * \endcode
 * is identical to:
 * \code
 * #define MYAPP_MAJOR		1
 * #define MYAPP_MINOR		0
 * #define MYAPP_VERSION		1_0
 * \endcode
 * This is especially useful when passing minor and major versions around
 * with \c make to the C compiler.
 *
 * \param a  The major version number
 * \param b  The minor version number
 *
 * \hideinitializer
 */
#define GEN_VERSION(a, b)	GEN_VERSION_INTERN(a, b)

/*
 * We need this extra step in case a and b are macro definitions.
 * Example: GEN_VERSION_INTERN(MAJOR, MINOR) gives us `MAJOR_MINOR', but
 * instead we want something that looks like `1_0'.
 */
#define GEN_VERSION_INTERN(a, b)	a##_##b


/**
 * \brief Get the string value of a token.
 *
 * Example:
 * \code
 * PP_STR(1) => "1"
 * \endcode
 *
 * \param s  The string to get the value of.
 *
 * \hideinitializer
 */
#define PP_STR(s)			PP_STR_INTERN(s)

/* Again, we need internal PP_STR if the parameter is a macro definition. */
#define PP_STR_INTERN(a)		#a

/**
 * \brief Generate a version string from two integers using the preprocessor.
 *
 * Example:
 * \code
 * GEN_VERSION_STR(1, 0)	==> ("1.0")
 * \endcode
 *
 * \param a  The major version number
 * \param b  The minor version number
 *
 * \hideinitializer
 */
#define GEN_VERSION_STR(a, b)		(PP_STR(a) "." PP_STR(b))

#endif /* GUNE_VERSION_H */
