# $Id$
#
# Makefile for Gune, BSD Make version.
#
# Probably the best idea is to create a wrapper Makefile for other platforms,
#  as BSD Makefile structure is extremely elegant.
#

SUBDIR=		gune test docs

.PHONY: testall doc release

testall: all
	test/test -a

doc:
	cd gune && ${MAKE} doc
	cd docs/man && ./mklinks.sh

release: doc

.include <bsd.subdir.mk>
