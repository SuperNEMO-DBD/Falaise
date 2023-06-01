====================================================================
SuperNEMO Demonstrator Simulation Configuration
====================================================================


Organization of the directory
================================

* ``2.4/`` : Static simulation configuration 

  Contents:
  
  * ``SimulationManager.conf``   :    Main   configuration    of   the
     Bayeux/Geant4 simulation manager
  * ``SimulationServices.conf``  : Main  configuration of  the service
    manager
  * ``processes/`` : Configuration files for Geant4's physics processes
  * ``sensitive/`` : Configuration files for Geant4's sensitive detectors
  * ``hit_processing/`` : Configuration files for hit processing
    
* ``variants/`` : Configuration 

  * ``2.0/`` : Configuration of simulation variant models and parameters

    * ``SimulationVariantRepository.conf``   :   Definition   of   a   variant
      repository  with   all  embedded   components  of   the  simulation:
      *geometry*,  *vertex  generation*,  *primary event  generation*  and
      *simulation engine*.   This mechanism describes the  various options
      (*variants*) selectable  while setting up the  demonstrator geometry
      and allows to describe use choice through specific *variant profile*
      files.
    * ``SimulationVariants.conf``  : Configuration  of the  simulation variant
      registry
    * ``SimulationVariantsGDM.conf``  : Description  of the  global dependency
      model  for variants  related to  simulation engine,  primary event
      generation, vertex generation and geometry options.
  


Testing the simulator engine
================================

The Falaise library must be built first.

.. code:: shell
   
   $ bash testing/test-2.4.bash
..


Running the simulation configuration with Bayeux tools
======================================================

.. code:: bash

   $ _falaise_prefix="`dirname $(which fltags)`/.."
   $ _falaise_resources="$(pwd)"
   $ bxg4_production \
	  --logging-priority "trace" \
	  --datatools::logging "trace" \
	  --datatools::resource-path "falaise@${_falaise_resources}" \
	  --load-dll "Falaise@${_falaise_prefix}/lib" \
	  --variant-config "@falaise:snemo/demonstrator/geant4_control/Geant4VariantRepository.conf" \
	  --variant-gui \
	  --variant-store "/tmp/_falaise_simulation.profile" \
          --batch \
          --config @falaise:snemo/demonstrator/geant4_control/Geant4Manager.conf \
          --vertex-generator-seed 0 \
          --event-generator-seed 0  \
          --g4-manager-seed 0       \
          --shpf-seed 0             \
          --output-prng-seeds-file "/tmp/mc_g4_production.seeds" \
          --output-prng-states-file "/tmp/mc_g4_production.states" \
	  --number-of-events 10 \
	  --number-of-events-modulo 1 \
	  --output-data-file "/tmp/_falaise_simulation.xml.gz" \
	  --output-data-format "bank" \
	  --output-data-bank name "SD"
..
  
