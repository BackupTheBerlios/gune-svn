#
# $Id$
#
# Gune library Makefile
#

LIB=	gune
SRCS=	error.c lists.c string.c stack.c queue.c array.c ht.c alist.c	\
	misc.c
INCS=	error.h lists.h string.h stack.h queue.h array.h ht.h alist.h	\
	misc.h								\
	gune.h version.h types.h

# XXX: Not sure how portable this is beyond GCC/xlint
CFLAGS+=	-I.. ${DEFS}
LINTFLAGS+=	-I.. ${DEFS}

.PHONY: doc

# For developers who have Doxygen only
doc:
	doxygen

# Make sure includes are installed too on install target
beforeinstall:
	${INSTALL_DIR} ${INCSDIR}

afterinstall: includes

.include <bsd.lib.mk>
