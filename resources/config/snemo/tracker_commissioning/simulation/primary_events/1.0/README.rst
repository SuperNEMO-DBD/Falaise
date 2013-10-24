=======================================================================
Primary event generation for SuperNEMO tracker commissioning simulation
=======================================================================

:Authors: François Mauger
:Date:    2013-10-20

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to  describe several primary
event  generators   as  input  for  SuperNEMO   tracker  commissioning
simulation.

 * Version is : ``1.0``

Files
=====

  * ``README.rst`` : this file,
  * ``manager.conf``   :   the   main  configuration   file   of   the
    Bayeux/geomtools genbb/decay0 manager object,
  * ``high_energy_cosmic_rays.def``  : a  list of  generators of  high
    energy cosmic rays,
  * ``inspector_histos_cosmic.conf``  : the  definition of  histograms
    dedicated to  the kinematics of  generated primary muons  (for the
    ``bxgenbb_inspector`` application).

List the name of the available particle generators
==================================================

From the Falaise source directory: ::

   $ bxgenbb_inspector \
      --datatools::resource_path "falaise@$(pwd)/resources" \
      --configuration "@falaise:config/snemo/tracker_commissioning/simulation/primary_events/1.0/manager.conf" \
      --action "list"
   List of particle generators:
   |-- Am241                          : genbb::wdecay0 (not initialized)
   |-- Bi207                          : genbb::wdecay0 (not initialized)
   :
   |-- electron.cosmic.500MeV         : genbb::single_particle_generator (not initialized)
   :
   |-- electron.500keV                : genbb::single_particle_generator (not initialized)
   |-- gamma.50keV                    : genbb::single_particle_generator (not initialized)
   `-- muon.cosmic.sea_level.toy      : snemo::genbb::cosmic_muon_generator (not initialized)


Generate 1000 primary events from the ``muon.cosmic.sea_level.toy`` generator
=============================================================================

Preliminary method (considering the Falaise build directory): ::

   $ cd {Falaise build directory}
   $ LD_LIBRARY_PATH=$(pwd)/BuildProducts/lib:${LD_LIBRARY_PATH} \
     bxgenbb_inspector \
      --datatools::resource_path "falaise@$(pwd)/BuildProducts/share/Falaise-1.0.0/resources" \
      --load-dll "Falaise" \
      --configuration "@falaise:config/snemo/tracker_commissioning/simulation/primary_events/1.0/manager.conf" \
      --action "shoot"  \
      --generator "muon.cosmic.sea_level.toy" \
      --prng-seed 314159 \
      --number-of-events 1000 \
      --modulo 100 \
      --histo-def "@falaise:config/snemo/tracker_commissioning/simulation/primary_events/1.0/inspector_histos_cosmic.conf" \
      --prompt \
      --delayed \
      --prompt-time-limit 1 \
      --title-prefix "Cosmic muons generator" \
      --output-file "histos_cosmic_muons.root"
   $ root histos_cosmic_muons.root
   ...
   root [0]
   Attaching file histos_cosmic_muons.root as _file0...
   root [1]  TBrowser b; // Here you may browse the histograms
   root [2] .q
   To be done...
