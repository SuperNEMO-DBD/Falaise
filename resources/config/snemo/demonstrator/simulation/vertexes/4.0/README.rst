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
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
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
     --number-of-vertices 10000 \
     --output-file "vertices.txt" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "profile.rep" \
     --vertex-generator "source_pads_bulk" \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta" \
     --visu-output-file "vertices-visu-dd.data.gz"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-load "profile.rep"
   geomtools> ldd vtx vertices-visu-dd.data.gz
   geomtools> G --with-category source_submodule
   List of available GIDs :
   [1100:0] as 'source_submodule'
   geomtools> display -yz [1100:0]


Vertex generator from calibration source with basic layout:

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
     --number-of-vertices 1000 \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-set "demonstrator:layout=Basic" \
     --datatools::variant-set "source_calibration:active=true" \
     --datatools::variant-store "calib_profile.rep" \
     --vertex-generator "source_calibration_all_spots" \
     --output-file "calib_vertices.txt" \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "calib_vertices-visu-dd.data.gz"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-load "calib_profile.rep"
   geomtools> ldd vtx calib_vertices-visu-dd.data.gz
   geomtools> G --with-category commissioning_source_plane
   List of available GIDs :
   [1500:0] as 'source_submodule'
   geomtools> display -yz [1500:0]


Vertex generator with half-commissioning layout:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-set "demonstrator:layout=HalfCommissioning" \
     --datatools::variant-set "commissioning_source:column=48" \
     --datatools::variant-set "commissioning_source:row=1" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "hc_profile.rep" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 1000 \
     --output-file "hc_vertices.txt" \
     --vertex-generator "single_commissioning_spot" \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "hc_vertices-visu-dd.data.gz"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/variance.conf" \
     --datatools::variant-load "hc_profile.rep"
   geomtools> ldd vtx hc_vertices-visu-dd.data.gz
   geomtools> G --with-category commissioning_source_plane
   List of available GIDs :
   [1500:0] as 'source_submodule'
   geomtools> display -yz [1500:0]

Other available generator in half-commissioning layout:


.. raw:: sh
      --vertex-generator "all_commissioning_spots"
