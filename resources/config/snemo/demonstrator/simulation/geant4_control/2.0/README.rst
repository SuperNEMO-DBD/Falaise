================================================================
Geant4 simulation for SuperNEMO demonstrator
================================================================

:Authors: François Mauger
:Date:    2016-05-12

.. contents::
   :depth: 3
..

Presentation
============

This directory  contains the files  needed to run Geant4 simulation
for the SuperNEMO demonstrator simulation.

 * Version is : ``2.0``

Files
=====

  * ``README.rst`` : This file.
  * ``manager.conf``  :  The main  configuration  file for the
  Bayeux/mctools Geant4 simulation engine.
  * ``processes/``  : The directory that contains configuration files
    for physics processes
  * ``sensitive/``  : The directory that contains configuration files
    for sensitive detectors
  * ``hit_processing/``  : The directory that contains configuration files
    for step hit processing
  * ``variants/``  : The directory that contains configuration files
    for variant support

Check the configuration
=======================

First make sure the Bayeux and Falaise softwares are installed and setup: ::

  $ which bxg4_production
  ...

Run Geant4
----------------------------------------------------------------------

Run from the Falaise build directory:

.. raw:: sh

   xxx
