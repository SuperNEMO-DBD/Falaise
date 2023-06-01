#!/usr/bin/env bash
# Author: F.Mauger 
# Date: 2022-11-07
# Description: A Bash script to clean devel Falaise installation on Ubuntu (20/22.04).

origPwd=$(pwd)
falaiseVersion=$(grep ^falaise-version= ./_setup.d/install.log | cut -d'=' -f2)
buildDir=$(grep ^build-dir= ./_setup.d/install.log | cut -d'=' -f2)
installDir=$(grep ^install-dir= ./_setup.d/install.log | cut -d'=' -f2)
setupDir=$(grep ^setup-dir= ./_setup.d/install.log | cut -d'=' -f2)
echo >&2 "[info] falaiseVersion=${falaiseVersion}"
echo >&2 "[info] About to remove the following directories :"
echo >&2 "[info]   buildDir=${buildDir}"
echo >&2 "[info]   installDir=${installDir}"
echo >&2 "[info]   setupDir=${setupDir}"

echo >&2 "Confirm (y|N) : "
read CMD

if [ -n ${CMD} -a "${CMD}" == "y" ]; then

    if [ -d ${installDir} ]; then
	echo >&2 "Removing '${installDir}'... "
	rm -fr ${installDir}
    fi
    
    if [ -d ${buildDir} ]; then
	echo >&2 "Removing '${buildDir}'... "
	rm -fr ${buildDir}
    fi
    
    if [ -d ${setupDir} ]; then
	echo >&2 "Removing '${setupDir}'... "
	rm -fr ${setupDir}
    fi
fi

cd ${origPwd}
exit 0

# end
