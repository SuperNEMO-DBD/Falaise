#!/usr/bin/env bash

opwd=$(pwd)

if [ -d __install ]; then
    rm -fr __install
fi

rebuild=0


if [ "$1" = "-r" ]; then
    rebuild=1
fi
if [ ${rebuild} -eq 1 ]; then
    if [ -d __build ]; then
	rm -fr __build
    fi
fi

if [ ! -d __build ]; then
    mkdir __build
fi
cd __build

cmake -DCMAKE_INSTALL_PREFIX:PATH=${opwd}/__install ../../..
make 
make install

cd ${opwd}
find . -name "*~" -exec rm -f \{\} \;
exit 0
# end