=======================
FLsimulate example ex00
=======================

:Author: F.Mauger
:Date: 2017-03-24

       This example  shows how  to setup  and use  FLSimulate (Falaise
       3.0)  to produce  simulation  data  for SuperNEMO  demonstrator
       experiment.

In the following,  we use the default simulation setup  version 2.1 as
published in the Falaise 3.0.0  resource directory and registered with
tag ``urn:snemo:demonstrator:simulation:2.1``.

In principle,  users should  specify the simulation  setup to  be used
through  its official  tag,  represented  by an  URN  registered in  a
database  internal to  Falaise. In  this example,  the default  tag is
automatically selected.


The simulation setup tagged ``urn:snemo:demonstrator:simulation:2.1`` implies:

* Experimental setup 1.0 (tag: ``urn:snemo:demonstrator:setup:1.0``), with:

  * Geometry version 4.0 (tag: ``urn:snemo:demonstrator:geometry:4.0``)

* Vertex generation version 4.1 (tag: ``urn:snemo:demonstrator:simulation:vertexes:4.1``)
* Event generation version 1.2 (tag: ``urn:snemo:demonstrator:simulation:decays:1.2``)
* Geant4 simulation setup (tag:   ``urn:snemo:demonstrator:simulation:2.1``), with:

    * Variant setup (tag: ``urn:snemo:demonstrator:simulation:2.1:variants``)

      * Default variant profile (tag: ``urn:snemo:demonstrator:simulation:2.1:variants:profiles:default``)

    * Services setup (tag: ``urn:snemo:demonstrator:simulation:2.1:services``)

These  tags are  automatically resolved,  if needed,  to configuration
file paths  by the Bayeux/datatools'  kernel and a  dedicated embedded
*URN query service*.


Setup
=====

Some  variant  parameters are  tweaked  through  an embedded  *variant
service*.   Here,   a  default  variant  *profile*   is  automatically
selected.   It  applies  specific configuration  values  for  geometry
parameters, vertex and decay generators...

You      can      check      the     default      variant      profile
(``urn:snemo:demonstrator:simulation:2.1:variants``) at:

``@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/profiles/default.profile``

It consists in:

* Geometry  setup:  *basic*  geometry layout  with  enriched  Selenium
  source and **with** uniform magnetic field (25 gauss)
* Vertex generation: randomized uniformly from  the bulk volume of all
  source pads (generator name: ``source_pads_bulk``)
* Primary  events (decay)  generation:  \ :sup:`82`\  Se 0nubb  decays
  (generator name: ``Se82.0nubb``)
* Simulation engine: standard  electro-magnetic interactions model and
  no detailed output profile (**without** *visu track* hits)

Simulation run
==============

See the ``flsimulate.conf`` main configuration script:

* 10 generated events

Configuration files
===================

The configuration of the simulation  run consists in the following file:

* ``flsimulate.conf``  : main  configuration script  adapted from  the
  output of:

.. code:: sh

   $ flsimulate --help-scripting
..


Run the simulation
==================

Generate simulated data:

.. code:: sh

   $ flsimulate \
	  --config "flsimulate.conf" \
	  --output-metadata-file "flSD.meta" \
	  --output-file "flSD.brio"
..

Here some output metadata  are generated and automatically stored
within the  output file (``flSD.brio``) which  contains the generated
Monte Carlo events (``SD`` bank).

Note also the ``--output-metadata-file`` switch which stores the metadata in
the ``flSD.meta`` companion file.



Event display
=============

Visualization of output Monte Carlo events:

.. code:: sh

   $ flvisualize --input-file "flSD.brio"
..
