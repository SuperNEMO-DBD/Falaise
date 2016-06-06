====================================================================
Primary event generation for simulation (common)
====================================================================

:Authors: F. Mauger, Y. Lemière, G. Oliviéro
:Date:    2016-05-11

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to  describe several primary
event generators as input for SuperNEMO or BiPo3 simulation.

  * Setup label is : ``common::simulation::primary_events``
  * Version is : ``1.1``

Files:
======

  * ``README.rst`` : this file,
  * ``manager.conf`` : the   main  configuration file of   the
    genbb/decay0 manager object (from Bayeux/genbb_help),

List the names of the available particle generators:
========================================================

From  Falaise build  directory,  run:

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
      --configuration "@falaise:config/common/simulation/primary_events/1.1/manager.conf" \
      --action "list" \
      --list-print-mode "raw"


Generate 1000 primary events from the ``Bi214_Po214`` generator:
======================================================================

From  Falaise build  directory,  run:

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-2.1.0/resources" \
      --configuration "@falaise:config/common/simulation/primary_events/1.1/manager.conf" \
      --action "shoot"  \
      --generator "Bi214_Po214" \
      --prng-seed 314159 \
      --number-of-events 1000 \
      --modulo 100 \
      --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
      --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
      --prompt \
      --delayed \
      --prompt-time-limit 1 \
      --title-prefix "Bi214_Po214" \
      --output-file "histos_Bi214_Po214.root"

Then use Root to browse the generated histograms: ::

.. raw:: sh

   $ root histos_Bi214_Po214.root
   ...
   root [0]
   Attaching file histos_Bi214_Po214.root as _file0...
   root [1]  TBrowser b; // Here you may browse the histograms
   root [2] .q
