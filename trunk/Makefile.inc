#
# $Id$
#
# Globally included Makefile for Gune.  Contains user configuration.
#

############################
# System dependent options #
############################

# Change this if necessary.  pkgsrc, for example, will set this for you.
PREFIX?=	/usr/local
LIBDIR=		${PREFIX}/lib
MANDIR=		${PREFIX}/share/man

# Treat C and lint warnings as errors.
CFLAGS+=	-g -ansi -pedantic-errors -W -Wall -Werror -Wpointer-arith
CFLAGS+=	-Wconversion -Wcast-align
LINTFLAGS+=	-w

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable bounds checking?
DEFS+=		-DBOUNDS_CHECKING

# Enable debug code?
DEFS+=		-DDEBUG
