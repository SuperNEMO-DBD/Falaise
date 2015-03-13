=================================
Falaise geoemtry example ``ex01``
=================================

Introduction
============

 * Description:

   This  example illustrates  how to  use the geometry manager to
   extract geometry information from the SuperNEMO demonstrator
   virtual geometry provided by Falaise.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Built object(s) :

   * ``ex01`` : the example executable linked to the ``Falaise`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Falaise library
2. Make a copy of the example directory: ::

     shell$ cp -a [Falaise-trunk]/resources/examples/geometry/ex01/ /tmp/falaise_geometry_ex01
     shell$ cd /tmp/falaise_geometry_ex01

3. Build and install the example: ::

     shell$ mkdir _build.d
     shell$ cd _build.d
     shell$ cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH="your falaise installation base dir" \
        ..
     shell$ make
     shell$ make install
     shell$ cd ..

4. Run the example: ::

     shell$ ./ex01

5. Clean: ::

     shell$ rm ex01
     shell$ rm -fr _build.d
