#!/bin/sh
#
# $Id$
#
# Shell/sed script to create MLINKS list from header files
# This functionality really belongs in Doxygen, but that is still under
# development.
#

SRCDIR="../../gune"

# Relative from SRCDIR
TARGET="../docs/man/man3/Makefile.inc"

cd $SRCDIR

echo "MAN = \\" > ${TARGET}

for FILE in *.h
do

MAN=${FILE}.3

echo "	${MAN} \\" >> ${TARGET}

done

# Empty line because last line has a \
echo "" >> ${TARGET}

#
# Now, add links
#
echo "MKLINKS = \\" >> ${TARGET}

for FILE in *.h
do

MAN=${FILE}.3

sed -E "
s/^[[:space:]]*[[:alpha:]_][[:alnum:]_]*[[:space:]]+([[:alpha:]_][[:alnum:]_]*)[[:space:]]*\\([[:alpha:]_].*\\)[[:space:]]*;[[:space:]]*\$/	${MAN}	\\1	\\\\/
t
d" ${FILE} >> ${TARGET}

done

# Empty line because last line has a \
echo "" >> ${TARGET}
