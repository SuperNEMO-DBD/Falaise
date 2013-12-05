Writing FLReconstruct Modules
=============================

\tableofcontents

Introduction
============

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref md_FLReconstruct).


The Minimal Module {#minimalmodule}
==================
Creating the Module Source Project
----------------------------------
Start with an empty directory

~~~~~
$ cd MyWorkSpace
$ mkdir MyModule
$ cd MyModule
$ ls
$
~~~~~

FLReconstruct modules must inherit from Bayeux's @ref dpp::base_module
abstract base class. This declares three pure abstract methods that
any concrete module must implement:

- @ref dpp::base_module::initialize
- @ref dpp::base_module::process
- @ref dpp::base_module::reset

In addition, to make the module loadable by `flreconstruct`, we need
to use a couple of macros to declare and define the loading and
registration mechanisms for us.

We therefore begin implementing our module by writing the header file,
which we'll name `MyModule.h`:

\include flreconstruct/MyModule/MyModule.h

Now add the implementation file, which we'll name `MyModule.cpp`

\include flreconstruct/MyModule/MyModule.cpp

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

