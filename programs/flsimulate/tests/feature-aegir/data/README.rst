===============================================================================================
Test data file for the ``snemo::simulation::from_ascii_files_event_generator`` class
===============================================================================================

Sample files of primary events from Co60 decays have been generated with:

.. code:: shell

   $ bxdecay0-run -s 314159 -n 20 -c background -N "Co60" "./genCo60_1" 
   $ bxdecay0-run -s 951413 -n 20 -c background -N "Co60" "./genCo60_2" 
..


Format of the primary event ASCII files
====================================================

.. code:: raw
	  
   Event 0

   Event 1

   Event 2

   ...

   Event N-1
..

Each event record is separated from the previous one by a blank line.


Format of an event record:
====================================================

.. code::raw

   EventId(int) EventTimeShift(real) DecayLabel(string)
   NbParticles(int)
   ParticleCode0(int) ParticleTimeShift0(real) ParticlePx0(real) ParticlePy0(real) ParticlePz0(real)
   ParticleCode1(int) ParticleTimeShift1(real) ParticlePx1(real) ParticlePy1(real) ParticlePz1(real)
   ...
   ParticleCodeK(int) ParticleTimeShiftK(real) ParticlePxK(real) ParticlePyK(real) ParticlePzK(real)
..



Types of variables and units:
-----------------------------

* Event ID is a positive integer.
* Event time shift is given in second.
* Event decay label is an unquoted character string (example: 'Co60')
* Number of particle is a positive or null integer:
* Particle code is a positive integer:

  - 1 : gamma
  - 2 : positron
  - 3 : electron
  - 47 : alpha

* Particle time shift is given in second.   
* Particle momentum coordinates are given in MeV/c.


   
.. end

