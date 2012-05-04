#!/usr/bin/env bash

opwd=$(pwd)

if [ -d __install ]; then
    rm -fr __install
fi

rebuild=0
cat_minimal=1
while [ -n "$1" ]; do
  if [ "x$1" = "x-r" ]; then
      rebuild=1
  elif [ "x$1" = "x-D" ]; then
      cat_minimal=0
  fi
  shift 1
done

if [ ${rebuild} -eq 1 ]; then
    if [ -d __build ]; then
	rm -fr __build
    fi
fi

if [ ! -d __build ]; then
    mkdir __build
fi
cd __build

cmake -DCMAKE_INSTALL_PREFIX:PATH=${opwd}/__install -DCAT_MINIMAL_BUILD=${cat_minimal} ../../..
make 
make install

cd ${opwd}
find . -name "*~" -exec rm -f \{\} \;
exit 0
# end