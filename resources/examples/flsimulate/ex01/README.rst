=======================
FLsimulate example ex01
=======================

:Author: F.Mauger
:Date: 2017-03-18

       This example  shows how  to setup  and use  FLSimulate (Falaise
       3.0)  to produce  simulation  data  for SuperNEMO  demonstrator
       experiment.

In the following, we use simulation  setup version 2.1 as published in
the  Falaise   3.0.0  resource  directory  and   registered  with  URN
``urn:snemo:demonstrator:simulation:2.1``.

Users  should specify  the simulation  setup  to be  used through  its
official tag, represented by an  URN registered in a database internal
to Falaise.


The simulation setup tagged ``urn:snemo:demonstrator:simulation:2.1`` implies:

* Experimental setup 1.0 (URN: ``urn:snemo:demonstrator:setup:1.0``), with:

  * Geometry version 4.0 (URN: ``urn:snemo:demonstrator:geometry:4.0``)

* Vertex generation version 4.1 (URN: ``urn:snemo:demonstrator:simulation:vertexes:4.1``)
* Event generation version 1.2 (URN: ``urn:snemo:demonstrator:simulation:decays:1.2``)
* Geant4 simulation setup (URN:   ``urn:snemo:demonstrator:simulation:2.1``), with:

    * Variants setup (URN: ``urn:snemo:demonstrator:simulation:2.1:variants``)
    * Services setup (URN: ``urn:snemo:demonstrator:simulation:2.1:services``)

These  tags are  automatically resolved,  if needed,  to configuration
file paths by a dedicated embedded *URN query service*.


Setup
=====

Some  variant  parameters are  tweaked  through  an embedded  *variant
service*.  A variant  *profile* must be provided by the  user to apply
configuration values different from the default ones.

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

* 10 generated events

Configuration files
===================

The configuration of the simulation  run consists in the following set
of files:

* ``flsimulate.conf``  : main  configuration script  adapted from  the
  output of:

.. code:: sh

   $ flsimulate --help-scripting
..

* ``vprofile.conf`` : variant profile as generated through the variant
  inspector/editor application:

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

* ``seeds.conf`` : this files contains  the seeds needed by the pseudo
  random  generators  used by  the  simulation  engine. This  file  is
  generated from:

.. code:: sh

   $ bxg4_seeds --no-safe-checks --base-dir "." --no-run-list-file --pattern "seeds.conf"
..


Run the simulation
==================

Generate simulated data:

.. code:: sh

   $ flsimulate \
	  --config "flsimulate.conf" \
	  --output-metadata-file "flSD.meta" \
	  --embedded-metadata=1 \
	  --output-file "flSD.brio"
..

Here,  some   output  metadata  are   generated  and  stored   in  the
``flSD.meta``  companion file.   The ``--embedded-metadata``  requests
that  the  metadata are  also  stored  within  the output  file  which
contains the generated Monte Carlo events (``SD`` bank).

Event display
=============

Visualization of output Monte Carlo events:

.. code:: sh

   $ flvisualize \
	  --detector-config-file "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/manager.conf" \
	  --variant-config "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
	  --variant-load "vprofile.conf" \
	  --input-file "flSD.brio"
..
