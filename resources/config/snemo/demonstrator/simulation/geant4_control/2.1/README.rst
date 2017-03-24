================================================================
Geant4 simulation for SuperNEMO demonstrator
================================================================

:Authors: François Mauger
:Date:    2017-01-14

.. contents::
   :depth: 3
..

Presentation
============

This directory  contains the files  needed to run Geant4 simulation
for the SuperNEMO demonstrator simulation.

* Version is : ``2.1``

This setup depends on:

* the SuperNEMO demonstrator's geometry layout (``4.0``) and its variants.
* the SuperNEMO vertex generation (``4.1``) and its variants.
* the SuperNEMO primary event generation (``1.2``) and its variants.

Files
=====

  * ``README.rst`` : This file.
  * ``manager.conf``  :  The main  configuration  file for the
  Bayeux/mctools Geant4 simulation engine.
  * ``processes/``  : This directory contains configuration files
    for physics processes.
  * ``sensitive/``  : This directory contains configuration files
    for sensitive detectors.
  * ``hit_processing/``  : This directory contains configuration files
    for step hit processing.
  * ``variants/``  : This directory contains configuration files
    for variant support.

Check the configuration
=======================

First make sure the Bayeux and Falaise softwares are installed and setup
from a (Cadfael)brew shell:

.. code:: sh

   brewsh> which bxvariant_inspector
   ...
   brewsh> which bxg4_production
   ...
..

Browse and edit primary event generation variant parameters and options
===============================================================================

1. From the build directory, generate documentation about variants:

.. code:: sh

   brewsh> bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
	  --action doc > flg4ctrl.rst
   brewsh> flg4ctrl.rst > flg4ctrl.html
   brewsh> xdg-open flg4ctrl.html &
   brewsh> rst2pdf flg4ctrl.rst
   brewsh> xdg-open flg4ctrl.pdf &
..
or:

.. code:: sh

   brewsh> pandoc -t latex flg4ctrl.rst -o flg4ctrl.pdf
..

2. From the build directory, browse/edit the primary event generation variant and generate a variant profile::

.. code:: sh

   brewsh> bxvariant_inspector \
	  --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
	  --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
          --variant-gui \
	  --variant-store "myprofile.conf"
..

Run Geant4
----------------------------------------------------------------------

Run from  the Falaise build directory.  Here we **don't** use  the variant
support but manually set vertex and particle generators associated to
the default geometry:

.. raw:: sh

   brewsh> LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxg4_production \
     --datatools::logging "warning" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --logging-priority "debug" \
     --load-dll Falaise \
     --config @falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/manager.conf \
     --vertex-generator-name "source_pads_bulk"  \
     --vertex-generator-seed 0 \
     --event-generator-name "Tl208" \
     --event-generator-seed 0  \
     --g4-manager-seed 0       \
     --shpf-seed 0             \
     --output-prng-seeds-file mc_g4_production.seeds   \
     --output-prng-states-file mc_g4_production.states \
     --batch \
     --using-time-statistics \
     --number-of-events 10 \
     --number-of-events-modulo 2 \
     --output-data-format "bank" \
     --output-data-bank "SD" \
     --output-data-file "cfg1-Tl208-source.xml"
..

Run from the Falaise build directory. Here we  **do** use  the variant
support to set the vertex  and particle generators associated to some
variant geometry layout:

.. raw:: sh

   brewsh> LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxg4_production \
     --datatools::logging "warning" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --logging-priority "debug" \
     --load-dll Falaise \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
     --variant-gui \
     --variant-store "cfg1.conf" \
     --config @falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/manager.conf \
     --vertex-generator-seed 0 \
     --event-generator-seed 0  \
     --g4-manager-seed 0       \
     --shpf-seed 0             \
     --output-prng-seeds-file mc_g4_production.seeds   \
     --output-prng-states-file mc_g4_production.states \
     --using-time-statistics \
     --number-of-events 100 \
     --number-of-events-modulo 10 \
     --output-data-format "bank" \
     --output-data-bank "SD" \
     --output-data-file "cfg1-out.xml"
..

Run in interactive mode:

.. raw:: sh

   brewsh> LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxg4_production \
     --datatools::logging "warning" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.0.0/resources" \
     --logging-priority "debug" \
     --load-dll Falaise \
     --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
     --variant-gui \
     --variant-store "cfg1.conf" \
     --config @falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/manager.conf \
     --vertex-generator-seed 0 \
     --event-generator-seed 0  \
     --g4-manager-seed 0       \
     --shpf-seed 0             \
     --output-prng-seeds-file mc_g4_production.seeds   \
     --output-prng-states-file mc_g4_production.states \
     --using-time-statistics \
     --number-of-events 100 \
     --number-of-events-modulo 10 \
     --output-data-format "bank" \
     --output-data-bank "SD" \
     --output-data-file "cfg1-source.xml" \
     --interactive \
     --g4-visu \
     --g4-macro "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/visu/visu_0.mac"
..


.. end
