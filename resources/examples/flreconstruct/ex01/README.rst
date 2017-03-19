============================
FLreconstruct example ex01
============================

:Author: F.Mauger
:Date: 2017-03-16

       This example shows how to  setup and use FLReconstruct (Falaise
       3.0)  to process  simulation  data  for SuperNEMO  demonstrator
       experiment and generate *reconstructed events*.

In the  following, we use a  reconstruction setup as published  in the
Falaise 3.0.0. This reconstruction setup  is applied to simulated data
generated   with   FLSimulate   using   the   SuperNEMO   demonstrator
experimental setup version 1.0.

This setup implies:

* Experimental setup version 1.0 (URN: "urn:snemo:demonstrator:setup:1.0"), with:

  * Variants setup (URN: "urn:snemo:demonstrator:geometry:4.0:variants")
  * Services                        setup                        (URN:
    "urn:snemo:demonstrator:geometry:4.0:services"),   with   embedded
    services:

    * Geometry version 4.0 (URN: "urn:snemo:demonstrator:geometry:4.0")


Setup
=====

The ``vprofile.conf`` variant configuration profile is extracted from
the variant profile used for the simulation, preserving only the
*geometry* part. See the ``samples/input/vprofile.conf`` profile for details.


Configuration files
===================

* ``flreconstruct.conf`` : main configuration script
* ``vprofile.conf`` : variant profile browsable by:

.. code:: sh

   $ bxvariant_inspector \
	  --datatools::logging="debug"  \
	  --load-dll "Falaise@$(flquery --libdir)" \
	  --datatools::resource-path="falaise@$(flquery --resourcedir)" \
	  --variant-config "urn:snemo:demonstrator:geometry:4.0:variants" \
	  --variant-load "vprofile.conf" \
	  --variant-gui
   ..



Input files
===========

* ``samples/input/flSD.brio`` : input data file generated from the flsimulate example ``ex01``.
* ``samples/input/flSD.meta`` : input metadata companion file (flsimulate example ``ex01``).
* ``samples/input/flSD-vprofile.conf`` : variant profile used in flsimulate example ``ex01`` from which
  is extracted the above ``vprofile.conf`` file.



Run the reconstruction
======================

Reconstruct simulated data:

.. code:: sh

   $ flreconstruct \
	  --config "flreconstruct.conf" \
	  --input-metadata-file "samples/input/flSD.meta" \
	  --input-file "samples/input/flSD.brio" \
	  --output-metadata-file "flRec.meta" \
	  --output-file "flRec.brio"
..

Here,  some   output  metadata  are   generated  and  stored   in  the
``flRec.meta``  companion file.

Event display
=============

Visualization of output Monte Carlo events:

.. code:: sh

   $ flvisualize \
	  --detector-config-file "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/manager.conf" \
	  --variant-config "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
	  --variant-load "vprofile.conf" \
	  --input-file "flRec.brio"
..

..
..     --variant-config "urn:snemo:demonstrator:geometry:4.0:variants"
..
