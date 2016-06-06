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
bipo3::processing::calorimeter_s2c_module
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
