Using The FLSimulate Application {#usingflsimulate}
================================

\tableofcontents

Introduction to FLSimulate {#usingflsimulate_intro}
==========================

FLSimulate's  task  is  to  simulate the  generation  and  passage  of
particles  through  the  SuperNEMO detector,  recording  the  detector
response and writing this to an output file. Its simulation engine for
physics   uses  the   [Geant4](http://geant4.cern.ch)  toolkit,   with
geometry modelling and I/O handled by the Bayeux framework.

Here  we present  a  brief  overview of  running  FLSimulate from  the
command line  to generate  an output  file suitable  for input  to the
[FLReconstruct](@ref usingflreconstruct) application.


At  present,  FLSimulate only  supports  simulation  of the  SuperNEMO
demonstrator module,  with several  geometry and  simulation variants.
The user is free to choose among several options:
- geometry layout of the detector: `Basic` (full detector) or `HalfCommissioning`
- activation of the magnetic field of not
- use of the external shieling
- change source material and thickness
- use of calibration sources of not
- use of a specific vertex generator, which may depends on the geometry layout
- use of a specific event generator, which may depends on the geometry layout
- use of a specific simulation options
Other options are available (see section below).

Please contact the Software Working Group if you have any questions or
feature requests.

There is also a legacy version of FLSimulate (flsimulate_legacy) which
does  not provide  as  many  features than  the  current  one.  It  is
deprecated and cannot support the new geometry and variants system.

Using FLSimulate on the Command Line {#usingflsimulate_commandline}
====================================

FLSimulate is  implemented as a  simple command line  application just
like familiar  UNIX commands such as  `ls`. In the following,  we will
write commands assuming that `flsimulate` is  in your path. If is not,
simply use the relative or absolute path to `flsimulate`.

You can get help on the options that can be passed to `flsimulate`
by running it with the `-h` of `--help` options, e.g.

~~~~~
$ flsimulate --help
lsimulate (3.0.0) : SuperNEMO simulation program
Usage:
  flsimulate [options]
Options:
  -h [ --help ]                        print this help message
  --help-scripting                     print help on input script format and
                                       schema
  --help-setup                         print help on simulation setup
  --version                            print version number
  -c [ --config ] file                 configuration script for simulation
  -m [ --output-metadata-file ] file   file in which to store metadata
  -M [ --embedded-metadata ] flag (=0) flag to (de)activate recording of
                                       metadata in the simulation results
                                       output file
  -o [ --output-file ] file            file in which to store simulation
                                       results
  -u [ --user-profile ] name (=normal) set the user profile ("expert",
                                       "normal", "production")
  -V [ --verbosity ] level             set the verbosity level

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
Demonstrator Module.  By default, one event is generated. It consists in
shooting one  1 MeV electron,  with vertices arbitrary  generated from
the center of the detector.

You must provide a configuration script to set the basic parameters of
the simulation (`SimulationSubsystem` section):
- the simulation version,
- the number of events,
- the input file for the  seeding of random number generators.
A    sample    configuration    script    is    provided    in    this
[document](FLSimulate-3.0.0.conf) (for Falaise 3.0.0).

A larger set of physics parameters can be tweaked through
support of variant configuration parameters (`VariantSubsystem` section):
- various geometry options,
- the vertex generator,
- the primary event  generator,
- additional simulation parameters...

The    list    of    user     choices    is    described    in    this
[document](FLSimulateVariantsDoc-2.1.pdf) (for  SuperNEMO Demonstrator
simulation  setup version  2.1 for  Falaise 3.0.0).  It describes  all
available   simulation   variant   parameters   and   their   possible
dependencies in this simulation context.

The  lists of  valid vertex  and  event generators  for each  geometry
variant options can  also be dynamically browsed  through the Bayeux's
`bxvariant_inspector` program  (with builtin  GUI mode of  your Bayeux
setup). From the Falaise build directory, run:

~~~~~
$ bxvariant_inspector \
    --load-dll "Falaise@$(flquery --libdir)" \
    --datatools::resource-path="falaise@$(flquery --resourcedir)" \
    --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
    --variant-gui \
    --variant-store "variants.profile"
~~~~~

Here the program  generates a variant profile file at  exit. This file
stores a given set of variant options and thus can be used as input of
the flsimulate program (see below).

To learn more about the format of the simulation configuration script,
use:

~~~~~
$ flsimulate --help-scripting
...
~~~~~

For example, to simulate 100 Se82 0nuBB events from the source foil in
the full Demonstrator and write them to a file named `example.brio` in
the  current   working  directory,  you  must   create  the  following
`simu.conf` script (using the `datatools::multi_properties` format):

~~~~~
#@description Main flsimulate configuration script
#@key_label  "section"
#@meta_label "description"

[section="SimulationSubsystem" description=""]
#@config Simulation setup
#@description  URN of the simulation setup
simulationUrn : string = "urn:snemo:demonstrator:simulation:2.1"
#@description Number of events to simulate (default=1)
numberOfEvents    : integer = 100
#@description Progression rate on simulated events
moduloEvents      : integer =  10
#@description Random seeds file
rngSeedFile       : string as path = "seeds.conf"

[section="VariantSubsystem" description=""]
#@config Variant setup
#@description List of variant settings
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

Here we can see that FLSimulate request 4 seeds, one for each random number generators
embedded in the Bayeux/Geant4 simulation engine:
- `EG` stands for *event generator*,
- `VG` stands for *vertex generator*,
- `MGR` stands for *simulation manager* (Geant4 kernel),
- `SHPF` stands for *step hit processing factory*, a software component responsible of the generation
  of the final collection of truth hits (see below).
It is very important to make sure that all simulation runs use different sets of seeds in order
to ensure statistical independancy of generated simulation data samples. The `bxg4_seeds` program
helps to generated such lists of independant seed sets.


Note the *VariantSubsystem* here uses an explicit list of *settings*
for some variant parameters. It is possible (and recommended) to use
a variant profile file in place of it:

~~~~~
...
[section="VariantSubsystem" description=""]
#@config Variant setup
#@description Variant profile
profile : string as path = "variants.profile"
~~~~~

where the `variants.profile` explicitely publishes all variant parameters:
~~~~~
@format=datatools::configuration::variant
#@format.version=1.0
#@organization=snemo
#@application=falaise

[registry="geometry"]
layout = "Basic"
layout/if_basic/magnetic_field = true
layout/if_basic/magnetic_field/is_active/type = "UniformVertical"
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude = 25 gauss
layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction = "+z"
layout/if_basic/source_layout = "Basic"
layout/if_basic/source_layout/if_basic/thickness = 250 um
layout/if_basic/source_layout/if_basic/material = "Se82"
layout/if_basic/source_calibration = false
layout/if_basic/shielding = true
calo_film_thickness = 25 um

[registry="vertexes"]
generator = "source_pads_bulk"

[registry="primary_events"]
generator = "Se82.0nubb"

[registry="simulation"]
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
output_profile = "none"

~~~~~


The FLSimulate output  file is  set from the  command line.  It  may use  the XML
format (for  debugging purpose)  or Bayeux's  Brio binary  format (for
production).

Simulation then runs with:
~~~~~
$ flsimulate -c simu.conf -m example.meta -o example.brio
... lots of logging ...
~~~~~

The  resultant   files  can  be  examined   with  the  `flreconstruct`
application,  see the  [dedicated guide](@ref  usingflreconstruct) for
further details.

Available Experiments {#usingflsimulate_experiments}
=====================

The currently only available experiment in `flsimulate` is:

- `demonstrator`

As of version 4.0 of the geometry and its associated variant system, it includes
two flavours of the general layout of the detector:
- `Basic` : realistic model of the full detector
- `HalfCommissioning` : realistic model of the detector with only one calorimeter wall assemblied with
   one tracker submodule and no source frame  (*French* side, end 2016-begin 2017 context).

Each of these layouts publish additional options.


Available Vertex/Event Generators {#usingflsimulate_generator_table}
=================================

The architecture of `flsimulate`  separates the specification of where
events  are generated  (geometry) from  the specification  of how  the
events  are  generated  (decays).  With  several  different  geometry
layouts  being modelled  and many  background, signal  and calibration
physics sources available, a wide range of vertex and event generators
are available.

From  Falaise 3.0  (simulation version  2.1), the  choice of  geometry
options  and   vertex  and   event  generators   is  done   through  a
Bayeux/datatools   variant   service   embedded  in   the   flsimulate
application.  A profile of variant  parameters must be created to suit
the user's needs (see sections above).

Note  that `flsimulate`  will  throw  an exception  if  you supply  an
unknown  generator for  the  experimental setup  being simulated.   In
principle,  the variant  system  does check  that  the combination  of
vertex  and  event  generator  are sensible.   Other  invalid  options
(geometry...)  should also be reported.


Available output profiles {#usingflsimulate_output_profiles}
=========================

By default FLSimulate produces collections of *truth* MC hits and stored
them  in  the  output  data model  (the  \ref  mctools::simulated_data
class).

For the  `Demonstrator` configuration  using the `Basic`  layout, four
*official* collections of truth hits are thus populated:

- `calo` : truth MC hits collected from the scintillator blocks of the main calorimeter
- `xcalo` : truth MC hits collected from the scintillator blocks of the X-calorimeter
- `gveto` : truth MC hits collected from the scintillator blocks of the gamma veto
- `gg` : truth MC hits collected from the drift volume of the tracker cells (Geiger regime)

Additional  collections of  hits can  be generated  : the  *detailed* MC
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
                      (shortcut for the `calo_details+tracker_details+source_details` rule)

The  activation   of  some   additional  output   is  done   with  the
\@simulation:output_profile variant  parameter. It  may be  limited by
geometry options, for example the `source_details` is not available with the
`HalfCommissioning` geometry layout.

Be aware that using this feature  implies that the simulation will use
additional CPU  and the output  file will use  a lot of  storage. This
option should  thus be reserved  for dedicated studies,  debugging
and visualization  purpose, not  for production  of large  datasets of
simulated data. It is invalidated when used with the `production` user context.

The description of the output simulation  data model is described in a
[dedicated page on FLSimulate output](@ref flsimulateoutput).


Example {#usingflsimulate_example}
=======

Falaise  provides  an  example  of FLSimulate  configuration.   It  is
published   from    the   installation   resource    directory   in
`examples/flsimulate/ex01/`.
