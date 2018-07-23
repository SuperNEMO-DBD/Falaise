====================================================================
Primary event generation for simulation (snemo::demonstrator)
====================================================================

:Authors: F. Mauger, Y. Lemière
:Date:    2017-12-13

.. contents::
   :depth: 3
..

Presentation
============

This directory contains  the files needed to  describe several primary
event generators as input for SuperNEMO simulation.

  * Setup label is : ``snemo::demonstrator::simulation::primary_events``
  * Version is : ``1.3``

Files
=====

  * ``README.rst`` : this file,
  * ``manager.conf`` : the   main  configuration file of the
    genbb/decay0 manager object (from Bayeux/genbb_help),
  * ``generators/`` : Directory for primary event generators definitions.
  * ``variants/`` : Directory for variant definitions.
  * ``_tools/`` : Utilities for mainteners.

Check the configuration
=======================

First make sure the Bayeux and Falaise software is installed and setup:

.. code:: sh

   $ which bxvariant_inspector
   ...
   $ which bxgenbb_inspector
   ...
..


Browse and edit primary event generation variant parameters and options
===============================================================================

1. From the build directory, generate documentation about variants:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
	  --action doc > flprimaries.rst
   $ rst2html flprimaries.rst > flprimaries.html
   $ xdg-open flprimaries.html &
   $ rst2pdf flprimaries.rst
   $ xdg-open flprimaries.pdf &
..
or:

.. code:: sh

   $ pandoc -t latex flprimaries.rst -o flprimaries.pdf
..


2. From the build directory, browse/edit the primary event generation variant:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
          --variant-gui \
	  --variant-store "myprofile.conf"
..

List the names of the available particle generators
========================================================

Without variants, from Falaise build directory, run:

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
      --configuration "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/manager.conf" \
      --action "list" \
      --list-print-mode "raw"
..

Using variants (there is no difference here):

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
      --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
      --variant-gui \
      --configuration "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/manager.conf" \
      --action "list" \
      --list-print-mode "raw"
..

Generate 1000 primary events from the ``Bi214_Po214`` generator without variant
==================================================================================

From Falaise build directory, run:

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
      --configuration "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/manager.conf" \
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
..

Then use Root to browse the generated histograms:

.. raw:: sh

   $ root histos_Bi214_Po214.root
   ...
   root [0]
   Attaching file histos_Bi214_Po214.root as _file0...
   root [1]  TBrowser b; // Here you may browse the histograms
   root [2] .q
..

Generate 1000 primary events from the ``gamma.1MeV`` generator from the variant setup
=======================================================================================

From Falaise build directory, run:

.. raw:: sh

   $ bxgenbb_inspector \
      --datatools::resource-path "falaise@$(pwd)/BuildProducts/share/Falaise-3.1.1/resources" \
      --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
      --variant-set "primary_events:generator=gamma.1MeV" \
      --variant-gui \
      --configuration "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/manager.conf" \
      --action "shoot"  \
      --prng-seed 314159 \
      --number-of-events 1000 \
      --modulo 100 \
      --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
      --prompt \
      --title-prefix "gamma_1MeV" \
      --output-file "histos_gamma_1MeV.root"
..

Then use Root to browse the generated histograms:

.. raw:: sh

   $ root histos_gamma_1MeV.root
   ...
   root [0]
   Attaching file histos_Bi214_Po214.root as _file0...
   root [1]  TBrowser b; // Here you may browse the histograms
   root [2] .q
..

.. end

