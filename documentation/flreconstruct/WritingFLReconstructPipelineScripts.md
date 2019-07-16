Writing FLReconstruct Pipeline Scripts {#writingflreconstructpipelinescripts}
======================================

\tableofcontents

Introduction to FLReconstruct Pipeline Scripts {#wflrps_intro}
================================================
Though Falaise supplies a set of pipelines for standard reconstruction, it
is also possible write your own custom pipeline scripts for use in `flreconstruct`. These are nothing more than text files marked up using the `datatools::multi_properties` syntax.
Custom pipeline scripts can be run in `flreconstruct` by passing their path
to the `-p` (or `--pipeline`) command, for example

~~~~~
$ flreconstruct -i example.brio -p myscript.conf
~~~~~

or

~~~~~
$ flreconstruct -i example.brio -p /home/me/myscript.conf
~~~~~

The major use cases for custom pipelines are to study performance
improvements to the current pipelines by
tuning parameters or implementing new modules. Here we cover the task of
writing a script to build a custom pipeline from the standard modules supplied
with Falaise. The more advanced step of writing and
using new modules is covered in the [Writing FLReconstruct Modules](@ref writingflreconstructmodules) section.

Discovering Available Modules {#wflrps_discoveringavailablemodules}
=============================
A list of pipeline modules known to `flreconstruct` can be obtained using its
`--help-module-list` argument, which will print the module names to stdout:

~~~~~
$ flreconstruct --help-module-list
Things2Root
dpp::chain_module
dpp::dummy_module
dpp::dump_module
dpp::if_module
dpp::input_module
dpp::output_module
dpp::skip_module
dpp::utils_module
mctools::simulated_data_input_module
snemo::processing::event_header_utils_module
snemo::processing::mock_calorimeter_s2c_module
snemo::processing::mock_tracker_s2c_module
snemo::reconstruction::cat_tracker_clustering_module
snemo::reconstruction::charged_particle_tracking_module
snemo::reconstruction::mock_tracker_clustering_module
snemo::reconstruction::sultan_tracker_clustering_module
snemo::reconstruction::trackfit_tracker_fitting_module
snemo::visualization::visu_toy_module
~~~~~

Details about the purpose of a module and the parameters you can supply
to configure it may be obtained by passing the module name as the argument of
the `--help-module`
command. This will print out detailed documentation, if it exists:

~~~~~
$ flreconstruct --help-module dpp::chain_module

======================================
Configuration of ``dpp::chain_module``
======================================

... further detailed output ...
~~~~~

This information can be used to select suitable modules and configurations
for your own pipeline. The following sections provide some simple examples
of pipeline scripts.


Implementing Pipeline Scripts {#wflrps_implementing_pipeline_scripts}
=============================
To demonstrate the basic syntax and structure of an `flreconstruct` pipeline
script, the following sections build up functionality from single modules
up to using a chain of plugin modules.

Trivial Pipeline {#trivial_pipeline}
----------------
If you do not supply a pipeline script to `flreconstruct`, it
will run a dumb pipeline that simple dumps each event to stdout.

We can reproduce this behaviour using the following simple script to
configure the pipeline as a single module:

\include flreconstruct/SimplePipeline.conf

The script is formatted as a `datatools::multi_properties` ASCII
file. Note the comments, especially:

* The required presence of the `@key_label` and `@meta_label` multi_properties flags
* The module constituting the pipeline must have the `name` key set to `pipeline`. FLReconstruct will use this to build the pipeline.

To try this out, copy the above text into a file, e.g.
`trivial_pipeline.txt` and run `flreconstruct` with it:

~~~~~
$ flreconstruct -i example.brio -p trivial_pipeline.txt
~~~~~

You should see the same output as the default case.

Creating a Chained Pipeline {#wflrps_chain_pipeline}
---------------------------
A pipeline with one module isn't very useful! In most cases we want to
plug together a sequence of modules, each performing a well defined
task on the event data.

Falaise supplies a special `chain_module` for chaining several modules
together. We can reproduce the same default dump behaviour using a
`chain_module` in a pipeline script as follows

\include flreconstruct/ChainPipeline.conf

Here, the `modules` key of the `pipeline` module takes a list of
modules that will form the `chain_module`.
To try this out, copy the above text into a file, e.g.
`single_chain_pipeline.txt` and run `flreconstruct` with it:

~~~~~
$ flreconstruct -i example.brio -p single_chain_pipeline.txt
~~~~~

You should see the same output as the default case.

Multi-Module Pipeline {#wflrps_multi_pipeline}
---------------------
A full chain pipeline can chain together 1 < N < X number of modules.
The previous example showed the basic construct of a chained pipeline
with a single module. We can of course go further and add further module
to the pipeline. For example, we can chain two dump modules together.
To distinguish these, we configure the modules to have different `title` and `indent` parameters.

\include flreconstruct/AdvancedChainPipeline.conf

The order in which the modules are processed is determined by the
order in which you list them in the `modules` key of the `dpp::chain_module`
configuration.

To try this out, copy the above text into a file, e.g.
`multi_chain_pipeline.txt` and run `flreconstruct` with it:

~~~~~
$ flreconstruct -i example.brio -p multi_chain_pipeline.txt
~~~~~

Try swapping the order of the modules to see what happens, and also try
adding further dump modules to the pipeline to experiment with the
sequence.

Using Plugin Modules {#wflrps_pluginmodules}
====================
Falaise supplies several modules as plugins, that is, functionality loaded
into `flreconstruct` after it starts running.

Going Further {#further}
=============
You can also write your own modules in C++ and plug them into the pipeline
to provide additional functionality. The proceedure for writing and
using new modules is covered in several stages, beginning with a [simple example](@ref writingflreconstructmodules).

Detailed Syntax Guide
==============
Basic Script Syntax {#usingflreconstruct_scriptingflsimulate_format}
-------------------

FLreconstruct scripts use the `datatools::multi_properties` format
from Bayeux. The script must begin with a mandatory header:

~~~~~~
#@description a short description of the reconstruction run
#@key_label  "name"
#@meta_label "type"
~~~~~~

The '`#@description`' line is optional but highly recommended.

Comments can be placed at any point in the script. Just prepend a sharp
('`#`') symbol to any comment line. You may also prepend a comment at
the end of a line:

~~~~~~
# This is a single commented line

{...some script command...} # Comment starts at the end of the line

# This is
# a commented
# block
~~~~~~

Note   that  lines   starting  with   `'#@`'  are   generally  special
meta-comments with embedded commands. They  should not be considered as
comments.  As a matter of rule,  the use of lines starting with '`#@`'
is reserved for system use.

After the header, the script contains *sections*. A section starts
with a section definition line with two identifiers:

- the *name* of the section,
- the *type* of the section (in `flreconstruct` scripts it *must be* `"flreconstruct::section"`).

The syntax is:
~~~~~~
[name="SectionName" type="flreconstruct::section"]

... section body ...
~~~~~~

The section body uses the `datatools::properties` format from Bayeux.  After the
definition line, a short description may be optionally provided thanks
to the '`#@config`' meta-comment:

~~~~~~
#@config a short description of the purpose of the section
~~~~~~

Then comes  the section body which  consists in a list  of parameter
setting directives. The format is:

~~~~~~
#@description a short description of the parameter
NAME : TYPE [DECORATOR] = VALUE
~~~~~~

where `NAME` is the identifier for the parameter,  `TYPE` is its type and `VALUE` the
selected value for this parameter. Some parameters may use an optional
`DECORATOR` which gives additional  information about the parameter's
type or processing.  Again, the  `#@description` line is optional, but
recommended. Example:

~~~~~~
#@description The number of events to be processed
numberOfEvents : integer = 10000
~~~~~~


How to run a mock calibration on simulated events {#usingflreconstruct_mockcalibalgo}
-------------------------------------------------

When we generate simulated events  with FLSimulate, the so-called `SD`
bank  contains  only  raw  information about  truth  calorimeter  and
tracker  hits. In  order to  be able  to reconstruct  and analyze  the
events,  we  must  apply  a calibration  procedure.  This  calibration
procedure  consists of determining  physics/geometrical  observables
associated to hits : deposited energy, particle times of flight, hit
positions in the geometry model and so on.

The  so-called   *mock  calibration* computes the *calibrated data* associated
to raw *simulated data*.

![Mock calibration algorithms applied to simulated events](@ref flr_mock_calib.png)

The mock calibration consists of:

- Tracker hits: compute the drift radius and longitudinal position along the anode wire
of the Geiger avalanche (and associated uncertainties).
- Calorimeter hits: compute the total energy deposit in the scintillator block
and the reference time when the particles first interacted with the block (and associated uncertainties).

A new `CD` bank holding these data is added in the event record, alongside the raw `SD` bank.

To see this processing in action, we first prepare a set of 10 simulated events (using the default simulation setup),
using the following `simu.conf` script for FLSimulate:
~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flsimulate" type="flsimulate::section"]
numberOfEvents : integer = 10
~~~~~~~~~~~

and run:
~~~~~
$ flsimulate -c simu.conf -o example.brio
...
~~~~~

The resulting `example.brio` file can be opened with the `flvisualize`
application. We can visualize the first event where we will
see any raw tracker/calorimeter hits, as well as the  position of the decay vertex
(cross on the source foil). An example event is shown below,
and your view may differ as by default flsimulate will use different
seeds on each run, and the styling/colours may also vary between machines:

![A simulated event](@ref flr_qs_sd_event.png)

To process this data in `flreconstruct` and apply the mock calibration
we create  the `rec.conf` script with a  custom `pipeline` inline
module made of three consecutive modules. The first one is responsible
of the calibration of the tracker  hits. The second one is responsible
of  the calibration  of the  calorimeter  hits.  The  last one  simply
prints the  content of the  events in the  standard output:

~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="pipeline" type="dpp::chain_module"]
modules : string[3] = "CalibrateTracker" "CalibrateCalorimeters" "Dump"

[name="CalibrateTracker" type="snemo::processing::mock_tracker_s2c_module"]

[name="CalibrateCalorimeters" type="snemo::processing::mock_calorimeter_s2c_module"]

[name="Dump" type="dpp::dump_module"]

~~~~~~~~~~~

We  use   default  settings   for  each  of   the  `CalibrateTracker`,
`CalibrateCalorimeters`  and  `Dump`   modules  so  the  corresponding
sections are empty.


We then run:
~~~~~
$ flreconstruct -i example.brio -p rec.conf -o example-cd.brio
...
~~~~~

The use of the `Dump` module prints the events in the terminal. Here is the first event:
~~~~~
|-- Bank 'CD' : "snemo::datamodel::calibrated_data"
|   |-- Properties : <empty>
|   |   `-- <no property>
|   |-- Calibrated calorimeter hits: 1
|   |   `-- Hit #0 : Id=0 GID=[1302:0.1.16.6.*] E=1027.5 keV t=2.10165 ns
|   `-- Calibrated tracker hits: 23
|       |-- Hit #0 : Id=0 GID=[1204:0.1.0.95] [prompt]
|       |-- Hit #1 : Id=1 GID=[1204:0.1.1.96] [prompt]
|       |-- Hit #2 : Id=2 GID=[1204:0.1.2.96] [prompt]
|       |-- Hit #3 : Id=3 GID=[1204:0.1.2.97] [delayed]
|       |-- Hit #4 : Id=4 GID=[1204:0.1.3.97] [prompt]
|       |-- Hit #5 : Id=5 GID=[1204:0.1.4.98] [prompt]
|       |-- Hit #6 : Id=6 GID=[1204:0.1.5.98] [prompt]
|       |-- Hit #7 : Id=7 GID=[1204:0.1.5.99] [prompt]
|       |-- Hit #8 : Id=8 GID=[1204:0.1.6.99] [prompt]
|       |-- Hit #9 : Id=9 GID=[1204:0.1.6.100] [prompt]
|       |-- Hit #10 : Id=10 GID=[1204:0.1.7.100] [prompt]
|       |-- Hit #11 : Id=11 GID=[1204:0.1.7.101] [prompt]
|       |-- Hit #12 : Id=12 GID=[1204:0.1.8.101] [prompt]
|       |-- Hit #13 : Id=13 GID=[1204:0.1.8.102] [prompt]
|       |-- Hit #14 : Id=14 GID=[1204:0.1.0.94] [prompt]
|       |-- Hit #15 : Id=15 GID=[1204:0.1.1.94] [prompt]
|       |-- Hit #16 : Id=16 GID=[1204:0.1.2.94] [prompt]
|       |-- Hit #17 : Id=17 GID=[1204:0.1.3.94] [prompt]
|       |-- Hit #18 : Id=18 GID=[1204:0.1.4.93] [prompt]
|       |-- Hit #19 : Id=19 GID=[1204:0.1.5.93] [prompt]
|       |-- Hit #20 : Id=20 GID=[1204:0.1.6.93] [prompt]
|       |-- Hit #21 : Id=21 GID=[1204:0.1.7.93] [prompt]
|       `-- Hit #22 : Id=22 GID=[1204:0.1.8.93] [prompt]
`-- Bank 'SD' : "mctools::simulated_data"
    |-- Properties : <empty>
    |   `-- <no property>
    |-- Collection type : 1
    |-- Collections of step hit handles :
    |   |-- Category 'calo' has 1 hit(s) [capacity=1]
    |   `-- Category 'gg' has 25 hit(s) [capacity=25]
    |-- Primary event :
    |   |-- Auxiliary properties: <none>
    |   |-- Label : 'Se82.0nubb'
    |   |-- Time  : 0 ns
    |   |-- Vertex  : <none>
    |   |-- Particles: [2]
    |   |   |-- Particle #0 :
    |   |   |   |-- Generation Id  : <none>
    |   |   |   |-- Type           : 3 (label='e-')
    |   |   |   |-- PDG code       : <none>
    |   |   |   |-- Mass           : 0.510999 MeV
    |   |   |   |-- Charge         : -1 e
    |   |   |   |-- Time           : 0 ns
    |   |   |   |-- Kinetic energy : 1.1133 MeV
    |   |   |   |-- Momentum       : (0.706726,-0.130119,-1.36412) MeV
    |   |   |   |-- Vertex         : <no vertex>
    |   |   |   |-- Auxiliary properties: <none>
    |   |   |   `-- Valid          : 1
    |   |   `-- Particle #1 :
    |   |       |-- Generation Id  : <none>
    |   |       |-- Type           : 3 (label='e-')
    |   |       |-- PDG code       : <none>
    |   |       |-- Mass           : 0.510999 MeV
    |   |       |-- Charge         : -1 e
    |   |       |-- Time           : 0 ns
    |   |       |-- Kinetic energy : 1.8817 MeV
    |   |       |-- Momentum       : (0.0921589,0.473499,2.28718) MeV
    |   |       |-- Vertex         : <no vertex>
    |   |       |-- Auxiliary properties: <none>
    |   |       `-- Valid          : 1
    |   |-- GENBB weight : 1
    |   |-- Classification : '2e0p0g0a0X'
    |   `-- Valid: 1
    |-- Time : <none>
    `-- Vertex : (-0.0225505,1697.97,182.097) mm
...
~~~~~

We clearly see that a new  `CD` bank (*calibrated data*) has been added
to  the   event  record.   It   contains  two  collections   of  hits,
respectively   *calorimeter*   and   *tracker*  hits.

The `example-cd.brio` file output by `flreconstruct` and the pipeline
script may be opened in `flvisualize` as before. Looking at the first
event again, we can see additional rendering and information associated
to the `CD` hits (as before, your output will differ due to different
random number seeds used in `flsimulate`):

![A calibrated simulated event](@ref flr_qs_cd_event.png)

The superimposed  white circles on  top of the colored  ones represent
the  calibrated   tracker  hits  for   which  the  drift   radius  and
longitudinal position along the anode wire have been computed from the
truth hits' timestamps.  Here, there  is only one calorimeter hit, the
total  deposited  energy  in  the  block  and  time  of  entering  the
scintillator block  are also computed  (with respect to  the arbitrary
decay time set by the simulation engine).


How to run a tracking algorithm on simulated events {#usingflreconstruct_trackfitalgo}
---------------------------------------------------

Now we know how to perform the calibration step, we are interested in the reconstruction
of electron tracks in the tracking chamber. For that we need additional processing on top
of the `CD` bank which is now available in the event record.

The reconstruction of tracks associated to charged particles traversing the tracking chamber
is a two step procedure:

- first the calibrated Geiger hits are clusterized in *tracker clusters*, using some special vicinity criteria,
- then a fit is performed on each cluster to compute helix or line segments compatible with the Geiger hits.

![Track fitting procedure applied to calibrated events](@ref flr_track_fit.png)

We create a new `rec.conf` script with a  custom `pipeline` inline module
made of five consecutive modules. The first two perform the mock calibration
as shown in the previous section. Then we use a new module `CATTrackerClusterizer`
to run the clustering algorithm, followed by the `TrackFitting` module to run
the line/helix fitting on the found clusters. As before, we add a dump module
at the end of the pipeline to print each event to standard output.

As we now use modules implemented by dedicated Falaise plugins, we must explicitly
provide a `flreconstruct.plugins` section with the list of plugins that must
be dynamically loaded to allow the pipeline to work:

~~~~~~~~~~~
#@key_label  "name"
#@meta_label "type"

[name="flreconstruct.plugins" type="flreconstruct::section"]
plugins : string[3] = "Falaise_CAT" \
                      "TrackFit" \
                      "Falaise_TrackFit"

[name="pipeline" type="dpp::chain_module"]
modules : string[5] = \
  "CalibrateTracker" \
  "CalibrateCalorimeters" \
  "CATTrackerClusterizer" \
  "TrackFitting" \
  "Dump"

[name="CalibrateTracker" type="snemo::processing::mock_tracker_s2c_module"]

[name="CalibrateCalorimeters" type="snemo::processing::mock_calorimeter_s2c_module"]

[name="CATTrackerClusterizer" type="snemo::reconstruction::cat_tracker_clustering_module"]
TPC.processing_prompt_hits  : boolean = true
TPC.processing_delayed_hits : boolean = false

[name="TrackFitting" type="snemo::reconstruction::trackfit_tracker_fitting_module"]
fitting_models   : string[2] = "helix" "line"
line.only_guess  : string[4] = "BB" "BT" "TB" "TT"
helix.only_guess : string[8] = "BBB" "BBT" "BTB" "BTT" "TBB" "TBT" "TTB" "TTT"

[name="Dump" type="dpp::dump_module"]

~~~~~~~~~~~

We run:
~~~~~
$ flreconstruct -i example.brio -p rec.conf -o example-rec.brio
[notice:void datatools::library_loader::init():449] Automatic loading of library 'Falaise_CAT'...
[notice:void datatools::library_loader::init():449] Automatic loading of library 'TrackFit'...
[notice:void datatools::library_loader::init():449] Automatic loading of library 'Falaise_TrackFit'...
...
|-- Bank 'CD' : "snemo::datamodel::calibrated_data"
:   ...
|-- Bank 'SD' : "mctools::simulated_data"
:   ...
|-- Bank 'TCD' : "snemo::datamodel::tracker_clustering_data"
:   ...
`-- Bank 'TTD' : "snemo::datamodel::tracker_trajectory_data"
    ...
~~~~~

Now two banks have been added in the event records:
- `TCD` :  the *tracker  clustering data* contains  the result  of the
  tracker  hit  clusterization with  one  or  several solutions,  each
  containing a  collection of candidate  clusters of hit  cells. These
  clusters are used as the input of the track fitting algorithm
- `TTD` :  the *tracker  trajectory data* contains  the result  of the
  tracker fitting  with one  or several  solutions, each  containing a
  collection of candidate fitted tracks (helix or line).

The display shows the best found clusterization/fitting solutions with
two clusters  of tracker hits (red  and blue) and the  best associated
tracks  that  have been  computed  from  these clusters.   We  clearly
recognize  a  two  electrons  event  pattern, but  here  only  one  is
associated to a calorimeter block.

![A simulated event with reconstructed tracker clusters and fitted tracks](@ref flr_qs_trackfit_event.png)


To do {#usingflreconstruct_todo}
-----

Document more reconstruction steps:

- charged particle tracking module with extrapolation of vertex, impact points, curvature, track/calorimeter association...
- gamma tracking
- particle identification

Supported sections and parameters in FLReconstruct scripts {#usingflreconstruct_scriptingflsimulate_sectionsandparameters}
----------------------------------------------------------

The  FLReconstruct  script  contains  up   to  five  sections  of  type
`flreconstruct::section` with the following names:

- `flreconstruct` : this is the  system/base section where to set general
  parameters such as:

  - `numberOfEvents` : the number of  events to be processed from the input (integer,
    optional, default is: `0` which means *all* events will be processed),
  - `experimentalSetupUrn` : the experimental setup tag
    (default is: `urn:snemo:demonstrator:setup:1.0`),

- `flreconstruct.variantService`  :  this  is the  *variants*  section
  where  the  Bayeux  *variant  service*  dedicated  to  the
  management   of    variant   parameters   and    configurations   is
  configured. In principle, this section inherits the configuration of
  the variant service used to generate simulated data.

  Parameters of interest are:
  - `configUrn`  :  the  configuration  tag for  the  variant  service
	associated  to the  simulation setup  (string, optional).   If not
	set, it is automatically resolved from the experiment setup tag.
  - `config` : the path to the main configuration file for the variant
	service   associated  to   the   simulation  setup   (string/path,
	optional).   If not  set, it  is automatically  resolved from  the
	`configUrn` tag.
  - `profileUrn`  :  the configuration  tag  for  the variant  profile
	chosen by the  user to perform the  simulation (string, optional).
	If not set, it may  be automatically resolved from the `configUrn`
	tag if the variant configuration has a registered default profile.
  - `profile` : the path to the  variant profile chosen by the user to
	perform the simulation (string/path,optional).   If not set, it is
	automatically resolved from the `profileUrn` tag or from input
	metadata.
  - `settings` :  a list  of explicit  setting for  variant parameters
	chosen by  the user to  perform the simulation (array  of strings,
	optional).   If not  set, it  is automatically  resolved from  the
	`profileUrn` tag or from input metadata.

- `flreconstruct.services`  : this  is  the  *services* section  where
  explicit  configuration for  the embedded  Bayeux/datatools *service
  manager* is defined (by tag or explicit configuration file).

  Parameters of interest are:

  - `configUrn`  :  the  configuration  tag for  the  service  manager
	associated to the  data producer setup (string,  optional). If not
	set, it is automatically resolved from the experimental setup tag.
  - `config` : the path to the main configuration file for the service
	manager service  associated to  the reconstrcution  setup (string/path,
	optional).   If not  set, it  is automatically  resolved from  the
	`configUrn` tag.

- `flreconstruct.plugins`  :  this  is  the  *plugins*  section  where
  explicit  directives are  defined to  load Falaise  plugin libraries
  which define various types (classes) of processing modules.

  Parameters of interest are:

	- `plugins`  :  the  list of plugins to be loaded.
	- `PLUGIN_NAME.directory` :  the directory  from where  the plugin
      dynamic library  named `PLUGIN_NAME` should be  loaded (default:
      "@falaise.plugins:",   i.e.  the   standard  location   for  the
      installation of Falaise's plugins).

- `flreconstruct.pipeline`  : this  is  the  *pipeline* section  where
   general setup of the reconstruction pipeline is provided.

  Parameters of interest are:

	- `configUrn` : the tag of a registered/official pipeline.
	- `config` :  the main  configuration file describing  the modules
	  used  along  the pipeline.   If  not  set, it  is  automatically
	  resolved from the `configUrn` tag.
	- `module` :  the name  of the top  level pipeline  module, chosen
	  from the list of modules  defined in the main configuration file
	  (default: `"pipeline"`).

A sample configuration script showing the organisation of the above parameters
is shown below. Note that many of the parameters are commented out as
they are generally note needed except for advanced use or testing.

\include flreconstruct/FLReconstruct-3.0.0.conf

The majority of scripts will just use the `flreconstruct.plugins` and
`flreconstruct.pipeline` sections to select from the set of
plugins and pipelines approved by the Calibration and Reconstruction
Working Group. Output results from these can be used in the preparation
of analyses.

Inline modules {#usingflreconstruct_scriptingflsimulate_inlinemodules}
--------------

In the  case the `flreconstruct.pipeline`  section does not  define an
explicit pipeline configuration by  tag (`configUrn`) or configuration
file path (`config`),  it is possible to provide a  list of additional
sections which define reconstruction  modules. This technique is named
the *inline* pipeline mode. Such a section uses the following format:

~~~~~~~~
[name="ModuleName" type="ModuleType"]

... module's configuration parameters ...

~~~~~~~~

where `ModuleName` is  the unique name of the  module and `ModuleType`
is the identifier of its registered class type (see below).

This mode should not be used  for production runs. It is expected that
only  official  registered pipeline  setups  are  used in  such a case,
through  the `configUrn`  properties  in the  `flreconstruct.pipeline`
section.




