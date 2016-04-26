================================================================
Vertex generation for SuperNEMO demonstrator simulation
================================================================

:Authors: François Mauger
:Date:    2016-04-06

.. contents::
   :depth: 3
..


Presentation
============

This directory  contains the files  needed to describe  several vertex
  generators as input for SuperNEMO demonstrator simulation.

 * Version is : ``4.0``

Files
=====

  * ``README.rst`` : This file.
  * ``manager.conf``  :  The main  configuration  file  of the  Bayeux
    ``genbb::manager`` object.
  * ``generators/*_vg.lis`` : definitions of vertex generators.

Check the configuration
=======================

First make sure the Bayeux and Falaise softwares are installed and setup: ::

  $ which bxgenvtx_production
  ...

Print the list of available vertex generators
---------------------------------------------

From  Falaise build  directory (this  is preliminary),  run:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/variance.conf" \
     --datatools::variant-qt-gui \
     --list

Generate 10000 vertexes from the surface of the experimental hall roof
----------------------------------------------------------------------

Run from the Falaise build directory (preliminary):

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 100 \
     --output-file "test_vertices.txt" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/variance.conf" \
     --datatools::variant-qt-gui

Note: some options are not used here:

     --visu
     --visu-spot-zoom 2.0
     --vertex-generator "source_pads_bulk"
