#
# $Id$
#
# Globally included Makefile for Gune.  Contains user configuration.
#

############################
# System dependent options #
############################

# Uncomment if necessary.  pkgsrc, for example, will set this for you.
# Without pkgsrc, default is /usr
#PREFIX=		/usr/local

CFLAGS+=	-ansi -pedantic-errors -Wall

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable bounds checking?
DEFS+=		-DBOUNDS_CHECKING

# Enable debug code?
DEFS+=		-DDEBUG
