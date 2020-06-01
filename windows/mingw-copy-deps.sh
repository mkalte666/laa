#!/usr/bin/env bash
#
# Copyright: © 2015 Jeffrey Clark <https://github.com/h0tw1r3/>
# License: GPLv3 (http://www.gnu.org/licenses/gpl-3.0.html)
#
set -o errtrace

error() {
        echo "ERROR in $0 : line $1 exit code $2"
        exit $2
}
trap 'error ${LINENO} ${?}' ERR

if [[ -z $1 || ! -d $1 || ! -f $2 ]]
then
        echo "Usage: $(basename $0) SYSROOT BINARY"
        echo "Search SYSROOT for BINARY dependencies and copy to dirname(BINARY)"
        exit 1
fi

SYSROOT=$1
COPYTO=$(dirname $2)

if [[ ! -w ${COPYTO} ]]
then
        echo "Error: ${COPYTO} is not writable"
        exit 1
fi

function copydeps
{
        depfilename=$(basename "${1}")
        strings "${1}" | awk '{temp=tolower($0)} temp ~ /^([^\\ ])*\.dll$/ && !/^'${depfilename,,}'$/ { print $0 }' | while read dll
        do
                destname=${COPYTO}/$(basename "${dll}")
                [ -f "${destname}" ] && continue

                find "${SYSROOT}" -name "${dll}" -and -not -path "*win32*"| while read f
                do
                        cp -v "${f}" "${destname}" && copydeps "${f}" && break
                done
        done
}

copydeps "${2}"