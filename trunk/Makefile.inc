#
# $Id$
#
# Globally included Makefile for Gune.  Contains user configuration.
#

############################
# System dependent options #
############################

# Uncomment if necessary.  pkgsrc, for example, will set this for you.
PREFIX?=		/usr/local

# Treat C and lint warnings as errors.
CFLAGS+=	-g -ansi -pedantic-errors -Wall -Werror
LINTFLAGS+=	-w

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable bounds checking?
DEFS+=		-DBOUNDS_CHECKING

# Enable debug code?
DEFS+=		-DDEBUG

#
# Check this
#
DESTDIR?=	${PREFIX}
LIBDIR?=	/lib
BINDIR?=	/bin
