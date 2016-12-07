============================
FLsimulate example ex01
============================

:Author: F.Mauger
:Date: 2016-12-07

TODO: flsimulate_next temporary stands for the flsimulate program.

This example shows how to setup FLsimulate to produce simulation data.

The simulation  setup uses  version 2.1, as  published in  the Falaise
resource directory.

Setup
=====

* Geometry setup: basic without magnetic field
* Primary events: \ :sup:`208`\ Tl decays
* Vertex: randomized uniformly from the bulk volume of the tracker field wires
* Simulation engine: use detailed output profile (with *visu track* hits, use huge storage)

Simulation run
==============

* 100 generated events

Configuration files
===================

* ``flsimulate.conf`` : main configuration file adapted from the output of:

.. code:: sh

   $ flsimulate_next --help-scripting
..

* ``vprofile.conf`` : variant profile as generated through:

.. code:: sh

   $ bxvariant_inspector \
   --datatools::logging="debug"  \
   --load-dll "Falaise@$(flquery --libdir)" \
   --datatools::resource-path="falaise@$(flquery --resourcedir)" \
   --variant-config \
     "@falaise:config/snemo/demonstrator/simulation/geant4_control/
         2.1/variants/repository.conf" \
   --variant-gui \
   --variant-store "vprofile.conf"
..

* ``seeds.conf`` : the random generator seed file as generated from:

.. code:: sh

   $ bxg4_seeds --pattern "seeds.conf"
..


Run the simulation
=====================

Generate simulated data:

.. code:: sh

   $ flsimulate_next --config "flsimulate.conf" --output-file "flSD.brio"
..


Visualization of output events:

.. code:: sh

   $ flvisualize \
   --variant-config \
     "@falaise:config/snemo/demonstrator/simulation/geant4_control/
        2.1/variants/repository.conf" \
   --variant-load "vprofile.conf" \
   --input-file "flSD.brio"
..
