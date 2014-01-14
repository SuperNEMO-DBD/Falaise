Using The FLSimulate Application
================================

\tableofcontents

Introduction {#intro}
============
FLSimulate's task is to simulate the generation and passage of particles
through the SuperNEMO detectors (BiPo3 and SuperNEMO), recording the
detector response and writing this to an output file. Its simulation
engine for geometry and physics uses the [Geant4](http://geant4.cern.ch)
toolkit, with I/O handled by the Bayeux framework.

Here we present a brief overview of running FLSimulate from the command
line to generate an output file suitable for input to the
[FLReconstruct](@ref md_FLReconstruct) application.

At present (Alpha 3), FLSimulate supports simulation of the SuperNEMO
demonstrator module, tracker module for commissioning and the BiPo3
detector. Note however that this interface is not complete yet.

Please contact the Software Working Group if you have any questions or
feature requests.

Using FLSimulate on the Command Line {#usingfls}
====================================
FLSimulate is implemented as a simple command line application just
like the familiar UNIX commands like `ls`. In the following, we will
write commands assuming that `flsimulate` is in your path. If is not,
simply use the relative or absolute path to `flsimulate`.

You can get help on the
options that can be passed to `flsimulate` by running it with the `-h`
of `--help` options, e.g.

~~~~~
$ flsimulate --help
flsimulate 1.0.0
Usage:
  flsimulate [options]
Options
  -h [ --help ]                         print this help message
  --version                             print version number
  -v [ --verbose ]                      increase verbosity of logging
  -n [ --number ] [events] (=1)         number of events to simulate
  --experiment [name] (=default)        experiment to simulate
  -x [ --vertex-generator ] [name] (=experimental_hall_roof)
                                        The name of the vertex generator
  -e [ --event-generator ] [name] (=muon.cosmic.sea_level.toy)
                                        The name of the event generator
  -o [ --output-file ] [file]           file in which to store simulation
                                        results

$
~~~~~

The `--version` option provides detailed information of the current
status of the application, including which libraries it uses:

~~~~~
$ flsimulate --version
flsimulate 1.0.0

Copyright (C) 2013 SuperNEMO Collaboration

flsimulate uses the following external libraries:
* Falaise : 1.0.0
* Bayeux  : 1.0.0
* Boost   : 105300
* Geant4  : 10.x (eventually)

$
~~~~~

At present, you can only run the simulation in batch mode. The Demonstrator
Module, Tracker Commissioning and BiPo3 experiments can be selected to
be simulated, the default being the Demonstrator Module. By default, 1
event is generated, with vertices generated at the roof of the hall volume,
and primary muons (mu+ or mu-) with energies/directions from some cosmic
sea level approximated spectrum. You can modify the number of events and
the output file, which can be in XML or Brio format.

Note that at present (Alpha3) the validity of the experiment and
event/vertex generator combinations are not checked. You may therefore
need to set all of these by hand to obtain a valid set for simulation.

For example, to simulate 100 events in the Tracker Commissioning experiment
and write them to a file named `example.brio` in the current working
directory simply run

~~~~~
$ flsimulate --experiment=tracker_commissioning -n 100 -o example.brio
... lots of logging ...
$
~~~~~

The resultant file can be examined with the `flreconstruct` application.

Available Experiments {#fls_experiments}
=====================
The currently available experiment names in `flsimulate` that can be passed
to the `--experiment` argument are:

- Demonstrator
- BiPo3
- Tracker_Commissioning

The above spellings must be used, but the interface is case insensitive.
For example, "demo" would result in a failure, but any of "Demonstrator",
"demonstrator", "DEMONSTRATOR" or "dEmOnStRaToR" would work.

