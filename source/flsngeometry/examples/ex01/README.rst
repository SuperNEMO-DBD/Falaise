==============================
 sngeometry ``examples/ex01``
==============================

Introduction
============

This program illustrates how to setup a standalone geometry manager
within SN@ilWare.

Quick start
===========

1. Build, install and setup the ``sngeometry`` library
2. Make a copy of the example directory::

     $ cp -a [sngeometry install base directory]/share/sngeometry/examples/ex01 /tmp/ex01
     $ cd /tmp/ex01

3. Build and install the example program::

     $ mkdir __build
     $ cd __build
     $ cmake -DCMAKE_INSTALL_PREFIX=../install ..
     $ make
     $ make install

4. Run the example program :::

     $ cd ..
     $ ./install/standalone_geo
