#!/usr/bin/env bash

which flquery
if [ $? -ne 0 ]; then
    echo "error: Falaise is not installed! Abort!" >&2
    exit 1
fi

opwd=$(pwd)

source_dir=${opwd}
build_dir=${opwd}/_build.d
install_dir=${opwd}/_install.d

if [ -d ${build_dir} ]; then
    rm -fr ${build_dir}
fi
mkdir -p ${build_dir}

cd ${build_dir}
cmake \
    -DFalaise_DIR=$(flquery --prefix) \
    ${source_dir}
# cmake -DCMAKE_INSTALL_PREFIX=${install_dir} \
#     -DFalaise_DIR=$(flquery --prefix) \
#     ${source_dir}
if  [ $? -ne 0 ]; then
    echo "error: CMake configuration failed! Abort!" >&2
    cd ${opwd}
    exit 1
fi
make
if  [ $? -ne 0 ]; then
    echo "error: Build configuration failed! Abort!" >&2
    cd ${opwd}
    exit 1
fi

flsimulate -n 1 --experiment demonstrator -x source_pads_bulk -e Se82.0nubb -o test.xml
if  [ $? -ne 0 ]; then
    echo "error: flsimulate failed! Abort!" >&2
    cd ${opwd}
    exit 1
fi

echo "notice: Dump 'SD' bank head..." >&2
ls -l test.xml
echo "--------------------------------------------------------" >&2
grep -B 4 -A 10 SD test.xml >&2
echo "--------------------------------------------------------" >&2
echo "" >&2

flreconstruct -i test.xml -p ${source_dir}/AccessThingsPipeline.conf -o test-reco.xml
if  [ $? -ne 0 ]; then
    echo "error: flreconstruct failed! Abort!" >&2
    cd ${opwd}
    exit 1
fi


ls -l test-reco.xml

echo "notice: Dump 'ATMCounter' bank head..." >&2
ls -l test-reco.xml
echo "--------------------------------------------------------" >&2
grep -B 4 -A 21 ATMCounter test-reco.xml >&2
echo "--------------------------------------------------------" >&2
echo "" >&2

cd ${opwd}

rm -fr ${build_dir}

exit 0
