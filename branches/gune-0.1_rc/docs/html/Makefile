#
# $Id$
#
# HACK: This is unportable.  We should use the BSD Makefiles system somehow.
# bsd.doc.mk is not suitable, it tries to copy the Makefile and builds
# something called `paper.ps'.  I'm afraid we have to write our own
# `bsd.html.mk' or something for this to be nice.  (really, bsd.doc.mk should
# be updated to reflect the fact that most projects don't use postscript
# but html etc)
#

RM=rm -f

clean:
	${RM} *.html
	${RM} *.png
	${RM} *.css
	${RM} formula.repository
