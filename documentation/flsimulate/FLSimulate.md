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
[FLReconstruct](@ref   usingflreconstruct)    and   [FLVisualize](@ref
usingflvisualize) applications.

Each FLSimulate simulation  setup is built on top  of an *experimental
setup*.  The experimental  setup is characterized by  a geometry model
and possibly other dedicated services (electronics...).

When initialized, the simulation setup triggers :
- the instantiation of a *variants  service* (for setting user options
  like geometry layout, vertex and decay generators...)
- the   instantiation   of   some  dedicated   *services*   (geometry,
  electronics...),
- the instantiation of a some *modules* dedicated to specific tasks:
	* the *simulation* module with some core components for vertex and
	  particles generation, particle tracking (Geant4 based engine),
	* the *digitization* module (not used yet),
	* the *output* module to save  output simulated data (and possible
      embedded metadata).

At  present,  FLSimulate only  supports  simulation  of the  SuperNEMO
demonstrator module, with several geometry and simulation *variants*.

The user is free to choose among several options:
- geometry  layout  of  the   detector:  `Basic`  (full  detector)  or
  `HalfCommissioning`
- activation of the magnetic field of not,
- use of the external shieling of not,
- change source material and thickness,
- use of calibration sources of not,
- use  of  a specific  vertex  generator,  which  may depends  on  the
  geometry layout,
- use of a specific event generator, which may depends on the geometry
  layout,
- use of specific simulation options.
Other options are also available (see section below).

Please contact the Software Working Group if you have any questions or
feature requests.

There   is    also   an    old   (legacy)   version    of   FLSimulate
(flsimulate_legacy) which does  not provide as many  features than the
current one.  It is deprecated and cannot support the new geometry and
variants system.

Using FLSimulate on the Command Line {#usingflsimulate_commandline}
====================================

FLSimulate is  implemented as a  simple command line  application just
like familiar  UNIX commands such as  `ls`. In the following,  we will
write commands  assuming that the  `flsimulate` executable is  in your
path.  If it  is not,  simply  use the  relative or  absolute path  to
`flsimulate`.

You can get help on the options that can be passed to `flsimulate`
by running it with the `-h` of `--help` options, e.g.

~~~~~
$ flsimulate --help
flsimulate (3.0.0) : SuperNEMO simulation program
Usage:
  flsimulate [options]
Options:
  -h [ --help ]                        print this help message
  --help-scripting                     print help on input script format and
                                       schema
  --help-simulation-setup              print help on simulation setup
  --version                            print version number
  -V [ --verbosity ] level             set the verbosity level
  -u [ --user-profile ] name (=normal) set the user profile ("expert",
                                       "normal", "production")
  -c [ --config ] file                 configuration script for simulation
  -m [ --output-metadata-file ] file   file in which to store metadata
  -E [ --embedded-metadata ] flag (=0) flag to (de)activate recording of
                                       metadata in the simulation results
                                       output file
  -o [ --output-file ] file            file in which to store simulation
                                       results
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

Note that the exact versions shown  will depend on the current release
and what versions of packages are linked.

Quick start {#usingflsimulate_quickstart}
===========

Here  we  propose  to  run  FLSimulate  with  the  default  simulation
setup. Only the output file is set from the command line:

~~~~~
$ flsimulate -o example.xml
...
~~~~~

The  output `example.xml`  file  contains one  unique simulated  event
corresponding to  a Se-82 (0nubb)  decay emitted from a  random source
pad (SuperNEMO Demonstrator geometry model 4.0 with `Basic` layout).
You  may  browse  the  output XML file  using  your  favorite  text
editor.    You   will    see    some   leading    records   of    type
`datatools::properties` corresponding  to the metadata.  Then  comes a
record of  type `datatools::things`. This data  structure contains the
unique simulated event with collections of truth hits.

You may display the event with:
~~~~~
$ flvisualize -i example.xml
...
~~~~~


Scripting FLSimulate {#usingflsimulate_scriptingflsimulate}
====================

FLSimulate basically runs the  simulation in batch mode.  Practically,
a *configuration script* must be generally provided by the user.

Script's format {#usingflsimulate_scriptingflsimulate_format}
---------------

The script  uses  the  Bayeux's `datatools::multi_properties`  format.
The script contains a mandatory header:

~~~~~~
#@description a short description of the simulation run
#@key_label  "name"
#@meta_label "type"
~~~~~~

The '`#@description`' line is optional but highly recommended.

You may add comments at any place  in the script. Just prepend a sharp
('`#`') symbol to any comment line.  You may also prepend a comment at
the end of a line. Examples:

~~~~~~
# This is a single commented line

{...some script command...} # Comment starts at the end of the line

# This is
# a commented
# block
~~~~~~

Note   that  lines   starting  with   '`#@`'  are   generally  special
meta-comment with embedded commands. They  should not be considered as
comments.  As a matter of rule,  the use of lines starting with '`#@`'
is reserved for system use.

After the header, the script contains *sections*. A section starts
with a section definition line with two identifiers:

- the *name* of the section,
- the *type* of the section (here it must be `"flsimulate::section"`).

The syntax is:
~~~~~~
[name="SectionName" type="flsimulate::section"]

... section's body ...
~~~~~~

The section's body uses the `datatools::properties` format.  After the
definition line, a short description may be optionally provided thanks
to the '`#@config`' meta-comment:

~~~~~~
#@config a short description of the purpose of the section
~~~~~~

Then comes  the section's body which  consists in a list  of parameter
setting directives. The format is:

~~~~~~
#@description a short description of the parameter
NAME : TYPE [DECORATOR] = VALUE
~~~~~~

where `NAME` is the parameter's name,  `TYPE` its type and `VALUE` the
selected value for this parameter. Some parameters may use an optional
`DECORATOR` which gives additional  informations about the parameter's
type or processing.  Again, the  `#@description` line is optional, but
recommended. Example:
~~~~~~
#@description The number of events to be generated
numberOfEvents : integer = 10000
~~~~~~


How to change the number of events simulated {#usingflsimulate_changenumberofevents}
--------------------------------------------

By default, FLSimulate generate only one event. To change this behaviour,
you must create a `simu.conf` script with the `flsimulate` section and
define the `numberOfEvents` integer parameter in it:
~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 100
~~~~~~~~~~~


How to select the event generator {#usingflsimulate_selecteventgenerator}
---------------------------------

FLSimulate use a default primary event generator which is generally
selected from the context of the selected simulation setup.
The choice of the event generator is handle by the embedded *variant service*
(`primary_events` registry).
You must create a `simu.conf` script with the `flsimulate.variantService` section and
define the `settings` string array parameter in it. This array must contain the proper
*variant* setting directive, namely:
~~~~~~~~~~~
primary_events:generator=GENERATOR_NAME
~~~~~~~~~~~
where `GENERATOR_NAME` is the name of a valid decay generator.

Example: to select the Tl-208 decay generator, use:
~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 100

[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[1] = "primary_events:generator=Tl208"
~~~~~~~~~~~


How to select the vertex generator {#usingflsimulate_selectevertexgenerator}
----------------------------------

FLSimulate use a default vertex generator which is generally
selected from the geometry context of the selected simulation setup.
The choice of the vertex generator is handle by the embedded *variant service*
(`vertexes` registry).
You must create a `simu.conf` script with the `flsimulate.variantService` section and
define the `settings` string array parameter in it. This array must contain the proper
*variant* setting directive, namely:
~~~~~~~~~~~
vertexes:generator=GENERATOR_NAME
~~~~~~~~~~~
where `GENERATOR_NAME` is the name of a valid vertex generator.

Example: to  select the generator of  vertice from the bulk  volume of
drift cells' field wire, use:

~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 100

[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[1] = "vertexes:generator=field_wire_bulk"
~~~~~~~~~~~

Note the  mandatory double quotes  ('`"`') around the  variant setting
string directive.

You may want to simultaneously  modify several variant parameters with
respect  to  the default  values.  You  must then  accumulate  setting
directives, here from the above recipes:

~~~~~~~~~~~
...
[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[2] = \
  "primary_events:generator=Tl208" \
  "vertexes:generator=field_wire_bulk"
~~~~~~~~~~~

You  should take  care  on the  size  of the  array  type between  the
brackets, i.e. `string[2]`.  It must  be compatible with the number of
setting directives  you pass to  the *variant service*. Note  also the
use of the backslash ('`\`') as the  last character on a line. It acts
as the continuation mark for a very long parameter definition line. In
this example, we  wrote one setting directive per line  (indented by a
few spaces) to make the script more readable.


How to select the geometry layout {#usingflsimulate_selectgeometrylayout}
---------------------------------

The  choice  of some  geometry  options  is  handled by  the  embedded
*variant   service*  (`geometry`   registry).   You   must  create   a
`simu.conf`  with the  `flsimulate.variantService` section  and define
the `settings` string array parameter in it, as explained above.  This
array must contain the proper *variant* setting directive, namely:

~~~~~~~~~~~
geometry:layout=LAYOUT_NAME
~~~~~~~~~~~

where `LAYOUT_NAME` is the name of a valid vertex generator. Supported
values are:
- `Basic` (default),
- `HalfCommissioning`.

Example: to select the *half-commissioning* layout, use:
~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 100

[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[1] = "geometry:layout=HalfCommissioning"
~~~~~~~~~~~

More geometry  options are available  from the *variant  service*, see
[this section](@ref usingflsimulate_variants).


How to choose the random numbers sequence {#usingflsimulate_chooserng}
-----------------------------------------

By default, FLSimulate initializes embedded pseudo random number generators
(PRNG) with a default seeding policy. Four seeds are internally chosen by a
smart algorithm and then passed to the core components of the simulation engine:

- the event generator,
- the vertex generator,
- the Geant4 Monte Carlo engine,
- the truth hit postprocessor(s).

To gain full control on the simulation process, you can select the seeds yourself.
You must create the `simu.conf` with the `flsimulate.simulation` section and
define the four seeding integer parameters:

~~~~~~~~~~~~~
[name="flsimulate.simulation" type="flsimulate::section"]
rngEventGeneratorSeed         : integer = 314159
rngVertexGeneratorSeed        : integer = 765432
rngGeant4GeneratorSeed        : integer = 123456
rngHitProcessingGeneratorSeed : integer = 987654
~~~~~~~~~~~~~

Beware to use a set of four different values to avoid correlation effects between
generators.

Another technique is available, it consists in providing an external file which contains the four seeds.
See [this section](@ref usingflsimulate_managingrandomnumbersequences) for further details.


Script's supported sections and parameters {#usingflsimulate_scriptingflsimulate_sectionsandparameters}
------------------------------------------

The  FLSimulate's  script  contains  up   to  five  sections  of  type
`flsimulate::section` with the following names:

- `flsimulate` : this is the  system/base section where to set general
  parameters.

  Parameters of interest are:

  - `numberOfEvents` : the number of  events to be generated (integer,
    optional, default is: `1`),
  - `doSimulation` : flag to  activate the simulation module (boolean,
    default is: `true`),
  - `doDigitization`  :  flag  to  activate  the  digitization  module
    (boolean, default is: `false`, not used yet).

- `flsimulate.simulation` : this is the *simulation* section where the
  simulation setup is chosen as  well as parameters for the management
  of pseudo random number generators (PRNG): seeds, logging about PRNG
  states.

  Parameters of interest are:

  - `simulationSetupUrn` : the simulation setup tag (string, mandatory
    for  production   runs,  otherwise   optional,  default   tag  is:
    `urn:snemo:demonstrator:simulation:2.1`).
  - `simulationSetupConfig`  : the  explicit  path  to the  simulation
    configuration   file  for   the  Bayeux/mctools   *Geant4  driver*
    (string/path, optional).  If not set, it is automatically resolved
    from the simulation setup tag.
  - `rngSeedFile` :  the input file  for the seeding of  random number
	generators (string/path, mandatory  for production runs, otherwise
	optional; if  this file  is not provided,  initial PRNG  seeds are
	automatically computed and saved in metadata or in a default log file).
  - `rngEventGeneratorSeed` : the explicit seed (integer) for the event generator.
  - `rngVertexGeneratorSeed` : the explicit seed (integer) for the vertex generator.
  - `rngGeant4GeneratorSeed` : the explicit seed (integer) for the Geant4 generator.
  - `rngHitProcessingGeneratorSeed` : the explicit seed (integer) for the hit post-processing algorithms.
  - `rngSeedFileSave` : the file path where to store the effective seeds used by the simulation.

- `flsimulate.digitization`  : this  is  the *digitization*  section
  (not used yet).

- `flsimulate.variantService` :  this is the *variants*  section where
  the Bayeux/datatools  *variant service* dedicated to  the management
  of  variant parameters  is  configured.  Users  are  given here  the
  opportunity to tweak some core options about:

  - the geometry,
  - the vertex generation,
  - the decay generation,
  - the simulation engine itself.

  Parameters of interest are:
  - `configUrn`  :  the  configuration  tag for  the  variant  service
	associated  to the  simulation setup  (string, optional).   If not
	set, it is automatically resolved from the simulation setup tag.
  - `config` : the path to the main configuration file for the variant
	service   associated  to   the   simulation  setup   (string/path,
	optional).   If not  set, it  is automatically  resolved from  the
	`configUrn` tag.
  - `profileUrn`  :  the configuration  tag  for  the variant  profile
	chosen by the  user to perform the  simulation (string, optional).
	If not set, it may  be automatically resolved from the `configUrn`
	tag is the variant configuration has a registered default profile.
  - `profile` : the path to the  variant profile chosen by the user to
	perform the simulation (string/path,optional).   If not set, it is
	automatically resolved from the `profileUrn` tag.
  - `settings` :  a list  of explicit  setting for  variant parameters
	chosen by  the user to  perform the simulation (array  of strings,
	optional).   If not  set, it  is automatically  resolved from  the
	`profileUrn` tag.

  See [this section](@ref usingflsimulate_variants) for details.


- `flsimulate.services`  :  this  is the  *services*  section  where
  explicit  configuration for  the embedded  Bayeux/datatools *service
  manager* is defined (by tag or explicit configuration file). Typical
  services are:

  - the  *geometry*   service  :  it  describes   the  geometry  model
	associated  to  the  experimental  setup   on  top  of  which  the
	simulation setup is built.
  - the  *electronics*  service (not  used  yet)  : it  describes  the
    electronics model associated  to the experimental setup  on top of
    which the simulation setup is built.

  Parameters of interest are:

  - `configUrn`  :  the  configuration  tag for  the  service  manager
	associated to  the simulation setup (string,  optional). If not set,
	it is automatically resolved from the simulation setup tag.
  - `config` : the path to the main configuration file for the service
	manager service  associated to  the simulation  setup (string/path,
	optional).   If not  set, it  is automatically  resolved from  the
	`configUrn` tag.

A sample configuration script  (commented) is provided in this
[document](FLSimulate-3.0.0.conf) (for Falaise 3.0.0).

For a  given run, the simulation  setup is selected from  the script's
`flsimulate.simulation` section.   Falaise uses  a special  service to
registered   blessed/official  configuration   (geometry,  simulation,
reconstruction...). Any  setup of interest  may be registered  with an
unique *tag*. The  tag is a simple character string  which uses an URN
scheme.  For  example, Falaise  version  3.0.0  is released  with  the
registered     simulation    setup     version     2.1    with     tag
`urn:snemo:demonstrator:simulation:2.1.     This   is    the   default
simulation setup  whcih will be  used if not explicitely  requested by
the user.

Thus, in the `flsimulate.simulation` section, the simulation setup tag
is optional and  defaults to: `urn:snemo:demonstrator:simulation:2.1`.
If  no other  parameters are  explicitly set,  FLSimulate will  try to
resolve all  other configuration components using  a dependency scheme
displayed on the table below:

~~~~~
+--urn:snemo:demonstrator:simulation:2.1 (simulation setup)
   +-- urn:snemo:demonstrator:setup:1.0 (related experimental setup)
   +-- urn:snemo:demonstrator:simulation:2.1:services (used service configuration)
   |   +-- urn:snemo:demonstrator:geometry:4.0 (used geometry model)
   +-- urn:snemo:demonstrator:simulation:2.1:variants (used variant configuration)
   |   +-- urn:snemo:demonstrator:simulation:2.1:variants:profiles:default (default variant profile)
   +-- urn:snemo:demonstrator:simulation:vertexes:4.1 (used vertex generation setup)
   +-- urn:snemo:demonstrator:simulation:decays:1.2 (used decay generation setup)
~~~~~

Effective  paths  to  various  configuration  files  are  automatically
resolved by a special service.

The          default          variant         profile,          namely
`urn:snemo:demonstrator:simulation:2.1:variants:profiles:default`,  is
associated to the default simulation setup. It implies:

- the `Basic` geometry layout (full) demonstrator module with external
  iron  shielding,  uniform magnetic  field  (25  Gauss), enriched  Se
  source pads...
- (0nubb) decays of Se-82,
- vertices generated from the bulk volume of random source pads.

To learn more about the format of the simulation configuration script,
use:

~~~~~
$ flsimulate --help-scripting
...
~~~~~

FLSimulate's variant system {#usingflsimulate_variants}
===========================

Basics
------

As  mentionned  above, FLSimulate  embeds  a  *variant service*  which
offers  to the  user  the  possibility to  tweak  an  official set  of
parameters from a  *variant repository*. A variant  *profile* can thus
be generated and imported by the FLSimulate's subsystems.

The variant repository is a container which typically contains several
sets  of variant  parameters, the  *variant registries*,  organized by
topics.

The  variant  registry  implements  a  dynamic  hierarchy  of  variant
parameters  and  depender  variant  menus.  It  is  organized  like  a
filesystem where each parameter and variant has an unique path.

As an illustration, here is the organization of the geometry registry
used by the SuperNEMO demonstrator simulation setup (2.1) (tag : `"urn:snemo:demonstrator:simulation:2.1"`):

~~~~~~~~~
geometry/
+-- layout (string)
|   +-- if_basic/
|   |   +-- magnetic_field (boolean)
|   |   |   +-- is_active/
|   |   |       +-- type
|   |   |           +-- if_mapped/
|   |   |           |   +-- map
|   |   |           |   |   +-- if_user/
|   |   |           |   |       +-- map_file
|   |   |           |   +-- z_inverted
|   |   |           +-- if_uniform_vertical/
|   |   |               +-- magnitude (real, as magnetic flux density)
|   |   |               +-- direction (string)
|   |   +-- source_layout (string)
|   |   |   +-- if_basic/
|   |   |       +-- thickness (real, as length)
|   |   |       +-- material (string)
|   |   +-- source_calibration (boolean)
|   |   |   +-- is_active/
|   |   |       +-- type (string)
|   |   +-- shielding (boolean)
|   +-- if_half_coommissioning/
|       +-- gap (real, as length)
+-- calo_film_thickness (real, as length)
~~~~~~~~~

The  top level  so-called `geometry`  item --  here displayed  with an
arbitrary trailing  slash ('`/`')  -- represents the  *registry* which
hosts all  variant parameters  related to  the geometry  model.  Items
*without* a trailing slash are  *variant parameters* (here their types
are shown between parenthesis).  They  may be given values selected by
the user, depending  on their type and constraints  implemented by the
variant service:  support for units,  range or enumeration  of allowed
values...  Items *with* a trailing  slash are specific *variant menus*
which are  activated only  if specific values  are selected  for their
parent parameter.   An activated variant  menu publishes a new  set of
variant parameters which  are thus made available for the  user from a
deeper level of the variant hierarchy.

FLSimulate registries
---------------------

In  the current  implementation  (Falaise 3.0.0  and simulation  setup
version 2.1),  four variant  *registries* are  defined in  the variant
repository:
- `geometry` : handles geometry options,
- `vertexes` : handles vertex generation options,
- `primary_events`: handles decays generation options,
- `simulation` : handles options for the Geant4 engine.

The    list    of    user     choices    is    described    in    this
[document](FLSimulateVariantsDoc-2.1.pdf) (for  SuperNEMO Demonstrator
simulation  setup version  2.1 and  Falaise 3.0.0).  It describes  all
available   simulation   variant   parameters   and   their   possible
dependencies in this simulation context.

Note that the  variant system tries to ensure  the consistence between
various choices.  For example,  the vertex generators from calibration
source (registry  `vertexes`) are  not available when  the calibration
sources are not arranged in  the source frame geometry model (registry
`geometry`).   A dependency  scheme  is thus  supported  to take  into
account such constraints.

The  lists of  valid vertex  and  event generators  for each  geometry
variant options can  also be dynamically browsed  through the Bayeux's
`flsimulate-configure` program  (with builtin  GUI mode of  your Bayeux
setup).

To get help from the command line, run:
~~~~~
$ flsimulate-configure --help
~~~~~

To launch a FlSimulate configuraion session, run:
~~~~~
$ flsimulate-configure
~~~~~

And then  inspect the options  available from the  `Vertex generation`
and  `Primary events`  panels.   Be  aware that  some  choices may  be
inhibited by the `Geometry` context.


The   image   below  shows   the   typical   GUI  interface   of   the
`flsimulate-configure`   variant    inspector/editor   program.    The
interface here displays the `geometry` registry panel:

![FLSimulate's variant inspector GUI (geometry panel)](@ref fls_variants_gui_1.png)
@latexonly
\includegraphics[width=\linewidth]{fls_variants_gui_1}
@endlatexonly

The next  image shows the selection  of the vertex generator  from the
`Vertex generation` registry panel:

![FLSimulate's variant inspector GUI (vertexes panel)](@ref fls_variants_gui_2.png)
@latexonly
\includegraphics[width=\linewidth]{fls_variants_gui_2}
@endlatexonly

Notice: You Bayeux setup should have been compiled with Qt GUI support to benefit
of the `flsimulate-configure`'s GUI interface.


Using explicit variant settings  {#usingflsimulate_variants_explicitsettings}
-------------------------------

The  `flsimulate.variantService` section  may contains  the `settings`
parameter.  It consists  in a  list of  assignment instructions  to be
passed to the variant service.

The format of a setting directive is:

~~~~~~~~~~~
REGISTRY_NAME:PATH_TO_VARIANT_PARAMATER=VALUE
~~~~~~~~~~~

where:
- `REGISTRY_NAME` is the name of the variant registry which hosts the selected parameter,
- `PATH_TO_VARIANT_PARAMATER` is the path of the target parameter in the registry directory,
- `VALUE` is the textual representation of the selected value for the target parameter.

For example, to simulate 100 Tl-208  events from the bulk volume of the
tracker cells' field  wires in the full Demonstrator,  you must create
the      following       `simu.conf`      script       (using      the
`datatools::multi_properties` format):

~~~~~
#@description Main flsimulate configuration script
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 100

[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[2] = \
  "vertexes:generator=field_wire_bulk" \
  "primary_events:generator=Tl208"

~~~~~

The `settings`  parameter must not  be used for production  runs.  One
should use an explicit variant profile file (`profile`) in place of it
(see  below)  or, preferably,  a  registered  variant profile's  *tag*
(`profileUrn`) .


Using a variant profile {#usingflsimulate_variants_profile}
-----------------------

The   `flsimulate-configure`   program   proposes  an   interface   to
edit/select  a  set  of  parameters   and  then  generate  a  *variant
profile*. A variant profile is a file which contains the explicit list
of variant parameters with their values selected for a simulation run.

~~~~~
$ flsimulate-configure -o "variants.profile"
~~~~~

Here  the   `variant.profile`  file  is  generated   at  exit  (option
`-o`).   This  *profile* file  stores  a  set of  variant
options (using a  specific format) from which the  variant service can
pickup  the parameters'  value  selected by  the  user. The  *profile*
contains:

~~~~~
#@format=datatools::configuration::variant
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
generator = "free_spot"
generator/if_free_spot/x = 0 mm
generator/if_free_spot/y = 0 mm
generator/if_free_spot/z = 0 mm

[registry="primary_events"]
generator = "electron.1MeV"

[registry="simulation"]
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
output_profile = "none"

~~~~~

The file  uses an ASCII  format to  ease user's understanding  and for
debugging purpose.  However, unless you  know what you are  doing, you
should  not edit  this file  manually (or  even reorder  parameters or
registry sections)  because its format  is dynamic and depends  on the
selected  options  by  the user  through  the  `flsimulate-configure`'s
interface.

This file  can be  reused as  input of  the `flsimulate`  program (see
below). The `simu.conf` script now contains:.

~~~~~
...
[name="flsimulate.variantService" type="flsimulate::section"]
profile : string as path = "variants.profile"
~~~~~

where  the   `variants.profile`  explicitely  publishes   all  variant
parameters that  have been chosen  by the user. The  following example
displays  the  default  variant  profile  associated  to  the  default
simulation setup (version 2.1) in Falaise 3.0.0:

~~~~~
#@format=datatools::configuration::variant
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

Available experiments and simulation setups {#usingflsimulate_experimentssimsetups}
===========================================

The  currently  only  available  experiment  in  `flsimulate`  is  the
SuperNEMO Demonstrator (tag=`"urn:snemo:demonstrator"`).

Only       one       experimental       setup       is       available
(tag=`"urn:snemo:demonstrator:setup:1.0"`).   It   consists   in   the
description of the SuperNEMO  demonstrator detector through a geometry
model identified with the `"urn:snemo:demonstrator:geometry:4.0"` tag
as shown above in the dependency table.

In  the future,  additional experimental  setups will  be implemented,
including not only the geometry model  but also the description of the
electronics (at least  the part of it which is  needed for the offline
software, digitization...).

As of  version 4.0 of  the geometry  model and its  associated variant
system, the  experimental setup includes  two flavours of  the general
layout of the detector:
- `Basic`  :  realistic  model   of  the  full  demonstrator  detector
  (*French*  and  *Italian*  sides,   source  frame  with  calibration
  sources, external shielding)
- `HalfCommissioning` : realistic model of  the detector with only one
  calorimeter wall assemblied with one tracker submodule and no source
  frame (only  *French* side,  like in the  context of  end 2016-begin
  2017).

Each of these layouts publishes additional options.


Available Vertex/Event Generators {#usingflsimulate_generator_table}
=================================

The architecture of `flsimulate`  separates the specification of where
events  are generated  (geometry) from  the specification  of how  the
events  are  generated  (decays).  With  several  different  geometry
layouts  being modelled  and many  background, signal  and calibration
physics sources available, a wide range of vertex and event generators
are available.

From  Falaise  3.0  (simulation  setup version  2.1),  the  choice  of
geometry  options  and  vertex/decay  generators  is  done  through  a
Bayeux/datatools   *variant  service*   embedded  in   the  FLSimulate
application.  A profile of variant  parameters must be created to suit
the user's needs (see sections above).

Note  that `flsimulate`  will  throw  an exception  if  you supply  an
unknown  generator for  the  experimental setup  being simulated.   In
principle,  the variant  system  does check  that  the combination  of
vertex  and  event generators  are  sensible.

Summary of available configurations {#usingflsimulate_summaryofavailableconfigurations}
===================================

Falaise   is  distributed   with   an  official   set  of   predefined
configurations of various types:
- experiments
- experimental setups
- geometry models
- simulation setups
- reconstruction pipelines

These  official  configurations are  registered  in  a dedicated  *URN
service*.   Each  configuration is  given  an  unique identifier:  its
*tag*.  It is  a character string which uses the  *URN* scheme format.
In principle, users can thus address, in a given context, any official
configuration  thanks to  its tag.   Some configurations  are possibly
linked by some dependency scheme.  Generally, a given simulation setup
is designed  to work with  a special experimental setup.   It wouldn't
work with another  one. For example, the default  simulation setup for
the SuperNEMO  demonstrator implies a collection  of vertex generators
taht  are not  compatible  with  the BiPo3  geometry  model. The  *URN
service* tries to formally handle such kind of constraints.

Once  selected,  a  tag  can  thus   be  associated  to  one  or  more
configuration  files  from  the   Falaise  resource  directory.   Such
operations are automatically handled by an *URN resolver service*.

Users  may  also  use  their  own  simulation  setups  and  associated
configuration  files. In  such case,  explicit paths  to configuration
files must be  provided in configuration scripts. This  mode should be
reserved for expert users.


List of available experiments {#usingflsimulate_summaryofavailableexperiments}
-----------------------------

1.  SuperNEMO demonstrator:
		* Description:  The SuperNEMO  demonstrator experiment  (all phases:
		  half-commissioning, full demonstrator)
		* Tag : `"urn:snemo:demonstrator"`
2.  BiPo3 detector (not used yet):
		*   Description: The BiPo3 detector
		*   Tag : `"urn:bipo3:detector"`

List of available experimental setups {#usingflsimulate_summaryofavailableexperimentalsetups}
-------------------------------------

1.  SuperNEMO demonstrator experimental setup version 1.0:
		*   Description: The model of the SuperNEMO demonstrator experimental setup
		*   Tag : `"urn:snemo:demonstrator:setup:1.0"`
		*   Related experiment: `"urn:snemo:demonstrator"`
		*   Associated to:
			+   Variant system: `"urn:snemo:demonstrator:setup:1.0:variants"`
			+   Services system: `"urn:snemo:demonstrator:setup:1.0:services"`
				- Geometry service: `"urn:snemo:demonstrator:geometry:4.0"`

List of available simulation setups {#usingflsimulate_summaryofavailablesimulationsetups}
-----------------------------------

1. SuperNEMO demonstrator simulation setup version  2.1 :
		*   Description: Simulation setup for the SuperNEMO demonstrator detector
		*   Tag : `"urn:snemo:demonstrator:simulation:2.1"`
		*   Based on:
			+   Experimental setup: `"urn:snemo:demonstrator:setup:1.0"`
			+   Vertex generation system: `"urn:snemo:demonstrator:simulation:vertexes:4.1"`
			+   Primary events generation system : `"urn:snemo:demonstrator:simulation:decays:1.2"`
		*   Associated to:
			+   Services system: `"urn:snemo:demonstrator:simulation:2.1:services"`
			+   Variant system: `"urn:snemo:demonstrator:simulation:2.1:variants"`
				- Blessed profiles: the default one below
				- Default profile: `"urn:snemo:demonstrator:simulation:2.1:variants:profiles:default"`


Available MC hits output profiles {#usingflsimulate_hits_output_profiles}
=================================

By  default FLSimulate  produces collections  of *truth*  MC hits  and
stored them in the output data model (the \ref mctools::simulated_data
class).

For  the SuperNEMO  Demonstrator  simulation  configuration using  the
`Basic` geometry layout, four *official* collections of truth hits are
thus populated:

- `calo` : truth MC hits collected from the scintillator blocks of the
  main calorimeter
- `xcalo` :  truth MC hits  collected from the scintillator  blocks of
  the X-calorimeter
- `gveto` :  truth MC hits  collected from the scintillator  blocks of
  the gamma veto
- `gg` : truth MC hits collected  from the drift volume of the tracker
  cells (Geiger regime)

Additional collections  of hits can  be generated : the  *detailed* MC
hits.   So far,  we  use one  unique collection  of  truth hits  named
`__visu.tracks`. The `__visu.tracks` collection  collects all *MC step
hits* that have been generated along particle tracks that crossed some
volumes of interest. To activate the recording of such output, several
*output profiles* have been made available.

For the `Demonstrator/Basic` configuration/layout, these are:

- `calo_details` : collect  all Geant4 step hits from  the calo, xcalo
  and gveto sensitive detectors
- `tracker_details` : collect all Geant4 step hits from within volumes
  in the tracker part of the detector
- `source_details` : collect all Geant4  step hits from within volumes
  in the source part of the detector
- `all_details`     : collect all Geant4 step hits from any volumes of
  interest (shortcut for the
  `calo_details+tracker_details+source_details` rule)

The  activation   of  some   additional  output   is  done   with  the
`simulation:output_profile` variant  parameter. It may be  limited by
geometry options,  for example the `source_details`  output profile is
not available with the `HalfCommissioning` geometry layout because the
source frame is not available in this layout.

Example: select the full detail output profile:

~~~~~
#@description Main flsimulate configuration script
#@key_label  "name"
#@meta_label "type"
[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[1] = "simulation:output_profile=all_details"

~~~~~


Be aware that using this feature  implies that the simulation will use
additional CPU  and the output  file will use  a lot of  storage. This
option should  thus be reserved  for dedicated studies,  debugging and
visualization  purpose,  not  for  production  of  large  datasets  of
simulated  data.

The description of the output simulation  data model is described in a
[dedicated page on FLSimulate output](@ref flsimulateoutput).


Managing random number sequences  {#usingflsimulate_managingrandomnumbersequences}
================================

As shown in the [above section](@ref usingflsimulate_chooserng), users
can take  full control on  random number sequences used  internally by
FLSimulate.

The   script   `flsimulate.simulation`  section   recognizes   several
configuration   parameters   concerning    PRNG   initialization   and
management.

For production runs, it is mandatory to use the `rngSeedFile`  parameter:

~~~~~~~~~~~~~
[name="flsimulate.simulation" type="flsimulate::section"]
#@config Simulation setup
...
#@description Random seeds external file
rngSeedFile : string as path = "seeds.conf"
...
~~~~~~~~~~~~~

The `seeds.conf` file is typically generated by the following command:
~~~~~
$ bxg4_seeds -k -d . -p seeds.conf
~~~~~

It uses the following format:

~~~~~
{EG=142921705; MGR=569932270; SHPF=1008320517; VG=1002945362}
~~~~~

Here we can see that FLSimulate  requests 4 seeds, one for each random
number generators embedded in the Bayeux/Geant4 simulation engine:
- `EG` stands for *event generator*,
- `VG` stands for *vertex generator*,
- `MGR` stands for *simulation manager* (Geant4 engine),
- `SHPF`  stands  for  *step   hit  processing  factory*,  a  software
  component responsible of  the generation of the  final collection of
  truth hits (see below).

It  is very  important  to  make sure  that  all  simulation runs  use
different sets of seeds in order to ensure statistical independancy of
generated simulation  data samples. The `bxg4_seeds`  program helps to
generated such  lists of  independant seed  sets in  the context  of a
massive Monte Carlo production.

If the  input seed  file is  not provided, the  initial seeds  will be
automatically generated  by the  seed manager embedded  in FLSimulate.

User may  also provide  the `rngSeedFileSave` parameter:  it describes
the file where to save the  original seeds, particularly when they are
not set explicitely through  `rngSeedFile` or `rngXXXGeneratorSeed` parameters.
By default, original seeds
will be saved in metadata of in the `__flsimulate-seeds.log` file in
the current directory. One can thus select explicitely this output file with:

~~~~~~~~~~~~~
[name="flsimulate.simulation" type="flsimulate::section"]
#@config Simulation setup
...
#@description File to save initial random seeds
rngSeedFileSave : string as path = "path/to/the/init-seeds.log"
...
~~~~~~~~~~~~~


Output data file {#usingflsimulate_outputdatafile}
================

The FLSimulate  output file is set  from the command line  through the
`-o`  or `--output-file`  switch.   It  may use  the  XML format  (for
debugging purpose) or Bayeux's Brio binary format (for production).

Simulation then runs with:

~~~~~
$ flsimulate -c simu.conf -o example.brio
... lots of logging ...
~~~~~

Here  the `example.brio`  file  contains the  Monte Carlo  generated
events.

The system automatically select the proper format driver from the file
extension.  Supported file extensions are:

- `.brio` : Boost  over Root I/O with embedded  portable binary format
  (fast and highly compressed, reserved for production)
- `.data`, `.data.gz`, `.data.bz2` :  Portable binary format (fast and
  possibly highly compressed, reserved for production)
- `.txt`, `.txt.gz`, `.txt.bz2` : Portable ASCII text format (slow and
  possibly highly compressed)
- `.xml`, `.xml.gz`, `.xml.bz2`  : Portable XML format  (very slow and
  possibly compressed, reserved for debugging)

The  resultant data  files can  be examined  with the  `flreconstruct`
application,  see the  [dedicated guide](@ref  usingflreconstruct) for
further details.

Note also that FLSimulate automatically computes some metadata besides
the simulated events.  These metadata are stored by default within the
output data file itself.  These metadata  can be reused in the context
of  the  data reconstruction  or  other  analysis  tools. It  is  also
possible  to  save the  metadata  container  within a  human  readable
companion file  (using the `datatools::multi_properties` format) using
the `--output-metadata-file` switch. Example:

~~~~~
$ flsimulate -c simu.conf -o example.brio --output-metadata-file example.meta
...
~~~~~

In case the user  does not choose to store the  metadata in the output
data  file and  no  explicit  external metadata  file  is selected,  a
default one is generated with name `__flsimulate-metadata.log`.

Output metadata {#usingflsimulate_outputmetadata}
===============

Each FLSimulate run produces a  set of *context sensitive* data, known
as *metadata*.   A set  of metadata  is thus  created just  before the
simulation run starts. It contains informations about the simulation's
configuration  setup  and  inputs,   arranged  in  sections  (general,
simulation, digitization,  variants, services...) in the  same way the
configuration script is organized. This  enables the off line check of
the simulation parameters.

As mentionned above,  metadata is saved by default in  the output data
file, through some kind of header (XML) or parallel branch (BRIO).  If
asked on  the command line (`-m`  or `--output-metadata-file` switch),
FLSimulate also saves metadata using the `datatools::multi_properties`
format   in   an   external   companion   file   (default   name   is:
`__flsimulate-metadata.log`).

User profiles {#usingflsimulate_userprofiles}
=============

Falaise proposes three *user profiles* for the Monte Carlo production:

- `expert`   :   reserved   for   *expert*  users   who   are   able   to
  hack/corrupt/distort the Falaise and FLSimulate systems thanks to a large
  set of options and features,
- `normal` : for *normal* users with a smaller set of options and features;
  this is the default profile,
- `production` : this mode should be reserved for official production of
  Monte Carlo data; some options and features are inhibited to prevent
  from generating data with ambiguous or non official configuration(s).

!TODO give details about what can/cannot be done within each user profile.

Examples {#usingflsimulate_examples}
========

Example 1 : using FLSimulate with a configuration script {#usingflsimulate_examples_example1}
--------------------------------------------------------

Here we want to simulate 10 Tl-208 events from the bulk volume of the
tracker cells' field wires in the full Demonstrator.

1. Prepare the following `simu.conf` script:
~~~~~
#@description Main flsimulate configuration script
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 10

[name="flsimulate.variantService" type="flsimulate::section"]
settings : string[3] = \
  "vertexes:generator=field_wire_bulk" \
  "primary_events:generator=Tl208" \
  "simulation:output_profile=all_details"
~~~~~
2. Run `flsimulate`:
~~~~~
$ flsimulate -c simu.conf -o example.brio
...
~~~~~
   Here we use the *brio* format. This binary format is not user friendly and we cannot
   display its contents with standard tools.
3. You may display the events with:
~~~~~
$ flvisualize -i example.brio
...
~~~~~


Example 2 : using FLSimulate with a configuration script and an explicit variant profile {#usingflsimulate_examples_example2}
----------------------------------------------------------------------------------------

Here we use the same simulation setup than in case 2:

1. Prepare the following `variant.profile` file:
~~~~~~
$ flsimulate-configure \
    --no-gui \
    -t urn:snemo:demonstrator:simulation:2.1 \
    -s "geometry:layout=Basic" \
    -s "vertexes:generator=field_wire_bulk" \
    -s "primary_events:generator=Tl208" \
    -s "simulation:output_profile=all_details" \
    -o "variant.profile"
~~~~~~
   Note the list of `--variant-set` switches used to select the options. We obtain the following profile:
~~~~~
#@format=datatools::configuration::variant
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
generator = "field_wire_bulk"

[registry="primary_events"]
generator = "Tl208"

[registry="simulation"]
physics_mode = "Constructors"
physics_mode/if_constructors/em_model = "standard"
production_cuts = true
output_profile = "all_details"

~~~~~
2. Prepare the following `simu.conf` script:
~~~~~
#@description Main flsimulate configuration script
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 10

[name="flsimulate.variantService" type="flsimulate::section"]
profile : string as path = "variant.profile"
~~~~~
3. Run `flsimulate`:
~~~~~
$ flsimulate -c simu.conf -o example.data.gz
...
~~~~~
4. Display the events:
~~~~~
$ flvisualize -i example.data.gz
...
~~~~~

Example 3 : using FLSimulate with an explicit configuration, profile and seeds {#usingflsimulate_examples_example3}
------------------------------------------------------------------------------

Here we use again the same simulation setup than in examples 1 and 2.

1. Generate a set of seeds:
~~~~~
$ bxg4_seeds -k -d . -T -p seeds.conf
~~~~~
2. Generate the `variant.profile` file:
~~~~~~
$ flsimulate-configure \
    --no-gui \
    -t urn:snemo:demonstrator:simulation:2.1 \
    -s "geometry:layout=Basic" \
    -s "vertexes:generator=field_wire_bulk" \
    -s "primary_events:generator=Tl208" \
    -s "simulation:output_profile=none" \
    -o "variant.profile"
~~~~~~
3. Prepare the following `simu.conf` script:
~~~~~
#@description Main flsimulate configuration script
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 10

[name="flsimulate.simulation" type="flsimulate::section"]
rngSeedFile : string as path = "seeds.conf"

[name="flsimulate.variantService" type="flsimulate::section"]
profile : string as path = "variant.profile"
~~~~~
4. Run `flsimulate` twice:
~~~~~
$ flsimulate -c simu.conf -o example.xml
...
$ flsimulate -c simu.conf -o example2.xml
...
~~~~~
5. Check that both generated files have strictly the same contents:
~~~~~
$ diff example.xml example2.xml
$ echo $?
0
~~~~~
6. Display the events:
~~~~~
$ flvisualize -i example.xml
...
~~~~~
