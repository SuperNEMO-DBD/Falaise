Using The FLSimulate Application {#usingflsimulate}
================================

\tableofcontents

Introduction to FLSimulate {#usingflsimulate_intro}
==========================
FLSimulate's task is to simulate the generation and passage of particles
through the SuperNEMO detector, recording the
detector response and writing this to an output file. Its simulation
engine for physics uses the [Geant4](http://geant4.cern.ch)
toolkit, with geometry modelling and I/O handled by the Bayeux framework.

Here we present a brief overview of running FLSimulate from the command
line to generate an output file suitable for input to the
[FLReconstruct](@ref usingflreconstruct) application.

At present, FLSimulate only supports simulation of the SuperNEMO
demonstrator module, with several geometry and simulation variants.
The user is free to choose among several options:
- geometry layout of the detector: `Basic` (full detector) or `HalfCommissionig`
- activation of the magnetic field of not
- use of the external shieling
- change source material and thickness
- use of calibration sources of not
- use of a specific vertex generator
- use of a specific event generator
- use of a specific simulation options
Other options are available (see section below).

Please contact the Software Working Group if you have any questions or
feature requests.

Using FLSimulate on the Command Line {#usingflsimulate_commandline}
====================================
FLSimulate is implemented as a simple command line application just
like familiar UNIX commands such as `ls`. In the following, we will
write commands assuming that `flsimulate` is in your path. If is not,
simply use the relative or absolute path to `flsimulate`.

You can get help on the options that can be passed to `flsimulate`
by running it with the `-h` of `--help` options, e.g.

~~~~~
$ flsimulate --help
flsimulate (3.0.0) : SuperNEMO simulation program
Usage:
  flsimulate [options]
Options:
  -h [ --help ]             print this help message
  --help-scripting          print help on input script format and schema
  --version                 print version number
  -c [ --config ] file      configuration script for simulation
  -o [ --output-file ] file file in which to store simulation results

~~~~~

The `--version` option provides detailed information of the current
status of the application, including which libraries it uses:

~~~~~
$ flsimulate --version
flsimulate 3.0.0

Copyright (C) 2013-2017 SuperNEMO Collaboration

flsimulate uses the following external libraries:
* Falaise : 3.0.0
* Bayeux  : 3.0.0
* Boost   : 106000
* Geant4  : 9.6.4

~~~~~

Note that the exact versions shown will depend on the current release
and what versions of packages are linked.

flsimulate  basically   runs  the  simulation  in   batch  mode.   The
Demonstrator  Module  experiment  is   automatically  selected  to  be
simulated,  the  default  geometry  layout being  the  `Basic`  (full)
Demonstrator Module.  By default, 1 event is generated as one single 1
MeV electron,  with vertices arbitrary generated  from the center of
the detector.

You must provide a configuration script to set basic  parameters of the
simulation (`SimulationSubsystem` section):
- the simulation version,
- the number of events,
- the input file for the  seeding of random number generators.

A larger set of physics parameters can be tweaked through
support of variant configuration parameters (`VariantSubsystem` section):
- various geometry options,
- the  vertex generator,
- the primary event  generator,
- additional simulation parameters...
The list  of  user   choices  is  described  in  the   section  about  the
[description      of      simulation     variant      parameters](FlSimulateVariantsDoc.html). The lists of valid vertex and event
generators  for  each  geometry  variant options  can also be  dynamically
browsed through the Bayeux's `bxvariant_inspector` program.

The output  file is  set from the  command line.  It  can use  the XML
format (for  debugging purpose)  or Bayeux's  Brio binary  format (for
production).

To learn more about the format of the script, use:
~~~~~
$ flsimulate --help-scripting
...
~~~~~

For example, to simulate 100 Se82 0nuBB events from the source foil
in the full Demonstrator and write them to a file named `example.brio`
in the current working directory, you must create the following `simu.conf` script:
~~~~~
#@description Main flsimulate configuration script
#@key_label  "section"
#@meta_label "description"

[section="SimulationSubsystem" description=""]
#@description Simulation setup
experimentID      : string = "demonstrator"       # Name of detector to simulate (default="demonstrator")
simulationVersion : string = "2.1"                # Version of the simulation setup (default="2.1")
numberOfEvents    : integer = 100                 # Number of events to simulate (default=1)
moduloEvents      : integer =  10                 # Progression rate on simulated events
rngSeedFile       : string as path = "seeds.conf" # Random seeds file

[section="VariantSubsystem" description=""]
#@description Variant setup
settings : string[4] = \
  "@geometry:layout=Basic" \
  "@vertexes:generator=source_pads_bulk" \
  "@primary_events:generator=Se82.0nubb" \
  "@simulation:output_profile=none"

~~~~~

where the `seeds.conf` file is:
~~~~~
{EG=142921705; MGR=569932270; SHPF=1008320517; VG=1002945362}
~~~~~
as generated by the following command:
~~~~~
$ bxg4_seeds -k -d . -p seeds.conf
~~~~~

Simulation then runs with:
~~~~~
$ flsimulate --config simu.conf -o example.brio
... lots of logging ...
~~~~~

The  resultant   files  can  be  examined   with  the  `flreconstruct`
application,  see the  [dedicated guide](@ref  usingflreconstruct) for
further details.

Available Experiments {#usingflsimulate_experiments}
=====================

The currently only available experiment name in `flsimulate` is:

- `Demonstrator`

The above spellings must be used, but the interface is case insensitive.
For example, "demo" would result in a failure, but any of "Demonstrator",
"demonstrator", "DEMONSTRATOR" or "dEmOnStRaToR" would work.

Available Vertex/Event Generators {#usingflsimulate_generator_table}
=================================

The architecture of `flsimulate`  separates the specification of where
events  are generated  (geometry) from  the specification  of how  the
events  are  generated  (physics).  With  several  different  geometry
layouts  being modelled  and many  background, signal  and calibration
physics sources available, a wide range of vertex and event generators
are available.

From Falaise  3.0, the  choice of geometry  options, vertex  and event
generators is done through a Bayeux/datatools variant service embedded
in the flsimulate  application.  A profile of  variant parameters must
be created to suit the user's needs.

As  mentioned above,  the list  of user  choices is  described in  the
section about the [description  of simulation variant parameters](FlSimulateVariantsDoc.html).

Note  that `flsimulate`  will  throw  an exception  if  you supply  an
unknown  generator  for the  experimental  setup  being simulated.  In
principle,  it does  check that  the combination  of vertex  and event
generator are sensible.


Available output profiles {#usingflsimulate_output_profiles}
=========================

By default FLSimulate produces collections of truth MC hits and stored
them  in  the  output  data model  (the  \ref  mctools::simulated_data
class).

For the  `Demonstrator` configuration  using the `Basic`  layout, four
*official* collections of truth hits are thus populated:

- `calo` : truth MC hits collected from the scintillator blocks of the main calorimeter
- `xcalo` : truth MC hits collected from the scintillator blocks of the X-calorimeter
- `gveto` : truth MC hits collected from the scintillator blocks of the gamma veto
- `gg` : truth MC hits collected from the drift volume of the tracker cells

Additional  collections of  hits can  be generated  : the  detailed MC
hits.  So far,  we  use  one unique  collection  of  truth hits  named
`__visu.tracks`. The `__visu.tracks` collection  collects all *MC step
hits* that have been generated along particle tracks that crossed some
volumes of interest. To activate the recording of such output, several
*output profiles* have been made available.

For the `Demonstrator/Basic` configuration/layout, these are:

- `calo_details`    : collect all Geant4 step hits from the calo, xcalo and gveto sensitive detectors
- `tracker_details` : collect all Geant4 step hits from within volumes in the tracker part of the detector
- `source_details`  : collect all Geant4 step hits from within volumes in the source part of the detector
- `all_details`     : collect all Geant4 step hits from any volumes of interest
                      (shortcut for `calo_details+tracker_details+source_details`)

The  activation   of  some   additional  output   is  done   with  the
`\@simulation:output_profilè variant parameter.

Be aware that using this feature  implies that the simulation will use
additional CPU  and the output  file will use  a lot of  storage. This
option should  thus be reserved  for dedicated studies,  debugging
and visualization  purpose, not  for production  of large  datasets of
simulated data.

The description of the output simulation  data model is described in a
[dedicated page on FLSimulate output](@ref flsimulateoutput).
