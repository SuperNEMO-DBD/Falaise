VisuToy Falaise module
======================

Introduction
------------

This  example shows  how to  build a  ``flreconstruct`` pipeline  that
processes the SuperNEMO data (SD and  CD banks from the simulation and
reconstruction output)  and display the  event using a  simple Gnuplot
based visualization driver.

Configuration file(s):

  * ``config/pipeline.conf``   :    the   ``flreconstruct``   pipeline
    configuration  files that  describes the  chain of  modules to  be
    processed.

Usage
-----

  1. First setup the Falaise software.

  2. Check the geometry of the SuperNEMO demonstrator (not mandatory): ::

      $ bxgeomtools_inspector \
          --datatools::logging "warning" \
          --load-dll Falaise@${FALAISE_INSTALL_DIR}/lib/x86_64-linux-gnu \
          --datatools::resource-path "falaise@${FALAISE_INSTALL_DIR}/share/Falaise-1.0.0/resources" \
          --manager-config "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf"

  3. Generate a set of simulated events: ::

      $ flsimulate \
        --experiment "demonstrator" \
        --vertex-generator "source_strips_bulk" \
        --event-generator  "Se82.0nubb" \
	--number 10 \
	--output-file "Se82_0nubb-source_strips_bulk_SD.brio"

  4. Run the visualization pipeline: ::

      $ flreconstruct \
        --input-file "Se82_0nubb-source_strips_bulk_SD.brio" \
        --pipeline "config/mc_visu_pipeline.conf"

  5. Run a reconstruction pipeline to fill the CD bank: ::

      $ flreconstruct \
        --input-file "Se82_0nubb-source_strips_bulk_SD.brio" \
        --pipeline "config/mc_recons_cd_pipeline.conf" \
        --output-file "Se82_0nubb-source_strips_bulk_SD-CD.brio"

     Then visualize it: ::

      $ flreconstruct \
        --input-file "Se82_0nubb-source_strips_bulk_SD-CD.brio" \
        --pipeline "config/mc_visu_pipeline.conf"

  6. Run the visualization pipeline on some events with "SD", "CD" and "TCD" banks: ::

      $ flreconstruct \
        --input-file "@falaise.resources:modules/VisuToy/examples/data/Se82_0nubb-source_strips_bulk_SD-CD-TCD.brio" \
        --pipeline "config/mc_visu_pipeline.conf"
