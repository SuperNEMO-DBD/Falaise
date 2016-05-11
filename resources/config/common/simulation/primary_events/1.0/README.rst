====================================================================
Primary event generation for SuperNEMO simulation (common resources)
====================================================================

:Authors: François Mauger
:Date:    2013-10-23

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to  describe several primary
event generators as input for SuperNEMO or BiPo3 simulation.

  * Setup label is : ``common::simulation::primary_events``
  * Version is : ``1.0``

Note: this setup is obsolete.

Files:
======

  * ``README.rst`` : this file,
  * ``manager.conf`` : the   main  configuration file of   the
    Bayeux/genbb_help genbb/decay0 manager object,
  * ``background.def`` :  a list of generators  of radioactive decays
    considered as background sources in a DBD experiment,
  * ``calibration.def`` : a list  of generators of radioactive decays
    considered as calibration sources for the experimental setup,
  * ``dbd.def`` : a list of generators  of double beta decay for Ca48,
    Se82, Mo100 and Nd150,
  * ``misc.def`` : various generators of monokinetic electrons or gammas.
