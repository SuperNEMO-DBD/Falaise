================================================================
Vertex generation for SuperNEMO demonstrator simulation
================================================================

:Authors: François Mauger
	  Guillaume Oliviéro
:Date:    2016-04-06

.. contents::
   :depth: 3
..


Presentation
============

This directory  contains the files  needed to describe  several vertex
  generators as input for SuperNEMO demonstrator simulation.

 * Version is : ``4.1``

Files
=====

  * ``README.rst`` : This file.
  * ``manager.conf``  :  The main  configuration  file  of the  Bayeux
    ``genbb::manager`` object.
  * ``generators/`` : Directory for vertex generators definitions.
  * ``variants/`` : Directory for variant definitions.

Check the configuration
=======================

First make sure the Bayeux and Falaise softwares are installed and setup: ::

  $ which bxgenvtx_production
  ...

Print the list of available vertex generators
---------------------------------------------

From  Falaise build  directory (this  is preliminary),  run:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-qt-gui \
     --list

Generate 10000 vertexes from the surface of the experimental hall roof
----------------------------------------------------------------------

Run from the Falaise build directory (preliminary):

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 1000 \
     --vertex-modulo 100 \
     --output-file "vertices.txt" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "profile.conf" \
     --vertex-generator "source_pads_bulk" \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta" \
     --visu-output-file "vertices-visu-dd.data.gz"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-load "profile.conf"
   geomtools> ldd vtx vertices-visu-dd.data.gz
   geomtools> G --with-category source_submodule
   List of available GIDs :
   [1100:0] as 'source_submodule'
   geomtools> display -yz [1100:0]
   ...
   geomtools> q


Vertex generator from calibration source with basic layout:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 500 \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-set "geometry:layout=Basic" \
     --datatools::variant-set "geometry:layout/if_basic/source_calibration=true" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "calib_profile.rep" \
     --vertex-generator "source_calibration_all_spots" \
     --output-file "calib_vertices.txt" \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "calib_vertices-visu-dd.data.gz"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-load "calib_profile.rep"
   geomtools> ldd vtx calib_vertices-visu-dd.data.gz
   geomtools> G --with-category source_submodule
   List of available GIDs :
   [1100:0] as 'source_submodule'
   geomtools> display -yz [1100:0]


Vertex generator with half-commissioning layout:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-set "geometry:layout=HalfCommissioning" \
     --datatools::variant-set "vertexes.commissioning:type=SingleSlot" \
     --datatools::variant-set "vertexes.commissioning:type/if_single_slot/column=48" \
     --datatools::variant-set "vertexes.commissioning:type/if_single_slot/row=1" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "hc_profile.rep" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 100 \
     --vertex-modulo 20 \
     --output-file "hc_vertices.txt" \
     --vertex-generator "commissioning.single_spot" \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "hc_vertices-visu-dd.data.gz"

Other available generator in half-commissioning layout:

.. raw:: sh

     --vertex-generator "commissioning.all_spots"

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf" \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.0/variants/repository.conf" \
     --datatools::variant-load "hc_profile.rep"
   geomtools> ldd vtx hc_vertices-visu-dd.data.gz
   geomtools> G --with-category commissioning_source_plane
   List of available GIDs :
   [1500:0] as 'source_submodule'
   geomtools> display -yz [1500:0]
