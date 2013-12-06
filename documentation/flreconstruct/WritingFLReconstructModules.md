Writing FLReconstruct Modules
=============================

\tableofcontents

Introduction {#introduction}
============

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref md_FLReconstruct).

FLReconstruct uses a [pipeline pattern](http://en.wikipedia.org/wiki/Pipeline_%28software%29) to process events. You can view this like a production
line with each stage on the line performing some operation on the event.
Each stage in the pipeline is called a "pipeline module" (or just "module")
and is implemented as a C++ class. The FLReconstruct application can load
new modules at runtime using a ["plugin" mechanism](http://en.wikipedia.org/wiki/Plug-in_%28computing%29). Scripting, as demonstrated in the
@ref md_FLReconstruct tutorial, is used to load new modules from plugins,
select the modules to use in the pipeline, and configure each module.

In this tutorial we will see how to implement our own modules for
use in the FLReconstruct pipeline. This will cover

1. Writing the basic C++ class for a pipeline class
2. Compiling the class into a plugin for use by FLReconstruct
3. Scripting the loading and use of the plugin in FLReconstruct
4. Implementing runtime module configuration

Getting your module to actually do something with the events that are
passed to it is deferred to [a later tutorial](@ref md_WorkingWithEventRecords).

The Minimal Module {#minimalmodule}
==================
Creating the Module Source Project {#minimalmodule_sources}
----------------------------------
We begin by creating an empty directory to hold the source code for
our example module, which we'll name "MyModule"

~~~~~
$ cd MyWorkSpace
$ mkdir MyModule
$ cd MyModule
$ ls
$
~~~~~

You are free to organise the source code under this directory as you see
fit. In this very simple case we will just place all files in the `MyModule`
directory without any subdirectories.

A "pipeline module" (or just "module") in FLReconstruct is a C++ class
inheriting from Bayeux's @ref dpp::base_module abstract base class.
This declares three pure abstract methods that any concrete module **must**
implement:

- @ref dpp::base_module::initialize
- @ref dpp::base_module::process
- @ref dpp::base_module::reset

In addition, to make the module loadable by `flreconstruct`, we need
to use a couple of macros to declare and define the loading and
registration mechanisms for us.

We therefore begin implementing our module by writing the header file,
which we'll name `MyModule.h`:

\include flreconstruct/MyModule/MyModule.h

Note the inheritance from @ref dpp::base_module, declaration of the
three virtual methods and use of the macro
@ref DPP_MODULE_REGISTRATION_INTERFACE. Note also the use of Doxygen
markup to document the file and methods. You don't need to do this,
but it is very useful and helps if your module is to be integrated
into the official mainline pipeline.

With the header in place, we now add the implementation file,
which we'll name `MyModule.cpp`

\include flreconstruct/MyModule/MyModule.cpp

Here we've implemented the trivial constructor/destructor and the three
virtual methods, and added the @ref DPP_MODULE_REGISTRATION_IMPLEMENT
macro. This macro, together with the use of
@ref DPP_MODULE_REGISTRATION_INTERFACE
in the header file provide all the boilerplate needed to allow `MyModule`
to be loaded by `flreconstruct`. Note especially that @ref DPP_MODULE_REGISTRATION_IMPLEMENT
takes two arguments; the actual typename and a string key, the latter being
used to identify the module in pipeline scripts. In principle the key
can be anything you like, but for clarity and uniqueness we strongly
recommend it match the class name.

The [initialize method](@ref dpp::base_module::initialize) is used to
configure the module. At present `MyModule` does not require any
configuration, so all we do is call the base class [_set_initialized](@ref dpp::base_module::_set_initialized) to mark it as initialized. This marking
is required by the pipeline management system, so you should always call
this method at the end of your module's [initialize method](@ref dpp::base_module::initialize).

The [process method](@ref dpp::base_module::process) performs the actual
operation on the event, which is represented as a @ref datatools::things
class, and passed to the method as a @ref datatools::things reference.
As noted above, a later tutorial will cover the interface and use of
@ref datatools::things . We therefore don't do anything with the event,
and simply write a message to standard output so that we'll be able to
see the method being called in `flreconstruct`. The [process method](@ref dpp::base_module::process)
**must** return a [processing exit code](@ref dpp::processing_status_flags_type).
In this case, our processing is always successful, so we return @ref dpp::PROCESS_OK.

The [reset method](@ref dpp::base_module::reset) is used to reset any
configuration performed by the
 [initialize method](@ref dpp::base_module::initialize). In this example,
 there is no configuration, so all we do is is call the base class [_set_initialized](@ref dpp::base_module::_set_initialized) to mark it as reset. This marking
is required by the pipeline management system, so you should always call
this method at the end of your module's [reset method](@ref dpp::base_module::reset). Note especially the call to `reset` in the destructor, as the
 pipeline management system requires modules to be uninitialized at
 destruction.


Building a Loadable Shared Library
----------------------------------
With the source code in place we need to build a shared library from it
that `flreconstruct` can load at runtime to make `MyModule` available
for use in the pipeline. To do this we add a CMake script to describe the
build.

\include flreconstruct/MyModule/CMakeLists.txt


Running `flreconstruct` With a Custom Module
--------------------------------------------
To use our new module in `flreconstruct` we need to tell the application
about it. We do this by adding a new section named `flreconstruct.plugins`
to the pipeline script:

\include flreconstruct/MyModule/MyModulePipeline.conf

The `plugins` key is a list of strings naming the libraries to be loaded
by `flreconstruct` at startup. By default, the string you provide will
be used as the library name, and it will be searched for as a file
named `libNAME.{so,dylib}` under

1. The current working directory,
2. The paths in `LD_LIBRARY_PATH`,
3. The system paths known to the dynamic linker.

To override this behaviour you can also explicitly specify the directory
to look in for the library

~~~~~~
[name="flreconstruct.plugins" type=""]
plugins : string[1] = "MyModule"
MyModule.directory : string = "/path/to"
~~~~~~

and you can also specify a full path to the library

~~~~~~
[name="flreconstruct.plugins" type=""]
plugins : string[1] = "my_lib_id"
my_lib_id.full_path : string = "/path/to/libMyModule.so"
~~~~~~

Note that the keys you put into the `plugins` list can just be internal
names (`my_lib_id` in the above) if you use the `full_path` method to
tell `flreconstruct` where to find them. This is most useful to avoid
any accidental lookup of the library on `LD_LIBRARY_PATH` or the system
paths, as well as to load two different libraries with the same filename
(but you should of course prefer unique library names!).


Adding Configuration {#minimalconfigurablemodule}
====================
The minimal module presented in [the section above](@ref minimalmodule)
outputs a fixed message which can only be changed by modifiying the
code and recompiling the module. In most use cases hard-coding like this
is sufficient, but if your module has parameters that may change
frequently (e.g. a threshold that requires optimization), it is easy
to make them configurable at runtime through the pipeline script.

Whilst this configurability of modules is extremely useful, you should
aim to minimize the number of parameters for ease of use. Remember that
the modular structure of the pipeline means that tasks can be broken down
into smaller chunks.

