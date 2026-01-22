#!/usr/bin/bash

thisPwd=$(pwd)
which falaise-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Falaise is not setup"
    cd ${thisPwd}
    exit 1
fi

buildDir="./_build.d"

rm -fr ${buildDir}
mkdir ${buildDir}
cd ${buildDir}

cmake ..
make

cp flgeom ${thisPwd}/

cd ${thisPwd}
exit 0
