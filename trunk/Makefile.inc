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
HTMLDIR=	${PREFIX}/share/doc

# Uncomment this to install HTML files too (currently not implemented)
#INSTALL_HTML=yes

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable bounds checking?
DEFS+=		-DBOUNDS_CHECKING

# Enable debug code?
#DEFS+=		-DDEBUG

# Add debug symbols? (GCC specific)
#CFLAGS+=	-g

.-include "Makefile.devel"
