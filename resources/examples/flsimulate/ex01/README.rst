============================
FLsimulate example ex01
============================

:Author: F.Mauger
:Date: 2017-03-16

       This example  shows how  to setup  and use  FLSimulate (Falaise
       3.0)  to produce  simulation  data  for SuperNEMo  demonstrator
       experiment.

In  the following,  we  use  the Geant4  based  simulation setup  uses
version 2.1 as published in the Falaise 3.0.0 resource directory.

This setup implies:

* Geometry version 4.0 (URN: "urn:snemo:demonstrator:geometry:4.0")
* Vertex generation version 4.1 (URN: "urn:snemo:demonstrator:simulation:vertexes:4.1")
* Event generation version 1.2 (URN: "urn:snemo:demonstrator:simulation:decays:1.2")
* Geant4 simulation setup (URN:   "urn:snemo:demonstrator:simulation:2.1"), with:

    * Variants setup (URN: "urn:snemo:demonstrator:simulation:2.1:variants")
    * Services setup (URN: "urn:snemo:demonstrator:simulation:2.1:services")


Setup
=====

See the ``vprofile.conf`` variant configuration profile below:

* Geometry setup: *basic* geometry layout **without** magnetic field
* Vertex: randomized  uniformly from  the bulk  volume of  the tracker
  field wires (generator name: ``field_wire_bulk``)
* Primary events: \ :sup:`208`\ Tl decays (generator name: ``Tl208``)
* Simulation engine:  standard electro-magnetic interactions  model and
  detailed output profile (with *visu track* hits, this implies huge storage)

Simulation run
==============

See the ``flsimulate.conf`` main configuration script:

* 100 generated events

Configuration files
===================

* ``flsimulate.conf`` : main configuration file adapted from the output of:

.. code:: sh

   $ flsimulate --help-scripting
..

* ``vprofile.conf`` : variant profile as generated through:

.. code:: sh

   $ bxvariant_inspector \
	  --datatools::logging="debug"  \
	  --load-dll "Falaise@$(flquery --libdir)" \
	  --datatools::resource-path="falaise@$(flquery --resourcedir)" \
	  --variant-config \
	    "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
	  --variant-gui \
	  --variant-store "vprofile.conf"
..

* ``seeds.conf`` : the random generator seed file as generated from:

.. code:: sh

   $ bxg4_seeds --no-safe-checks --base-dir "." --no-run-list-file --pattern "seeds.conf"
..


Run the simulation
=====================

Generate simulated data:

.. code:: sh

   $ flsimulate \
	  --config "flsimulate.conf" \
	  --output-metadata-file "flSD.meta" \
	  --output-file "flSD.brio"
..


Visualization of output Monte Carlo events:

.. code:: sh

   $ flvisualize \
	  --variant-config "urn:snemo:demonstrator:simulation:2.1:variants" \
	  --variant-load "vprofile.conf" \
	  --input-file "flSD.brio"
..
