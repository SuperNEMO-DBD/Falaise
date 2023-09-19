======================================
SuperNEMO Decays configuration
======================================


Organization of the directory
================================

* ``1.4/`` : Static decay generator configuration version ``1.4``

  Contents:

  - ``DecaysManager.conf`` : Main decay generator configuration file
  - ``DecayGenerators/`` : Decay generators configurations

    + ``BackgroundSources.conf`` : Various background sources
    + ``CalibrationSources.conf`` : Various calibration sources
    + ``ConfigurableSources.conf`` : Various configurable sources

      - ``tweakable_generator`` (of type ``genbb::single_particle_generator``) : Generator with various
	tweakable parameters (particle name, fixed or ranged energy, energy spectrum, direction of emission (cone)...)
      - ``aegir``  (of type ``snemo::simulation::arbitrary_event_generator_injector``) : Generator which wraps some possibly external event generator(s) defined from non official configuration files
	
    + ``DBDSources.conf`` : Various DBD emitter sources
    + ``GenericSources.conf`` : Various generic sources
 
* ``variants/`` : Variant  decay generator configurations

  Contents:

  - ``1.0/`` : Variant  decay generator  configuration ``1.0``

    Contents:
    
    + ``GenBBVariants.conf`` : Variant decay generator registry
    + ``GenBBVariantRepository.conf`` : Variant decay generator standalone repository
    + ``*.conf`` : Various definition files for decay generator variant models and parameters
    + ``GenBB.csv`` : List of available decay generators for the variant system


Testing the decay generator
================================

The Falaise library must be built first.

.. code:: shell
   
   $ bash testing/test-1.4.bash
..
