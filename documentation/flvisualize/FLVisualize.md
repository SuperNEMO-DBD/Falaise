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
flvisualize (3.0.0) : the SuperNEMO event display application

Usage:
  flvisualize [options]

General options:
  -h [ --help ]                         produce help message
  --version                             print version number
  -P [ --logging-priority ] level (=notice)
                                        set the logging priority threshold
  -l [ --load-dll ] library             set a DLL to be loaded.

Browser options:
  -s [ --scale ] value                  scale factor for computer screen
                                        (height/width)
  -a [ --auto-reading-delay ] value     automatic event reading delay in
                                        seconds
  --detector-config-file file           set the path to the detector
                                        configuration file
  --style-config-file file              set the path to the style configuration
                                        file
  --cut-config-file file                set the path to the cut configuration
                                        file
  --preload                             enable the load in memory of Boost
                                        archive files (working only with pure
                                        'bxg4_production' output)
  -i [ --input-files ] file             set an input file(s)

View options:
  --2d-display position (=left)         set 2D display position
  --full-2d-view                        add a new tab with top/front/side 2D
                                        view in one frame
  --focus-on-roi                        focus views on the 'region-of-interest'
  --show-simulated-vertex flag          show simulated vertex
  --show-simulated-tracks flag          show simulated tracks
  --show-simulated-hits flag            show simulated hits
  --show-calibrated-hits flag           show calibrated hits
  --show-calibrated-info flag           show calibrated info
  --show-tracker-clustered-hits flag    show tracker clustered hits
  --show-tracker-trajectories flag      show tracker trajectories
  --show-particle-tracks flag           show particle tracks

Variants support:
  --variant-config [file]               file from which to load the main
                                        configuration of the variant repository
  --variant-registry [rule]             rules for additional variant registries
  --variant-registry-dependencies [rule]
                                        dependencies for additional variant
                                        registries
  --variant-load [file]                 file from which to load a specific
                                        variant profile at startup
  --variant-load-no-unknown             do not ignore unknown registry sections
                                        at variant profile loading
  --variant-set [setting]               setting rules for variant parameters
                                        applied at startup
  --variant-gui                         launch the variant GUI editor at
                                        startup
  --variant-store [file]                file in which to store the effective
                                        current variant profile
  --variant-reporting [file]            file in which to print the variant
                                        usage report

Kernel options:
  --datatools::logging level (=fatal)   Set the datatools kernel's logging
                                        priority threshold.
                                        Example :
                                          --datatools::logging="trace"
  --datatools::resource-path rule       Register a resource path mounted
                                        through an identifier (library or
                                        module name).
                                        Example : Register the "path1/subdir1"
                                        path as the root directory of the "foo"
                                        library and the "path2/subdir2" path as
                                        the root directory of the "bar"
                                        software module:
                                          --datatools::resource-path="foo@path1
                                        /subdir1"
                                          --datatools::resource-path="bar@path2
                                        /subdir2"

Examples:

 1) Using default configuration :
    flvisualize \
      --input-file <simulation/reconstruction file>
 2) Auto reading mode:
    flvisualize \
      --verbose \
      --auto-reading-delay 2 \
      --input-file <simulation/reconstruction file>
 3) Visualizing BiPo3 detector :
    flvisualize \
      --detector-config-file $(flquery --resourcedir)/config/bipo3/geometry/2.0/manager.conf \
      --style-config-file $(flquery --resourcedir)/modules/EventBrowser/styles/bipo3_default.sty


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

macOS systems may also see some performance loss on startup and moving between events.

Both issues are under investigation.
