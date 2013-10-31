#!/usr/bin/env bash

opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}

build_dir=$(pwd)/_build
install_dir=$(pwd)/_install
test -d ${build_dir} && rm -fr ${build_dir}
test -d ${install_dir} && rm -fr ${install_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

ocd_manual_exe=$(which bxocd_manual)
if [ "x${ocd_manual_exe}" = "x" ]; then
    echo "ERROR: Cannot find 'bxocd_manual' executable!" >&2
    echo "ERROR: Bayeux seems not to be installed or setup !" >&2
    my_exit 1
fi
bayeux_install_dir=$(dirname $(dirname ${ocd_manual_exe}))
if [ ! -f ${bayeux_install_dir}/include/bayeux/version.h ]; then
    echo "ERROR: Cannot find Bayeux version header!" >&2
    echo "ERROR: Bayeux seems not to be installed or setup !" >&2
    my_exit 1
fi
echo "NOTICE: Found Bayeux at ${bayeux_install_dir}:" >&2

cmake \
    -DCMAKE_INSTALL_PREFIX=${install_dir} \
    -DCMAKE_FIND_ROOT_PATH:PATH=${bayeux_install_dir} \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed !" >&2
    my_exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "ERROR: Make failed !" >&2
    my_exit 1
fi
make install
if [ $? -ne 0 ]; then
    echo "ERROR: Installation failed !" >&2
    my_exit 1
fi
ls -R ${install_dir}

cd ${opwd}

export LD_LIBRARY_PATH=${install_dir}/lib:${LD_LIBRARY_PATH}

echo "NOTICE: Print OCD informations about the 'falaise::example::foo_module' class:" >&2
${ocd_manual_exe} \
  --load-dll "foo_module" \
  --action show \
  --class-id "falaise::example::foo_module"

${ocd_manual_exe} \
  --load-dll "foo_module" \
  --action skeleton \
  --class-id "falaise::example::foo_module" \
  --output-file "bar_module-skel.conf"

echo "NOTICE: Run the example program:" >&2
${install_dir}/bin/prog_foo_module

my_exit 0

# end
