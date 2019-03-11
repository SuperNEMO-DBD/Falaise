======================================
BiPo3 simulation control configuration
======================================

:Authors: François Mauger, Xavier Garrido
:Date:    2013-10-24

.. contents::
   :depth: 3
..


Presentation
============

This  directory contains  the files  needed  to run  the Geant4  based
simulation of the BiPo3 module.

 * Setup label is : ``bipo3::simulation::control``
 * Setup version is : ``1.0``


Files
=====

 * ``README.rst`` : This file.
 * ``manager.conf``   :   the   main   configuration   file   of   the
   Bayeux/mctools Geant4 simulation engine.
 * ``em.conf`` : The configuration file for electro-magnetic processes.
 * ``particles.conf``  : The  configuration file  of Geant4  particles
   used within the simulation.
 * ``step_hit_processor_factory.conf`` : The configuration of the *step
   hit processors* used to construct the simulated output data.
 * ``geant4_visualization.mac`` : Geant4 macro with visualization commands.


Usage
=====

 1. First make sure the Bayeux software is installed and setup with Geant support: ::

      $ which bxg4_production
      ...

 2. Build Falaise from a build directory of your choice.

 3. From  the Falaise  build  directory  (preliminary), simulate  1000
    Tl-208 decay events from the  ``source_bulk`` in the BiPo3 virtual
    geometry setup. Interactive mode with Geant4 online display and no
    output data file: ::

      $ export BIPOMC_WORK_DIR=/tmp/${USER}/bipo3_simul.d
      $ mkdir -p ${BIPOMC_WORK_DIR}
      $ LD_LIBRARY_PATH=$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH} \
        bxg4_production \
         --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-1.0.0/resources" \
         --load-dll Falaise       \
         --interactive            \
         --g4-visu \
         --vertex-generator-name "source_bulk"  \
         --vertex-generator-seed 0 \
         --event-generator-name "Tl208"  \
         --event-generator-seed 0 \
         --g4-manager-seed 0      \
         --shpf-seed 0            \
         --config "@falaise:config/bipo3/simulation/control/1.0/manager.conf" \
         --g4-macro "@falaise:config/bipo3/simulation/control/1.0/geant4_visualization.mac"
      Idle> /run/beamOn 10
      Idle> exit

 4. Simulate 1000 Tl-208 decay events  from the ``source_bulk`` in the
    BiPo3 virtual geometry setup. Batch run with output data file: ::

      $ export BIPOMC_WORK_DIR=/tmp/${USER}/bipo3_simul.d
      $ mkdir -p ${BIPOMC_WORK_DIR}
      $ LD_LIBRARY_PATH=$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH} \
        bxg4_production \
         --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-1.0.0/resources" \
         --load-dll Falaise       \
         --batch                  \
         --using-time-statistics  \
         --vertex-generator-name "source_bulk"  \
         --vertex-generator-seed 0 \
         --event-generator-name "Tl208"  \
         --event-generator-seed 0 \
         --g4-manager-seed 0      \
         --shpf-seed 0            \
         --output-prng-seeds-file "${BIPOMC_WORK_DIR}/g4_production.seeds"   \
         --output-prng-states-file "${BIPOMC_WORK_DIR}/g4_production.states" \
         --number-of-events 1000    \
         --number-of-events-modulo 100 \
         --config "@falaise:config/bipo3/simulation/control/1.0/manager.conf" \
         --output-data-file "${BIPOMC_WORK_DIR}/g4_production_0.brio" \
         > ${BIPOMC_WORK_DIR}/g4_production.log 2>&1

    Browse the working directory: ::

      $ tree --charset iso-8859-15 ${BIPOMC_WORK_DIR}
      /tmp/snemo/bipo3_simul.d
      |-- g4_production_0.brio
      |-- g4_production.log
      |-- g4_production.seeds
      |-- g4_production.states
      `-- g4_production.states.~backup~

    Check the output simulated data file: ::

      $ ls -l ${BIPOMC_WORK_DIR}/g4_production_0.brio
      -rw-r--r-- 1 snemo snemo 6428779 oct.  23 23:57 /tmp/snemo/bipo3_simul.d/g4_production_0.brio
