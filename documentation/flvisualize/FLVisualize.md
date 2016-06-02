Using The FLVisualize Application {#usingflvisualize}
=================================

\tableofcontents

Introduction to FLVisualize {#usingflvisualize_intro}
===========================
FLVisualize is the main detector/event viewer GUI for Falaise, and
can display both raw and reconstructed data for the BiPo3 and Demonstrator
detectors. It can be used to view

- Raw detector geometries
- Raw/Calibrated data from flsimulate
- Reconstructed tracks and calorimetry

Here we present a brief overview of running FLVisualize from the
command line.

Example Usage {#usingflvisualize_examples}
=============
The flvisualize program is a command line application just like any
other Unix style program (e.g. `ls`). In the following, we will
write commands assuming that `flvisualize` is in your path. If is not,
simply use the relative or absolute path to `flvisualize`.

You can get help on the options that can be passed to `flvisualize`
by running it with the `-h` or `--help` options, e.g.

~~~~~
snemo_event_browser -- A generic SuperNEMO event browser program

Usage :

  ${SNVISUALIZATION_BIN_DIR}/snemo_event_browser [OPTIONS] [ARGUMENTS]

Allowed options:
  -h [ --help ]                         produce help message
  -P [ --logging-priority ] arg (=notice)
                                        set the logging priority threshold
  -s [ --scale ] arg                    scale factor for computer screen
                                        (height/width)
  -a [ --auto-reading-delay ] arg       automatic event reading delay in
                                        seconds
  --detector-config-file arg            set the path to the detector config
                                        file
                                        default file is located at
                                        '${SNGEOMETRY_DATA_DIR}/resources/setup
                                        s/snemo/config_1.0/manager.conf'
  --style-config-file arg               set the path to the style config file
  --cut-config-file arg                 set the path to the cut config file
  --preload                             enable the load in memory of Boost
                                        archive files (working only with pure
                                        'sng4' output)
  -i [ --input-files ] arg              set an input file(s)
  -l [ --load-dll ] arg                 set a DLL to be loaded.


View options (arg is either true or false):
  --2d-display arg (=left)          set 2D display position
  --full-2d-view                    add a new tab with top/front/side 2D view
                                    in one frame
  --focus-on-roi                    focus views on the 'region-of-interest'
  --show-simulated-vertex arg       show simulated vertex
  --show-simulated-tracks arg       show simulated tracks
  --show-simulated-hits arg         show simulated hits
  --show-calibrated-hits arg        show calibrated hits
  --show-calibrated-info arg        show calibrated info
  --show-tracker-clustered-hits arg show tracker clustered hits
  --show-tracker-trajectories arg   show tracker trajectories
  --show-particle-tracks arg        show particle tracks


Examples :

 1) Using default configuration :
    snemo_event_browser \
         --input-file ${SNCORE_DATA_DIR}/resources/setups/snemo/samples/snemo_SD_CD_0.txt.gz
 2) Auto reading mode:
    snemo_event_browser \
         --verbose \
         --auto-reading-delay 2 \
         --input-file ${SNCORE_DATA_DIR}/resources/setups/snemo/samples/snemo_SD_0.txt.gz
 3) Setting a special detector config file:
    snemo_event_browser \
         --detector-config-file ${SNGEOMETRY_DATA_DIR}/resources/setups/bipo3/config_1.0/manager.conf \
         --style-config-file    ${SNVISUALIZATION_DATA_DIR}/resources/styles/bipo3_default.sty \
         --input-file ${SNCORE_DATA_DIR}/resources/setups/snemo/samples/snemo_SD_0.txt.gz
 4) Using default configuration (with preload of plain archived 'simulation_data' objects):
    snemo_event_browser \
         --verbose \
         --preload \
         --input-file plain_sng4_output.data.gz

 See README for other running examples

~~~~~

Note that at the current revision, porting of the program from the older
"SNWare" system has not been completed, so that the display names are
not accurate.

If no command line arguments are passed, then `flvisualize` will start
up and display the Demonstrator geometry:

~~~~~
$ flvisualize
... GUI window should pop up ...
~~~~~

To visualize a data file output by the `flsimulate` or `flreconstruct`
applications, you need to supply the input file:

~~~~~
$ flvisualize -i example.brio
~~~~~

Files may also be opened using the "File->Open file..." GUI menu item.

TODO: Screenshots and additional guides

Known Issues
============
Performance may be affected on slower systems or those without modern graphics
cards due to the complexity of the geometry.

OS X systems may also see some performance loss on startup and moving between events.

Both issues are under investigation.

