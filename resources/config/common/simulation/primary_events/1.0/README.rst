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

Files:

  * ``README.rst`` : this file,
  * ``manager.conf``   :   the   main  configuration   file   of   the
    Bayeux/geomtools genbb/decay0 manager object,
  * ``background.def`` :  a list of generators  of radioactive decays
    considered as background sources in a DBD experiment,
  * ``calibration.def`` : a list  of generators of radioactive decays
    considered as calibration sources for the experimental setup,
  * ``dbd.def`` : a list of generators  of double beta decay for Ca48,
    Se82, Mo100 and Nd150,
  * ``misc.def`` :

List the name of the available particle generators:

  From the ``primary_events/1.0`` directory: ::

    $ cat *.def | grep ^\\\[name=\" | cut -d' ' -f1 | cut -d= -f2 | tr -d '"'

  From the Falaise source directory: ::

   $ bxgenbb_inspector \
      --datatools::resource_path "falaise@$(pwd)/resources" \
      --configuration "@falaise:config/common/simulation/primary_events/1.0/manager.conf" \
      --action "list"

Generate 1000 primary events from the ``Bi214_Po214`` generator:

  From the Falaise source directory: ::

   $ bxgenbb_inspector \
      --datatools::resource_path "falaise@$(pwd)/resources" \
      --configuration "@falaise:config/common/simulation/primary_events/1.0/manager.conf" \
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

   $ root histos_Bi214_Po214.root
   ...
   root [0]
   Attaching file histos_Bi214_Po214.root as _file0...
   root [1]  TBrowser b; // Here you may browse the histograms
   root [2] .q
