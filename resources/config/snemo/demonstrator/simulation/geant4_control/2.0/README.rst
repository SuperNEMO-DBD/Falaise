================================================================
Geant4 simulation for SuperNEMO demonstrator
================================================================

:Authors: François Mauger
:Date:    2016-05-12

.. contents::
   :depth: 3
..

Presentation
============

This directory  contains the files  needed to run Geant4 simulation
for the SuperNEMO demonstrator simulation.

 * Version is : ``2.0``

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

First make sure the Bayeux and Falaise softwares are installed and setup: ::

  $ which bxg4_production
  ...

Run Geant4
----------------------------------------------------------------------

Run from the Falaise build directory:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxg4_production \
     --datatools::logging "warning" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --load-dll Falaise \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/variants/repository.conf" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "cfg1.conf" \
     --logging-priority "debug" \
     --config @falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/manager.conf \
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
     --number-of-events-modulo 10 \
     --output-data-format "bank" \
     --output-data-bank "SD" \
     --output-data-file "cfg1-Tl208-source.xml"

Run from the Falaise build directory:

.. raw:: sh

   $ LD_LIBRARY_PATH="$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH}" \
     bxg4_production \
     --datatools::logging "warning" \
     --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
     --load-dll Falaise \
     --datatools::variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/variants/repository.conf" \
     --datatools::variant-qt-gui \
     --datatools::variant-store "cfg1.conf" \
     --logging-priority "debug" \
     --config @falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/manager.conf \
     --vertex-generator-name "source_pads_bulk"  \
     --vertex-generator-seed 0 \
     --event-generator-name "Tl208" \
     --event-generator-seed 0  \
     --g4-manager-seed 0       \
     --shpf-seed 0             \
     --output-prng-seeds-file mc_g4_production.seeds   \
     --output-prng-states-file mc_g4_production.states \
     --interactive \
     --using-time-statistics \
     --number-of-events 10 \
     --number-of-events-modulo 10 \
     --output-data-format "bank" \
     --output-data-bank "SD" \
     --output-data-file "cfg1-Tl208-source.xml" \
     --g4-visu \
     --g4-macro "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/visu/visu_0.mac"
