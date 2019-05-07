#!/bin/bash
# Wrap configure/build/test steps on Travis-CI into a script

# Set(ings):
# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# Find ourselves
SELFDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECTDIR="$(dirname "${SELFDIR}")"

# On Linux, check for brew'd GCC
if [ `uname` == "Linux" ] ; then
  export CC=gcc-7
  export CXX=g++-7
  export FC=gfortran-7
fi

# Create build directory, deleting if present
rm -Rf build && mkdir build
cd build

# Configure, assuming presence of Ninja tool
cmake -DCMAKE_PREFIX_PATH="$(brew --prefix);$(brew --prefix qt5-base)" -DFALAISE_ENABLE_TESTING=ON -GNinja $PROJECTDIR
# Build using Ninja to auto-parallelize
ninja
# Run tests - rerunning any that fail in verbose mode
ctest || ctest -VV --rerun-failed

