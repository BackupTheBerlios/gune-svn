#!/bin/sh
#
# $Id$
#
# Script for creating a release
#

PROJECT="gune"
PROJECT_UPPER="GUNE"
SHLIB_VERSION="${PROJECT}/shlib_version"

VERSION_HEADER="${PROJECT}/version.h"
VERSION_MAJOR="${PROJECT_UPPER}_MAJOR_VERSION"
VERSION_MINOR="${PROJECT_UPPER}_MINOR_VERSION"

CREATE_TAR="tar czf"
RM="rm -rf"
MAKE=make
CAT=cat
GREP=grep
SED="sed -E"
EXPR=expr
MV="mv -f"

XARGS_RM="xargs rm -rf"


usage ()
{
	echo "Usage: release.sh [options]"
	echo "Options:"
	echo "-r   Make release tarball"
	echo "-t   Create a tag in subversion"
	echo "-v   Bump minor version number (and update header)"
	echo "-V   Bump major version number (and update header)"
	echo "-H   Update header files to reflect shlib_version"
	echo "-h   Show this help"
	echo "-p   Print version numbers and repos"
	echo
	echo "release.sh will look at ${SHLIB_VERSION} for major/minor"
	echo "version numbers.  For svn, it will call svn info on the current"
	echo "working directory and grab the URL line"
}

getrepos ()
{
	REPOS=`svn info | ${GREP} URL | ${SED} "s/URL:[[:space:]]*//" | ${SED} "s,/trunk,,`
}

getversion ()
{
	MAJOR=`${GREP} major ${SHLIB_VERSION} | ${SED} "s/[^0-9]*([0-9]+)[^0-9]*/\1/"`
	MINOR=`${GREP} minor ${SHLIB_VERSION} | ${SED} "s/[^0-9]*([0-9]+)[^0-9]*/\1/"`
	VERSIONSTRING="${MAJOR}.${MINOR}"
	BASENAME="${PROJECT}-${VERSIONSTRING}"
}

bumpmajor ()
{
	getversion
	NEWMAJOR=`${EXPR} ${MAJOR} + 1`
	${SED} "s/major[[:space:]]*=[[:space:]]*[0-9]+[^0-9]*/major=${NEWMINOR}/;minor[[:space:]]*=[[:space:]]*[0-9]+[^0-9]*/minor=0/" ${SHLIB_VERSION} > ${SHLIB_VERSION}.new
	${MV} ${SHLIB_VERSION}.new ${SHLIB_VERSION}
}

bumpminor ()
{
	getversion
	NEWMINOR=`${EXPR} ${MINOR} + 1`
	${SED} "s/minor[[:space:]]*=[[:space:]]*[0-9]+[^0-9]*/minor=${NEWMINOR}/" ${SHLIB_VERSION} > ${SHLIB_VERSION}.new
	${MV} ${SHLIB_VERSION}.new ${SHLIB_VERSION}
}

updateheader ()
{
	getversion
	${SED} "s/(.*${VERSION_MAJOR}[^0-9]*)[0-9]+([^0-9]*)/\1${MAJOR}\2/;s/(.*${VERSION_MINOR}[^0-9]*)[0-9]+([^0-9]*)/\1${MINOR}\2/" ${VERSION_HEADER} > ${VERSION_HEADER}.new
	${MV} ${VERSION_HEADER}.new ${VERSION_HEADER}
}

printinfo ()
{
	getversion
	getrepos
	echo "Major: ${MAJOR}"
	echo "Minor: ${MINOR}"
	echo "Version string: ${VERSIONSTRING}"
	echo "Repos: ${REPOS}"
}

#
# Remove developer-only stuff (assumes we are in the project's dir)
#
cleanup ()
{
	${SED} "/^(#.*)?$/d" devel-files | ${XARGS_RM}
	${RM} devel-files
}

maketar ()
{
	getrepos
	getversion
	cd ..
	svn export "${REPOS}/tags/${BASENAME}" "${BASENAME}"
	${CREATE_TAR} "${BASENAME}.tar.gz" "${BASENAME}"

	cd ${BASENAME}
	${MAKE} release
	cleanup
}


createtag ()
{
	getrepos
	getversion
	svn cp "${REPOS}/trunk" "${REPOS}/tags/${BASENAME}"
}


#
# Shell main
#

# No args?  Just show usage
if [ $# -eq 0 ]
then
	usage
	exit
fi

#
# Order is important here (eg, don't printinfo before we bumpmajor/bumpminor)
#
while getopts prtvVHh opt
do
	case "${opt}" in
		t) createtag ;;
		r) maketar ;;
		v) bumpminor; updateheader ;;
		V) bumpmajor; updateheader ;;
		H) updateheader ;;
		p) printinfo ;;
		h) usage; exit ;;
		?) usage; exit ;;
	esac
done

#
# Shift arguments beyond options, so $1 will be the arg after the last option
#
shift `${EXPR} ${OPTIND} - 1`
