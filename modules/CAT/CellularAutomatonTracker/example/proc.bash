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

pwd
cat_dir=${opwd}/../devel/LocalBuild/__install/lib/cmake/CellularAutomatonTracker
echo "cat_dir=${cat_dir}"
cd ${cat_dir}
export CAT_DIR=$(pwd)
cd ${opwd}
echo "CAT_DIR=${CAT_DIR}"

cd __build
cmake -DCMAKE_INSTALL_PREFIX:PATH=${opwd}/__install ..
make
make install

cd ${opwd}

exe=__install/bin/client_example
if [ -x ${exe} ]; then
    ${exe} > plot.data
    if [ $? -ne 0 ]; then
	echo "ERROR: CAT example client program '${exe}' failed ! Abort !" 1>&2
	exit 1
    fi
    which gnuplot > /dev/null 2>&1 
    if [ $? -ne 0 ]; then
	echo "ERROR: Cannot find Gnuplot ! Abort !" 1>&2
	exit 1
    fi
    gnuplot plot.gp
fi

exit 0
# end