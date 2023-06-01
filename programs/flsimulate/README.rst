==============================
Falaise Simulation Application
==============================

:Author: F.Mauger
:Date: 2022-10-27

What's Here
===========

The main application for the simulation of the SuperNEMO detector.
Any other code that is specific to the simulation - e.g. digitization.

Status
======

This  implementation of  FLsimulate  uses  the Bayeux/mctools'  Geant4
simulation    engine   through    a   dedicated    simulation   module
(mctools::g4::simulation_module) and  writes the simulated  event data
to a  file using  an output  module (dpp::output_module).   The output
file can then be processed by the FLreconstruct application.

Usage
======

Online help is available through:

.. code:: sh

   $ flsimulate --help
..

Variant support
===============

FLsimulate automatically  configures a  *variance service*  adapted to
the  experimental  setup.  For  now  only  the  ``snemo:demonstrator``
experiment is supported and has its specific variant configuration.

The variant  service allows  to select various  *tweakable* parameters
like:

- some geometry options (magnetic field, calibration sources...)
- the vertex generator (which may depends on some geometry options)
- the   primary   event  generator   (which   may   depends  on   some
  geometry/vertexes options)
- some parameters  specific to the simulation  itself (output profile,
  physics list...)

The  variant service  loads a  *variant  profile* file  which must  be
prepared     by    the     user    through     a    dedicated     tool
(``flsimulate-configure``).   This file  will  be  reused in  following
steps  of the  reconstruction/analysis  chain.  It  must be  preserved
carefully. It is  also possible to provide the *variant  profile* as a
list  of variant  parameter setting  rules in  the main  configuration
script (see below).


Scripting FLsimulate
====================

FLsimulate needs  to be provided  a human readable script  that setups
specific parameters for the simulation run. This configuration scripts
uses the ``datatools::multi_properties`` format.

Online help about the format of the script is available through:

.. code:: sh

   $ flsimulate --help-scripting
..


Examples
========


Configuration
---------------


Running ``flsimulate-configure`` to build a variant profile to feed the simulation.

Generating a variant profile from a default set of variant parameters:

.. code:: shell

   $ flsimulate-configure -M -H -o my_simu.profile
   $ cat my_simu.profile
..


Simulation
-------------

TBD

