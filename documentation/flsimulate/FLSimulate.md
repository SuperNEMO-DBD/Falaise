Using The FLSimulate Application {#usingflsimulate}
================================

\tableofcontents

Introduction to FLSimulate {#usingflsimulate_intro}
==========================
FLSimulate's task is to simulate the generation and passage of particles
through the SuperNEMO detectors (BiPo3 and SuperNEMO), recording the
detector response and writing this to an output file. Its simulation
engine for geometry and physics uses the [Geant4](http://geant4.cern.ch)
toolkit, with I/O handled by the Bayeux framework.

Here we present a brief overview of running FLSimulate from the command
line to generate an output file suitable for input to the
[FLReconstruct](@ref usingflreconstruct) application.

At present, FLSimulate supports simulation of the SuperNEMO
demonstrator module, tracker module for commissioning and the BiPo3
detector. Note however that these interfaces are still in development.

Please contact the Software Working Group if you have any questions or
feature requests.

Using FLSimulate on the Command Line {#usingflsimulate_commandline}
====================================
FLSimulate is implemented as a simple command line application just
like familiar UNIX commands such as `ls`. In the following, we will
write commands assuming that `flsimulate` is in your path. If is not,
simply use the relative or absolute path to `flsimulate`.

You can get help on the
options that can be passed to `flsimulate` by running it with the `-h`
of `--help` options, e.g.

~~~~~
$ flsimulate --help
flsimulate 2.1.0
Usage:
  flsimulate [options]
Options
  -h [ --help ]                         print this help message
  --version                             print version number
  -v [ --verbose ]                      increase verbosity of logging
  -n [ --number ] [events] (=1)         number of events to simulate
  --experiment [name] (=default)        experiment to simulate
  -x [ --vertex-generator ] [name] (=source_strips_bulk)
                                        The name of the vertex generator
  -e [ --event-generator ] [name] (=Se82.0nubb)
                                        The name of the event generator
  -p [ --output-profiles ] [rule]       The output profiles activation rule
                                        (setup the truth hits' level of
                                        details)
  -o [ --output-file ] [file]           file in which to store simulation
                                        results
~~~~~

The `--version` option provides detailed information of the current
status of the application, including which libraries it uses:

~~~~~
$ flsimulate --version
flsimulate 1.0.0

Copyright (C) 2013-2016 SuperNEMO Collaboration

flsimulate uses the following external libraries:
* Falaise : 2.1.0
* Bayeux  : 2.1.0
* Boost   : 106000
* Geant4  : 9.6.4
~~~~~

Note that the exact versions shown will depend on the current release
and what versions of packages are linked.

At present, you can only run the simulation in batch mode. The Demonstrator
Module, Tracker Commissioning and BiPo3 experiments can be selected to
be simulated, the default being the Demonstrator Module. By default, 1
event is generated, with vertices generated in the bulk of the source foil
with particle energies/directions sampled from the zero neutrino double
beta spectrum for Se82. You can modify the number of events and
the output file, which can be in XML (for debugging purpose) or
Bayeux's Brio binary format (for production).

Note that at present the validity of the experiment and
event/vertex generator combinations are not checked. You may therefore
need to set all of these by hand to obtain a valid set for simulation.
Preliminary lists of valid vertex and event generators for each
experiment are [tabulated below](@ref usingflsimulate_generator_table).

For example, to simulate 100 Se82 0nBB events from the source foil
in the Demonstrator and write them to a file named `example.brio` in the current working directory simply run

~~~~~
$ flsimulate --experiment=demonstrator -n 100 -e Se82.0nubb -x source_strips_bulk -o example.brio
... lots of logging ...
$
~~~~~

Or to simulate 100 Se82 2nBB events from the source foil
in the Demonstrator and write them to a file named `example2.brio` in the current working directory simply run

~~~~~
$ flsimulate --experiment=demonstrator -n 100 -e Se82.2nubb -x source_strips_bulk -o example2.brio
... lots of logging ...
$
~~~~~

The resultant files can be examined with the `flreconstruct` application, see the
[dedicated guide](@ref usingflreconstruct) for further details.

Available Experiments {#usingflsimulate_experiments}
=====================
The currently available experiment names in `flsimulate` that can be passed
to the `--experiment` argument are:

- Demonstrator
- BiPo3
- Tracker_Commissioning

The above spellings must be used, but the interface is case insensitive.
For example, "demo" would result in a failure, but any of "Demonstrator",
"demonstrator", "DEMONSTRATOR" or "dEmOnStRaToR" would work.

Available Vertex/Event Generators {#usingflsimulate_generator_table}
=================================
The architecture of `flsimulate` separates the specification of where
events are generated (geometry) from the specification of how the events
are generated (physics). With several different experiments being modelled
and many background, signal and calibration physics sources available,
a wide range of vertex and event generators are available.

Here we tabulate the vertex and event generators available for each
experiment. The first column in each table gives the argument that
should be supplied to the `--vertex-generator` ("Vertex Generator ID") and
`--event-generator` ("Event Generator ID") options of `flsimulate`.
The second column gives a brief description of the generator, with
subsequent columns indicating whether the generator applies to specific
experiment simulations.

Note that `flsimulate` will throw an exception if you supply an unknown generator for the experimental setup being simulated. However, it does not
check that the combination of vertex and event generator are sensible.
For example, you are at liberty to generate Se82.0nubb events within the
bulk of the anode wires!

**TODO** : Provide command line options to list available generators for a
given experiment.

Vertex Generators
-----------------
| Vertex Generator ID | Description | BiPo3 | Demonstrator | Tracker_Commissioning |
| ------------------ | :---------- | :---: | :----------: | :-------------------: |
| anode_wire_bulk | Generate vertex on the bulk volume of the anode wires | no | yes | yes |
| anode_wire_surface | Generate vertex on the surface of the anode wires | no | yes | yes |
| calo_wrapper_bulk | Vertex generation from the bulk of the main calorimeter wrapper | no | yes | no |
| calo_wrapper_surface | Vertex generation from the surface of the main calorimeter wrapper | no | yes | no |
| experimental_hall_roof | Generate vertex from the hall: | no | no | yes |
| field_wire_bulk | Generate vertex on the bulk volume of the field wires | no | yes | yes |
| field_wire_surface | Generate vertex on the surface of the anode wires: | no | yes | yes |
| gveto_wrapper_bulk | Vertex generation from the bulk of the gamma veto wrapper | no | yes | no |
| gveto_wrapper_surface | Vertex generation from the surface of the gamma veto wrapper | no | yes | no |
| reflector_bulk | Generate vertex in the bulk of the scintillator entrance window reflector | yes | no | no |
| reflector_deposit | Generate vertex on the front surface of the scintillator entrance window reflector | yes | no | no |
| scintillator_bulk | Generate vertex in the bulk of the scintillator block | yes | no | no |
| scintillator_side_surfaces | Generate vertex on the side surfaces of the scintillator block | yes | no | no |
| source_bulk | Generate vertex in the bulk volume of the source foil | yes | no | no |
| source_strips_bulk | Vertex generation from the bulk volume of the source strips | no | yes | no |
| source_strips_external_bulk | Vertex generation from the bulk volume of the outer source strips | no | yes | no |
| source_strips_external_surface | Vertex generation from the surface of the outer source strips | no | yes | no |
| source_strips_internal_bulk | Vertex generation from the bulk volume of the inner source strips | no | yes | no |
| source_strips_internal_surface | Vertex generation from the surface of the inner source strips | no | yes | no |
| source_strips_surface | Vertex generation from the surface of the source strips | no | yes | no |
| source_surface_all | Generate vertex on the surface of the source foil | yes | no | no |
| source_surface_back | Generate vertex on the back surface of the source foil | yes | no | no |
| source_surface_front | Generate vertex on the front surface of the source foil | yes | no | no |
| top_trigger_surface | Generate vertex on the top surface of the muon trigger top scintillator plate | no | no | yes |
| xcalo_wrapper_bulk | Vertex generation from the bulk of the X-wall calorimeter wrapper | no | yes | no |
| xcalo_wrapper_surface | Vertex generation from the surface of the X-wall calorimeter wrapper | no | yes | no |

Event Generators
----------------
| Event Generator ID | Description | BiPo3 | Demonstrator | Tracker_Commissioning |
| ------------------ | :---------- | :---: | :----------: | :-------------------: |
| Am241 | Am-241 decay [calibration] | yes | yes | yes |
| Bi207 | Bi-207 decay [calibration] | yes | yes | yes |
| Bi212_Po212 | Bi-212/Po-212 decay [background] | yes | yes | yes |
| Bi214_Po214 | Bi-214/Po-214 decay [background] | yes | yes | yes |
| Ca48.0nubb | Neutrinoless double beta decay of Ca-48, 0nubb(mn) [DBD] | yes | yes | yes |
| Ca48.0nubbM1 | Neutrinoless double beta decay of Ca-48, 0nubbM1 [DBD] | yes | yes | yes |
| Ca48.0nubbM2 | Neutrinoless double beta decay of Ca-48, 0nubbM2 [DBD] | yes | yes | yes |
| Ca48.0nubb_rhc_lambda_0 | Neutrinoless double beta decay of Ca-48, 0nubb(rhc-lambda) 0+ -> 0+ {2n} [DBD] | yes | yes | yes |
| Ca48.0nubb_rhc_lambda_0_2 | Neutrinoless double beta decay of Ca-48, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*} [DBD] | yes | yes | yes |
| Ca48.2nubb | Two neutrino double beta decay of Ca-48, 2nubb [DBD] | yes | yes | yes |
| Co60 | Co-60 decay [calibration] | yes | yes | yes |
| K40 | K-40 decay [background] | yes | yes | yes |
| Mn54 | Mn-54 decay [calibration] | yes | yes | yes |
| Mo100.0nubb | Neutrinoless double beta decay of Mo-100, 0nubb(mn) [DBD] | yes | yes | yes |
| Mo100.0nubbM1 | Neutrinoless double beta decay of Mo-100, 0nubbM1 [DBD] | yes | yes | yes |
| Mo100.0nubbM2 | Neutrinoless double beta decay of Mo-100, 0nubbM2 [DBD] | yes | yes | yes |
| Mo100.0nubb_rhc_lambda_0 | Neutrinoless double beta decay of Mo-100, 0nubb(rhc-lambda) 0+ -> 0+ {2n} [DBD] | yes | yes | yes |
| Mo100.0nubb_rhc_lambda_0_2 | Neutrinoless double beta decay of Mo-100, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*} [DBD] | yes | yes | yes |
| Mo100.2nubb | Two neutrino double beta decay of Mo-100, 2nubb [DBD] | yes | yes | yes |
| Na22 | Na-22 decay [calibration] | yes | yes | yes |
| Nd150.0nubb | Neutrinoless double beta decay of Nd-150, 0nubb(mn)[DBD] | yes | yes | yes |
| Nd150.0nubbM1 | Neutrinoless double beta decay of Nd-150, 0nubbM1 [DBD] | yes | yes | yes |
| Nd150.0nubbM2 | Neutrinoless double beta decay of Nd-150, 0nubbM2 [DBD] | yes | yes | yes |
| Nd150.0nubb_rhc_lambda_0 | Neutrinoless double beta decay of Nd-150, 0nubb(rhc-lambda) 0+ -> 0+ {2n} [DBD] | yes | yes | yes |
| Nd150.0nubb_rhc_lambda_0_2 | Neutrinoless double beta decay of Nd-150, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*} [DBD] | yes | yes | yes |
| Nd150.2nubb | Two neutrino double beta decay of Nd-150, 2nubb [DBD] | yes | yes | yes |
| Pa231 | Ra-226-90 decay [background] | yes | yes | yes |
| Pa234m | Pa-234m decay [background] | yes | yes | yes |
| Ra226 | Ra-226-90 decay [background] | yes | yes | yes |
| Se82.0nubb | Neutrinoless double beta decay of Se-82, 0nubb(mn)[DBD] | yes | yes | yes |
| Se82.0nubbM1 | Neutrinoless double beta decay of Se-82, 0nubbM1 [DBD] | yes | yes | yes |
| Se82.0nubbM2 | Neutrinoless double beta decay of Se-82, 0nubbM2 [DBD] | yes | yes | yes |
| Se82.0nubb_rhc_lambda_0 | Neutrinoless double beta decay of Se-82, 0nubb(rhc-lambda) 0+ -> 0+ {2n} [DBD] | yes | yes | yes |
| Se82.0nubb_rhc_lambda_0_2 | Neutrinoless double beta decay of Se-82, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*} [DBD] | yes | yes | yes |
| Se82.2nubb | Two neutrino double beta decay of Se-82, 2nubb [DBD] | yes | yes | yes |
| Sr90 | Sr-90 decay [background] | yes | yes | yes |
| Tl208 | Tl-208 decay [background] | yes | yes | yes |
| Y90 | Y-90 decay [background] | yes | yes | yes |
| electron.100keV | Electron with monokinetic energy @ 100 keV [miscellaneous] | yes | yes | yes |
| electron.1MeV | Electron with monokinetic energy @ 1 MeV [miscellaneous] | yes | yes | yes |
| electron.200keV | Electron with monokinetic energy @ 200 keV [miscellaneous] | yes | yes | yes |
| electron.20keV | Electron with monokinetic energy @ 20 keV [miscellaneous] | yes | yes | yes |
| electron.2MeV | Electron with monokinetic energy @ 2 MeV [miscellaneous] | yes | yes | yes |
| electron.3MeV | Electron with monokinetic energy @ 3 MeV [miscellaneous] | yes | yes | yes |
| electron.50-2000keV_flat | Electron with energy in the [50keV-2MeV] range [miscellaneous] | yes | yes | yes |
| electron.500keV | Electron with monokinetic energy @ 500 keV [miscellaneous] | yes | yes | yes |
| electron.50keV | Electron with monokinetic energy @ 50 keV [miscellaneous] | yes | yes | yes |
| electron.cosmic.500MeV | Electron with monokinetic energy @ 500 MeV [cosmic] | no | no | yes |
| gamma.100keV | Gamma with monokinetic energy @ 100 keV [miscellaneous] | yes | yes | yes |
| gamma.1MeV | Gamma with monokinetic energy @ 1 MeV [miscellaneous] | yes | yes | yes |
| gamma.20keV | Gamma with monokinetic energy @ 20 keV [miscellaneous] | yes | yes | yes |
| gamma.2615keV | Gamma with monokinetic energy @ 2.615 MeV [miscellaneous] | yes | yes | yes |
| gamma.2MeV | Gamma with monokinetic energy @ 2 MeV [miscellaneous] | yes | yes | yes |
| gamma.500keV | Gamma with monokinetic energy @ 500 keV [miscellaneous] | yes | yes | yes |
| gamma.50keV | Gamma with monokinetic energy @ 50 keV [miscellaneous] | yes | yes | yes |
| muon.cosmic.sea_level.toy | Parameters for the "cosmic muon generator" mode [cosmic] | no | no | yes |

Available output profiles {#usingflsimulate_output_profiles}
=========================

By default FLSimulate produces collections of truth MC hits and stored them in the output
data model (the \ref mctools::simulated_data class).

For the `Demonstrator` configuration, four *official* collections of truth hits are thus populated:

- `calo` : truth MC hits collected from the scintillator blocks of the main calorimeter
- `xcalo` : truth MC hits collected from the scintillator blocks of the X-calorimeter
- `gveto` : truth MC hits collected from the scintillator blocks of the gamma veto
- `gg` : truth MC hits collected from the drift volume of the tracker cells

For the `tracker_commissioning` configuration, two *official* collections of truth hits are supported:

- `trig` : truth MC hits collected from the muon trigger scintillator plates
- `gg` : truth MC hits collected from the drift volume of the tracker cells

Additional  collections of  hits can  be generated  : the  detailed MC
hits.  So far,  we  use  one unique  collection  of  truth hits  named
`__visu.tracks`. The `__visu.tracks` collection  collects all *MC step
hits* that have been generated along particle tracks that crossed some
volumes of interest. To activate the recording of such output, several
*output profiles* have been made available.

For the `Demonstrator` configuration, these are:

- `calo_details`    : collect all Geant4 step hits from the calo, xcalo and gveto sensitive detectors
- `tracker_details` : collect all Geant4 step hits from within volumes in the tracker part of the detector
- `source_details`  : collect all Geant4 step hits from within volumes in the source part of the detector
- `all_details`     : collect all Geant4 step hits from any volumes of interest
                      (shortcut for `calo_details+tracker_details+source_details`)

For the `tracker_commissioning` configuration, these are:

- `trigger_details` : collect all Geant4 step hits from the muon trigger sensitive detectors
- `tracker_details` : collect all Geant4 step hits from within volumes in the tracker part of the detector
- `outside_details` : collect all Geant4 step hits from within volumes filled with air outside the detector
- `all_details`     : collect all Geant4 step hits from any volumes of interest
                      (shortcut for `trigger_details+tracker_details+outside_details`)

The  activation   of  some   additional  output   is  done   with  the
`--output-profiles` option. Example:

~~~~~
$ flsimulate --experiment "Demonstrator" -n 100 --output-profiles "calo_details + tracker_details" -o example_with_visu_hits.brio
...
$
~~~~~

Be aware that using this feature  implies that the simulation will use
additional CPU  and the output  file will use  a lot of  storage. This
option should  thus be reserved  for dedicated studies,  debugging
and visualization  purpose, not  for production  of large  datasets of
simulated data. This feature is not used yet for the BiPo3 setup.

The description of the output simulation data model is described
in a [dedicated page on FLSimulate output](@ref flsimulateoutput).
