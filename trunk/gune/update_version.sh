#!/bin/sh
#
# $Id$
#
# Update the version number everywhere from the shlib_version file
#

major=`grep major shlib_version | sed -E "s/.*([0-9]).*/\1/"`
minor=`grep minor shlib_version | sed -E "s/.*([0-9]).*/\1/"`

echo ${major}.${minor}
