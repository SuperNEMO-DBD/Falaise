================================================================
Vertex generation for SuperNEMO demonstrator simulation
================================================================

:Authors: François Mauger
:Date:    2017-01-07

.. contents::
   :depth: 3
..


Presentation
============

This directory  contains the files  needed to describe  several vertex
generators as input for  SuperNEMO demonstrator simulation. This setup
depends on the SuperNEMO  demonstrator's geometry layout (``4.0``) and
its variants.

 * Version is : ``4.1``

Files
=====

  * ``README.rst`` : This file.
  * ``manager.conf``  :  The main  configuration  file  of the  Bayeux
    ``genvtx::manager`` object.
  * ``generators/`` : Directory for vertex generators definitions.
  * ``variants/`` : Directory for variant definitions.
  * ``_tools/`` : Utilities for mainteners.

Check the configuration
=======================

First make sure the Bayeux and Falaise software is installed and setup: ::

.. code:: sh

   $ which bxvariant_inspector
   ...
   $ which bxgenvtx_production
   ...
..

Browse and edit primary event generation variant parameters and options
===============================================================================

1. From the build directory, generate documentation about variants:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
	  --action doc > flvertexes.rst
   $ rst2html flvertexes.rst > flvertexes.html
   $ xdg-open flvertexes.html &
   $ rst2pdf flvertexes.rst
   $ xdg-open flvertexes.pdf &
..
or:

.. code:: sh

   $ pandoc -t latex flvertexes.rst -o flvertexes.pdf
..

3. From the build directory, browse/edit the primary event generation variant:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
          --variant-gui \
	  --variant-store "myprofile.conf"
..

4. From the build directory, generate a variant profile:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
	  --variant-store "myprofile.conf"
..

Print the list of available vertex generators
---------------------------------------------

From  the Falaise build  directory,  run:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
   bxgenvtx_production \
	 --logging "fatal" \
	 --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
	 --load-dll Falaise \
	 --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
	 --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
	 --variant-config           "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
	 --variant-gui \
	 --list

Generate 10000 vertexes from the surface of the experimental hall roof
----------------------------------------------------------------------

Run from the Falaise build directory.
Here we don't use the variant support:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --load-dll Falaise \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 100 \
     --output-file "vertices.txt" \
     --vertex-generator "experimental_hall_roof" \
     --visu \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta" \
     --visu-output-file "vertices-visu-dd.data.gz"
..

With variant support, we do not specify the vertex generator from the command line
with the ``--vertex-generator`` switch. Here we use the variant GUI interface:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --load-dll Falaise \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 100 \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-gui \
     --variant-store "profile.conf" \
     --visu \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta"
..

Generate 10000 vertexes from the internal surface of the shielding walls
---------------------------------------------------------------------------------

Run from the Falaise build directory. The ``bxgenvtx_production`` forces
the use of a vertex generator from the bulk volume of the external source pads,
regardless of the default generator specified by the variant repository:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --load-dll Falaise \
     --geometry-manager "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 100 \
     --vertex-generator "source_pads_external_bulk" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --visu \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta" \
     --visu-object "[1100:0]"
..

Generate 10000 vertexes from the internal surface of the shielding walls
---------------------------------------------------------------------------------

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --load-dll Falaise \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo    100 \
     --variant-config   "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-set      "geometry:layout/if_basic/shielding=true"  \
     --vertex-generator "shielding_all_internal_surfaces" \
     --visu \
     --visu-spot-zoom   2.0 \
     --visu-spot-color  "magenta"
..


Generate 10000 vertexes from the bulk of the shielding walls
---------------------------------------------------------------------------------

Run from the Falaise build directory (preliminary):

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --load-dll Falaise \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 100 \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-set "geometry:layout/if_basic/shielding=true"  \
     --variant-set "vertexes:generator=shielding_left_right_bulk"  \
     --visu \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta"
..


Generate 10000 vertexes from the source pads bulk volume
----------------------------------------------------------------------

Run from the Falaise build directory (preliminary):

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "fatal" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 100 \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-store "profile.conf" \
     --vertex-generator "source_pads_bulk" \
     --visu \
     --visu-object "[1100:0]" \
     --visu-spot-zoom 2.0 \
     --visu-spot-color "magenta" \
     --visu-output-file "vertices-visu-dd.data.gz"
..

Display the geometry with the set of previously generated vertexes:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-load "profile.conf"
   geomtools> ldd vtx vertices-visu-dd.data.gz
   geomtools> G --with-category source_submodule
   List of available GIDs :
   [1100:0] as 'source_submodule'
   geomtools> display -yz [1100:0]
   ...
   geomtools> q
..

Vertex generator from calibration source with basic layout:
----------------------------------------------------------------------

Use the *single spot* vertex generator:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-set "geometry:layout=Basic" \
     --variant-set "geometry:layout/if_basic/source_calibration=true" \
     --variant-set "vertexes:generator=source_calibration_single_spot" \
     --variant-set "vertexes:generator/if_source_calibration_single_spot/track=3" \
     --variant-set "vertexes:generator/if_source_calibration_single_spot/position=1" \
     --variant-store "calib_profile.rep" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 500 \
     --visu \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "calib_vertices-visu-dd.data.gz" \
     --visu-object "[1100:0]" \
     --output-file "calib_vertices.csv"
..

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-load "calib_profile.rep"
   geomtools> ldd vtx calib_vertices-visu-dd.data.gz
   geomtools> G --with-category source_submodule
   List of available GIDs :
   [1100:0] as 'source_submodule'
   geomtools> display -yz [1100:0]
..


Vertex generator with half-commissioning layout:
----------------------------------------------------------------------

Visualize the geometry only:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-set "geometry:layout=HalfCommissioning"
   geomtools> display
   geomtools> quit
..

Shoot vertexes from a single commissioning source:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgenvtx_production \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-set "geometry:layout=HalfCommissioning" \
     --variant-set "vertexes:generator=commissioning_single_spot" \
     --variant-set "vertexes:generator/if_half_commissioning_single_spot/column=48" \
     --variant-set "vertexes:generator/if_half_commissioning_single_spot/row=1" \
     --variant-gui \
     --variant-store "hc_profile.rep" \
     --geometry-manager         "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --vertex-generator-manager "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/manager.conf" \
     --shoot \
     --prng-seed 314159 \
     --number-of-vertices 10000 \
     --vertex-modulo 20 \
     --visu-spot-zoom 2.0 \
     --visu-spot-size "0.05 mm" \
     --visu-spot-color "red" \
     --visu-output-file "hc_vertices-visu-dd.data.gz"
..


Other available generator in half-commissioning layout: ``commissioning_all_spots``.

Visualize:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxgeomtools_inspector \
     --logging "warning" \
     --load-dll Falaise \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --manager-config           "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf" \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/vertexes/4.1/variants/repository.conf" \
     --variant-load "hc_profile.rep"
   geomtools> ldd vtx hc_vertices-visu-dd.data.gz
   geomtools> G --with-category commissioning_source_plane
   List of available GIDs :
   [1500:0] as 'commissioning_source_plane'
   geomtools> display -yz [1500:0]
..




Prepare the list of supported vertex generators for the variant system
---------------------------------------------------------------------------------

Extract the list of supported vertex generator from definition files (``generators/*.lis``)
and store it in the ``variants/models/vertexes_generators.csv`` file with group association:

.. raw:: sh

   $ ./_tools/_prepare_csv.sh
   $ cat variants/models/vertexes_generators.csv
..

Print the number of available generators:

.. raw:: sh

   $ wc -l variants/models/vertexes_generators.csv
..

Print the list of groups of vertex generators:

.. raw:: sh

   $ cat variants/models/vertexes_generators.csv | cut -d ':' -f3 | sort | uniq
..


.. END.
