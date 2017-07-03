Using The FLVisualize Application {#usingflvisualize}
=================================

\tableofcontents

Introduction to FLVisualize {#usingflvisualize_intro}
===========================

FLVisualize is the main detector/event viewer GUI for Falaise, and can
display both raw and reconstructed data for the BiPo3 and Demonstrator
detectors. It can be used to view:

- Raw detector geometries,
- Raw/Calibrated data from flsimulate,
- Reconstructed tracks and calorimetry.

Here we present a brief overview of running FLVisualize from the
command line.

Example Usage {#usingflvisualize_examples}
=============

The flvisualize  program is a  command line application just  like any
other Unix style program (e.g. `ls`).  In the following, we will write
commands assuming that  `flvisualize` is in your path.  If  it is not,
simply use the relative or absolute path to `flvisualize`.

You can get help on the options that can be passed to `flvisualize` by
running it with the `-h` or `--help` options, e.g.

~~~~~
$ flvisualize --help
flvisualize (3.0.0) : the SuperNEMO event display application

Usage:
  flvisualize [options]

General options:
  -h [ --help ]                         produce help message
  --version                             print version number
  -P [ --logging-priority ] level (=notice)
                                        set the logging priority threshold
  -l [ --load-dll ] library             set a DLL to be loaded.
  -x [ --experiment-setup ] tag         set the experimental setup tag.
  -v [ --variant-config ] tag           set the variant configuration.
  -f [ --variant-profile ] tag          set the variant profile.
  -s [ --services-config ] tag          set the services configuration.
  -G [ --ignore-input-metadata ] flag (=0)
                                        flag to ignore input metadata
  -M [ --input-metadata-file ] file     file from which to load metadata
  -i [ --input-file ] file              file from which to read input data
                                        (simulation, real)

Browser options:
  -S [ --scale ] value                  scale factor for computer screen
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
  -I [ --input-data-files ] file        set an input data file(s)

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

Kernel options:
  --datatools::logging level (=fatal)   Set the Bayeux/datatools kernel's
                                        logging priority threshold.
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
 3) Using special experimental setup with variant profile:
    flvisualize \
      --verbose \
      --experiment-setup "urn:snemo:demonstrator:setup:1.0" \
      --variant-profile "variant.profile" \
      --input-file <simulation/reconstruction file>
 4) Visualizing BiPo3 detector :
    flvisualize \
      --detector-config-file $(flquery --resourcedir)/config/bipo3/geometry/2.0/manager.conf \
      --style-config-file $(flquery --resourcedir)/modules/EventBrowser/styles/bipo3_default.sty

~~~~~

If no command line arguments are passed, then `flvisualize` will start
up  and  display the  SuperNEMO  demonstrator  default geometry  model
(tag=`"urn:snemo:demonstrator:setup:1.0"̀  with   its  default  variant
profile):

~~~~~
$ flvisualize
... GUI window should pop up ...
~~~~~

To visualize a data file output by the `flsimulate` or `flreconstruct`
applications, you need to supply the input file:

~~~~~
$ flvisualize -i example.brio
~~~~~

If metadata  are embedded in the  input data file, they  are extracted
from it and  FLSimulate tries to guess the experimental  setup and the
associated geometry model it should  use in combinaison with the data.
If  an explicit  *variant  profile*  has been  used  to configure  the
geometry, it must  be given in the command line  because once started,
FLSimulate (in its current version) cannot switch to another geometry
model  on  the fly.  In  such  case,  the  recommended way  to  invoke
`flvisualize` is to  explicitely describe the geometry  setup from the
command line:

~~~~~
$ flvisualize \
	-x "urn:snemo:demonstrator:setup:1.0" \
	-f "my-variant.profile" \
	-i example.brio
~~~~~

If metadata are only provided from an external file, you should use:
~~~~~
$ flvisualize \
  -M example.meta \
  -i example.brio
~~~~~

If external metadata *and* a non registered variant profile should be
used (no registered official tag is supported for it), you should use:
~~~~~
$ flvisualize \
  -M example.meta \
  -f "my-unregistered-variant.profile" \
  -i example.brio
~~~~~

The  program  displays  its  main  window  with  a  two  panels  event
viewer. The image below shows a Tl-208 decays from a field wire in the
tracking volume with emission of one electron and three gamma rays. On
the left panel, the *Focus on ROI* option is activated (see below).

![FLVisualize main window with one displayed event](@ref flv_event_0.png)
@latexonly
\includegraphics[width=\linewidth]{flv_event_1}
@endlatexonly

Files may  also be  opened using the  `"File->Open file..."`  GUI menu
item.  In  this case,  the experimental  setup (geometry)  and variant
profile should have  been set at start from the  command line as shown
above.

Description of the display
==========================

The top bar contains four menus:

- `File` : allow to load an input data file from FLSimulate or FLReconstruct,
reload a previously loaded file (rewind), save individual events in XML file,
load style file, quit the application.
- `View` : activate special display drivers (x3d, ROOT'S GL viewer), save images of the events,
- `Options` : focus on the event's *region of interest*  (*ROI*, automatically computed),
  activate the rendering of some parts of the event models:
	  - truth hits (from the `SD` bank,
	  - calibrated hits (from the `CD` bank),
	  - fitted tracks and reconstruction patterns (from reconstruction banks)...
- `Misc.` : additional options.

By default, FLVisualize starts with two panels, one  (left) for the detector top view
and the other (right) for a 3D rotating view.

The left panel allows to switch between top, side and front views thanks to radiobuttons
at the bottom.

The right panel contains several pans:

- `Overview` : display the 3D view
- `Tracks` : display a hierachy of the event data models with vertex, hits, tracks and
  other reconstruction data structures stored in the event model. Check boxes allow to
  make the corresponding graphics representations visible or not on the display.
- `Options` : proposes a set of additional display options (colors...)
- `Selection` : allows to select events given a set of cuts.

A bottom bar contains:

- a message log combo (read only),
- a set of navigation buttons to loop on input events (forward/backward).

It is recommended to maximize the size of the FLVisualize's main window on your desktop.

Regarding event selection, the selection is done on data bank level. For
instance, it is possible to select event by setting a range of run/event ids, a
cut that operates on the event header bank. It is also possible to use homemade
cuts by defining a cut manager file that holds several cut files. The cut manager
file should look likes

~~~~~
#@description Logging priority
logging.priority : string = "error"

#@description A list of files that contains definition of cuts
cuts.configuration_files : string[1] as path = "event_header_cuts.conf"
~~~~~

where ```event_header_cuts.conf``` file can hold an event list id cut such as

~~~~~
#@description A sample list of setups
#@key_label   "name"
#@meta_label  "type"

[name="list_of_ids_cut" type="snemo::cut::event_header_cut"]

#@description Cut description
cut.description : string = "Select event following a list of event ids"

#@description Logging priority
logging.priority : string = "error"

#@description The label/name of the event header' bank (mandatory)
EH_label : string = "EH"

#@description Activate list of event ids mode
mode.list_of_event_ids : boolean = true

#@description Filename with event id
list_of_event_ids.ids : string[5] = "0_3"  "0_7"  "0_15" "0_25" "0_69"
~~~~~

Only events from run ```0``` and with event ids ```3, 7, 15, 25``` and ```69```
will be selected.

To make use of this cut, ```flvisualize``` program should be run with the
```--cut-config-file``` option set :

~~~~~
$ flvisualize \
  --cut-config-file cut_manager.conf \
  -i example.brio
~~~~~

The additionnal ```--cut-config-file``` will enable the "Complex cut" widget and
the ```list_of_ids_cut``` may be selected within the combo box.

Known Issues
============

Performance may be affected on  slower systems or those without modern
graphics cards due to the complexity of the geometry.

macOS systems may also see some performance loss on startup and moving
between events.

Both issues are under investigation.
