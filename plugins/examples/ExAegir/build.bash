#!/usr/bin/env bash

which falaise-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Falaise seems not available!"
    exit 1
fi


srcDir="$(pwd)"
buildDir="$(pwd)/_build.d"
installDir="$(pwd)/_install.d"

if [ -d ${buildDir} ]; then
    rm -fr ${buildDir}
fi
mkdir -p ${buildDir}

if [ -d ${installDir} ]; then
    rm -fr ${installDir}
fi
mkdir -p ${installDir}

falaiseDir=$(falaise-config --cmakedir)
echo >&2 "[info] Found Falaise at '${falaiseDir}'."

cd ${buildDir}

cmake -DCMAKE_INSTALL_PREFIX=${installDir} \
      -DFalaise_DIR=${falaiseDir} \
      ${srcDir}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Configuration failed!"
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo >&2 "[error] Make failed!"
    exit 1
fi

make install
if [ $? -ne 0 ]; then
    echo >&2 "[error] Installation failed!"
    exit 1
fi

tree ${installDir}


exit 0
