================================
FLsimulate (legacy) example ex01
================================

:Author: F.Mauger
:Date: 2017-03-15

This example shows how to setup FLsimulate (legacy) to produce simulation data.

The simulation  setup uses version 2.0, as  published in  the Falaise
3.0.0 resource directory to be used with the ``flsimulate_legacy`` application.
It uses an old and fragile model for variants.

**BEWARE**: fLsimulate_legacy is obsolete and fragile.

Getting help
============

.. code:: sh

   $ flsimulate_legacy --help
..


Configuration files
===================

* ``vprofile.conf`` : variant profile as generated through:

.. code:: sh

   $ bxvariant_inspector \
	  --datatools::logging="debug"  \
	  --load-dll "Falaise@$(flquery --libdir)" \
	  --datatools::resource-path="falaise@$(flquery --resourcedir)" \
	  --variant-config \
	  "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/variants/repository.conf" \
	  --variant-gui \
	  --variant-store "vprofile.conf"
..

* ``seeds.conf`` : the random generator seed file as generated from:

.. code:: sh

   $ bxg4_seeds --no-safe-checks --base-dir "." --no-run-list-file --pattern "seeds.conf"
..


Run the simulation
==================

.. code:: sh

   $ flsimulate_legacy \
	  --variant-load="vprofile.conf" \
	  --number=100 \
	  --modulo=10 \
	  --vertex-generator="source_pads_bulk" \
	  --event-generator="Se82.0nubb" \
	  --input-seeds="seeds.conf" \
	  --output-file="out.brio"
..


Visualization of output events
==============================

Display events using the SuperNEMO demonstrator geometry version 4.0 (implied by simulation setup 2.0) :

.. code:: sh

   $ flvisualize \
	  --variant-config \
	    "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
	  --variant-load "vprofile.conf" \
	  --input-file "out.brio"
..
