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
[FLReconstruct](md_FLReconstruct.html) application.

At present (Alpha 1), FLSimulate only supports simulation of the SuperNEMO
tracker module in its commissioning setup, with cosmic muon events. Further
functionality will be added in later alphas for BiPo3 and the SuperNEMO
demonstrator.

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

At present, you can only run the simulation in batch mode using the
Tracker Commissioning geometry and event generators. By default, 1
event is generated, with vertices generated at the roof of the hall volume,
and primary muons with energies/directions from the cosmic sea level
spectrum. You can modify the number of events and the output file, which
can be in XML or Brio format.

For example, to generate 100 events and write them to a file named
`example.brio` in the current working directory simply run

~~~~~
$ flsimulate -n 100 -o example.brio
... lots of logging ...
$
~~~~~

The resultant file can be examined with the `flreconstruct` application.

