==============================
Falaise Simulation Application
==============================

:Author: F.Mauger
:Date: 2017-01-14

What's Here
===========

The main application for the simulation of the SuperNEMO detector.
Any other code that is specific to the simulation - e.g. digitization.

TODO (2016-12-07, FM): replace the former ``flsimulatemain`` program by the new
``flsimulatemain_next``.

Status
======

This implementation  of FLsimulate uses the  mctools/Geant4 simulation
engine      through      a       dedicated      simulation      module
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
the experimental  setup. For now only  the ``demonstrator`` experiment
is supported and has its specific variant configuration.

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
(``bxvariant_inspector``).   This file  will  be  reused in  following
steps  of the  reconstruction/analysis  chain.  It  must be  preserved
carefully.

Scripting FLsimulate
====================

FLsimulate needs  to be  provided a human  readable script  that setup
specific parameters for the simulation run. This configuration scripts
uses the ``datatools::multi_properties`` format.

Online help about the format of the script is available through:

.. code:: sh

   $ flsimulate_next --help-scripting
..


Examples
========

The  ``{Falaise-source-dir}/resources/examples/flsimulate/ex01/`` directory contains
a documented sample setup from which users can derive their own simulation setup.
