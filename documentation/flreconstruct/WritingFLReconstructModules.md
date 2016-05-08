Writing FLReconstruct Modules {#writingflreconstructmodules}
=============================

\tableofcontents

Introduction to the writing of FLReconstruct modules {#introduction}
========================================================================

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref usingflreconstruct).

FLReconstruct uses a [pipeline pattern](http://en.wikipedia.org/wiki/Pipeline_%28software%29) to process events. You can view this like a production
line with each stage on the line performing some operation on the event.
Each stage in the pipeline is called a "pipeline module" (or just "module")
and is implemented as a C++ class. The FLReconstruct application can load
new modules at runtime using a ["plugin" mechanism](http://en.wikipedia.org/wiki/Plug-in_%28computing%29). Scripting, as demonstrated in the
[tutorial on using FLReconstruct](@ref usingflreconstruct), is used to load new modules from plugins,
select the modules to use in the pipeline, and configure each module.

In this tutorial we will see how to implement our own modules for
use in the FLReconstruct pipeline. This will cover

1. Writing the basic C++ class for a pipeline class
2. Compiling the class into a plugin for use by FLReconstruct
3. Scripting the loading and use of the plugin in FLReconstruct
4. Implementing runtime module configuration

Getting your module to actually do something with the events that are
passed to it is deferred to [a later tutorial](@ref workingwitheventrecords).

Implementing a Minimal flreconstruct Module {#minimalmodule}
===========================================
Creating the Module Sources {#minimalmodule_sources}
---------------------------
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
**must** return a [processing exit code](@ref dpp::base_module::process_status).
In this case, our processing is always successful, so we
return @ref dpp::base_module::PROCESS_OK.

The [reset method](@ref dpp::base_module::reset) is used to reset any
configuration performed by the
 [initialize method](@ref dpp::base_module::initialize). In this example,
 there is no configuration, so all we do is is call the base class [_set_initialized](@ref dpp::base_module::_set_initialized) to mark it as reset. This marking
is required by the pipeline management system, so you should always call
this method at the end of your module's [reset method](@ref dpp::base_module::reset). Note especially the call to `reset` in the destructor, as the
 pipeline management system requires modules to be uninitialized at
 destruction.


Building a Loadable Shared Library {#minimalmodulebuilding}
----------------------------------
With the source code for `MyModule` in place we need to build a shared
library from it that `flreconstruct` can load at runtime to make
`MyModule` available for use in the pipeline. As `MyModule` uses components
from Bayeux, the compilation needs to have the path to the Bayeux headers
available. The simplest way to do this is to use CMake to build the
shared library and make use of Bayeux's [find_package](http://www.cmake.org/cmake/help/v2.8.10/cmake.html#command:find_package) support.

To do this, we add a CMake script alongside the sources:

~~~~~~
$ ls
MyModule.h  MyModule.cpp
$ touch CMakeLists.txt
$ ls
CMakeLists.txt MyModule.h MyModule.cpp
$
~~~~~~

The implementation of `CMakeLists.txt` is very straightforward:

\include flreconstruct/MyModule/CMakeLists.txt

Comments begin with a `#`. The first two commands simply setup CMake and
the compiler for us. The `find_package` command will locate Bayeux for us,
with the `REQUIRED` argument ensuring CMake will fail it Bayeux cannot be
found for any reason. The `include_directories` command uses a variable set
by the preceeding `find_package` command to ensure the compiler can locate
Bayeux's headers. Finally, the `add_library` and `set_target_properties`
commands are used to build and link actual library.
Breaking the arguments to `add_library` down one by one:

1. `MyModule` : the name of the library, which will be used to create the
on disk name. For example, on Linux, this will output a library file `libMyModule.so`, and on Mac OS X a library file `libMyModule.dylib`.
2. `SHARED` : the type of the library, in this case a dynamic library.
3. `MyModule.h MyModule.cpp` : all the sources need to build the library. The header is also listed so that it will show up in IDEs like Xcode.

The use of `set_target_properties` is restricted to Apple platforms, and
adds flags to pass to the linker when linking the `MyModule` library.
This is needed because the Apple dynamic linker requires all symbols (the
functions/classes in/used by the binary library) to be found at link time.
The `MyModule` class uses symbols from the Bayeux library, so strictly
speaking we should link `libMyModule` to `libBayeux`. However, as we will
load `libMyModule` into `flreconstruct`, we can rely on the latter to
provide these symbols for us (so called dynamic lookup). The extra link
flags therefore tell the Apple linker to use this runtime lookup
mechanism.

For more detailed documentation on CMake, please refer to the
[online help](http://www.cmake.org/cmake/help/documentation.html).

To build the library, we first create a so-called *build directory* so
that we can
isolate the binary files from the source code. This can be wherever you
like, but it's usually most convenient to create this alongside the
directory in which the sources reside. In this example we have the directory
structure:

~~~~~~
$ pwd
/path/to/MyWorkSpace
$ tree .
.
`-- MyModule
    |-- CMakeLists.txt
    |-- MyModule.cpp
    `-- MyModule.h

1 directory, 3 files
$
~~~~~~

so we can create the build directory under `/path/to/MyWorkSpace` as

~~~~~~
$ mkdir MyModule-build
$ tree .
.
|-- MyModule
|   |-- CMakeLists.txt
|   |-- MyModule.cpp
|   `-- MyModule.h
`-- MyModule-build

2 directories, 3 files
$
~~~~~~

The first step of the build is to change into the build directory and
run `cmake` to configure the build of `MyModule`:

~~~~~~
$ cd MyModule-build
$ cmake -DBayeux_DIR=/where/BayeuxConfig/is ../MyModule
~~~~~~

Here, the `Bayeux_DIR` argument should be the directory which holds the
file `BayeuxConfig.cmake`. This file is installed by Bayeux, and will
be located in the directory `<prefix>/lib/cmake/Bayeux-<bxversion>`, where
`<prefix>` is the root directory of your Falaise/Bayeux installation and <bxversion>
is the current Bayeux version.
The argument `../MyModule` points CMake to the directory holding the
`CMakeLists.txt` file for the project we want to build, in this case our
custom module.

Running the command will produce output that is highly system dependent,
but you should see something along the lines of

~~~~~~
$ cmake -DBayeux_DIR=/where/BayeuxConfig/is ../MyModule
-- The C compiler identification is GNU 4.3.4
-- The CXX compiler identification is GNU 4.3.4
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/MyWorkSpace/MyModule-build
$
~~~~~~

The last three lines are common (apart from the path), and indicate a
successful configuration. Listing the contents of the directory shows
that CMake has generated a Makefile for us:

~~~~~~~
$ ls
CMakeCache.txt  CMakeFiles  cmake_install.cmake  Makefile
$
~~~~~~~

To build the library for our module we therefore simply run make:

~~~~~~
$ make
Scanning dependencies of target MyModule
[100%] Building CXX object CMakeFiles/MyModule.dir/MyModule.cpp.o
Linking CXX shared library libMyModule.so
[100%] Built target MyModule
$
~~~~~~

If the build succeeds, we now have the shared library present in our
build directory:

~~~~~~
$ ls
CMakeCache.txt  CMakeFiles  cmake_install.cmake  libMyModule.so  Makefile
$
~~~~~~

With the library built, we now need to make `flreconstruct` aware of
it and to use it in the pipeline.


Running flreconstruct With a Custom Module {#minimalmodulerunning}
------------------------------------------
To use our new module in `flreconstruct` we need to tell the application
about it and then use it in the pipeline. We do this via the pipeline
script we pass to `flreconstruct` via

1. Adding a new section named `flreconstruct.plugins` which tells the application about libraries to be loaded.
2. Declaring a section for our module/

We create a script named `MyModulePipeline.conf` in our project directory:

~~~~~~
$ pwd
/path/to/MyWorkSpace/MyModule
$ ls
CMakeLists.txt  MyModule.cpp  MyModule.h  MyModulePipeline.conf
~~~~~~

This script takes the same basic form as shown in the [tutorial on using flreconstruct](@ref usingflreconstruct):

\include flreconstruct/MyModule/MyModulePipeline.conf

The `plugins` key in the `flreconstruct.plugins` section is a list of
strings naming the libraries to be loaded by `flreconstruct` at startup.
If these libraries contain modules, they will be automatically loaded.
By default, the string(s) you provide will be used as the library name,
and it will be searched for as a file named `libNAME.{so,dylib}` under

1. The current working directory,
2. The paths in `{DY}LD_LIBRARY_PATH`,
3. The system paths known to the dynamic linker.

Note that as of Mac OSX El Capitan, `DYLD_LIBRARY_PATH` is ignored and
cannot be used to set module lookup paths.
You can also explicitly specify the directory to look in for the library to
override the default lookup paths. For example, we built the `MyModule`
library in `/path/to/MyWorkSpace/MyModule-build`, so we can force
`flreconstruct` to look there, and only there, by doing

~~~~~~
[name="flreconstruct.plugins" type=""]
plugins : string[1] = "MyModule"
MyModule.directory : string = "/path/to/MyWorkSpace/MyModule-build"
~~~~~~

With the loading of the custom module in place, we can use it in the
script as we did for the builtin modules. As we did in in the [trivial pipeline example for flreconstruct](@ref trivial_pipeline),
we can simply declare the main pipeline module as being of the `MyModule`
type, hence the line

~~~~~~
[name="pipeline" type="MyModule"]
~~~~~~

Note that the `type` key value must always refer to the id with which the
module was registered, and this is determined by the second argument to
the @ref DPP_MODULE_REGISTRATION_IMPLEMENT macro called in the source code
for the module. Remember that when we wrote `MyModule.cpp` we called
  the macro as:

~~~~~~{.cpp}
DPP_MODULE_REGISTRATION_IMPLEMENT(MyModule,"MyModule");
~~~~~~

This is why we recommend that you register your modules with an id equal
to their C++ typename. It makes it absolutely clear which module is
to be constructed and minimizes potential name clashes.

We can now run `flreconstruct` with `MyModulePipeline.conf` as the pipeline
script. Because we're relying on the default lookup of the `MyModule`
library, we first change to the directory in which this library resides,
namely our build directory. We also need to have a file to process, so
we run `flsimulate` first to create a simple file of one event (NB in
the following, we assume you have `flsimulate` and `flreconstruct` in your
`PATH`).

~~~~~~
$ cd /path/to/MyWorkSpace/MyModule-build
$ ls
CMakeCache.txt  CMakeFiles  cmake_install.cmake  libMyModule.so  Makefile
$ flsimulate -n1 -o MyModuleTest.brio
....
$ ls
CMakeCache.txt  cmake_install.cmake  Makefile
CMakeFiles      libMyModule.so       MyModuleTest.brio
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():410] Automatic loading of library  'MyModule'...
...
Reader configuration parameters:
|-- Name : "files.mode"
|   |-- Type  : string (scalar)
|   `-- Value : "single"
`-- Name : "files.single.filename"
    |-- Type  : string (scalar)
    `-- Value : "MyModuleTest.brio"
MyModule::process called!
$
~~~~~~

So we can see that `flreconstruct` loaded the `MyModule` library for us,
and the `MyModule::process` method was called, showing that the pipeline
used our custom module! We can also add our module into a chain pipeline
and other pipeline structures. For example, try the following pipeline script:

\include flreconstruct/MyModule/MyModulePipeline2.conf

You should see each event being dumped, with the dumped info being bracketed
by the `MyModule::process called!` text from each of the `MyModule`
instances in the chain.


Making Your Module Configurable {#minimalconfigurablemodule}
===============================
The minimal module presented in [the section above](@ref minimalmodule)
outputs a fixed message which can only be changed by modifiying the
code and recompiling the module. In most use cases hard-coding like this
is sufficient, but if your module has parameters that may change
frequently (e.g. a threshold that requires optimization), it is easy
to make them configurable at runtime through the pipeline script.
To demonstrate this, we'll modify the `MyModule` class from earlier to
have a single `double` type data member and make this configurable.

Adding a Configurable Data Member {#minimalconfigurablemodulecpp}
---------------------------------
To begin with we simply add the declaration of the data member in the
header file:

\include flreconstruct/MyModuleConfigurable/MyModule.h

and add extra code in the implementation file to handle the management
of the member:

\include flreconstruct/MyModuleConfigurable/MyModule.cpp

The key additions are:

1. Initializer for member in `MyModule` constructor.
2. Reset of member to default value in `reset` method.
3. Use of the `myConfig` datatools::properties instance passed to the
`initialize` method to extract requested value for data member.

The first two items are simply management tasks, and the third is where
the main configuration is performed.

The use of the `DT_THROW_IF` macro
is used to enforce one-time initialization of the module so that we can't
accidently and silently override the configuration. This locking
behaviour is not required, but is useful to prevent such accidental
overwrites.

The try/catch block is used to "bind" the value of the `fudge_factor`
string key from the `myConfig` `datatools::properties` instance to the
actual `fudgeFactor_` data member. This is wrapped in a try/catch block
because `datatools::properties` will throw a `std:logic_error` exception if

1. The key cannot be found.
2. The value of the key cannot be converted to the requested type (in this
case `double`).

We don't do anything if an exception is thrown here as we are
happy in this case to use the default value for `fudgeFactor_` if the
configuration does not override it or fails otherwise.
Different error handling strategies can be applied to meet the needs of
your own configurable parameters. You should consult the documentation
for `datatools::properties` for full details of its key checking and
  extraction API.

An important restriction on configurable parameters is that they can only
be of types understood by the `datatools::properties` class, namely:

- `std::string`
- `int`
- `double`
- `bool`
- `std::vector` of all above types.

Building a Loadable Shared Library for a Configurable Module
------------------------------------------------------------
No special build setup is needed for a configurable module, so you can
use the CMake script
[exactly as given for the basic module above](@ref minimalmodulebuilding).
If you've made the changes as above, simply rebuild!


Configuring MyModule from the Pipeline Script {#minimalconfigurablemodulescript}
----------------------------------------------
In the preceeding section, we saw that module configuration is passed to
the module by an instance of the `datatools::properties` class.
This instance is created by `flreconstruct` for the module from the
properties, if any, supplied in the section of the pipeline script
defining the module. To begin with, we can use the pipeline script from
earlier to run the configurable module:

\include flreconstruct/MyModule/MyModulePipeline.conf

and run it in `flreconstruct` with

~~~~~~
$ cd /path/to/MyWorkSpace/MyModule-build
$ ls
CMakeCache.txt  cmake_install.cmake  Makefile
CMakeFiles      libMyModule.so       MyModuleTest.brio
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():410] Automatic loading of library  'MyModule'...
...
Reader configuration parameters:
|-- Name : "files.mode"
|   |-- Type  : string (scalar)
|   `-- Value : "single"
`-- Name : "files.single.filename"
    |-- Type  : string (scalar)
    `-- Value : "MyModuleTest.brio"
MyModule::process using fudgeFactor(1)
$
~~~~~~

We can see that the module has been run using the default value of the
parameter. The section of the pipeline script defining `MyModule` is
the line

~~~~~~
[name="pipeline" type="MyModule"]
~~~~~~

so to change the `fudgeFactor_` parameter, we simply add the appropriate
`datatools::properties` key for it to the section:

~~~~~~
[name="pipeline" type="MyModule"]
fudge_factor : real = 3.14;
~~~~~~

where the key name `fudge_factor` must match that looked for in the
`MyModule::initialize` method. How to document parameters is covered in
[a later tutorial](@ref documentingflreconstructmodules).
The format of `datatools::properties` key entries is
described in the documentation of that class.

Having add the key, we can rerun with the updated pipeline script:

~~~~~~
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():410] Automatic loading of library  'MyModule'...
...
Reader configuration parameters:
|-- Name : "files.mode"
|   |-- Type  : string (scalar)
|   `-- Value : "single"
`-- Name : "files.single.filename"
    |-- Type  : string (scalar)
    `-- Value : "MyModuleTest.brio"
MyModule::process using fudgeFactor(3.14)
$
~~~~~~

and we see that the parameter has been changed to the value defined in
the script. Try changing the value to see the effect.

Keys are bound to the section they are defined in, so we can use the
same module type multiple times but with different parameters. For example,
try the following pipeline script:

\include flreconstruct/MyModuleConfigurable/MyModulePipeline2.conf

You should see each event being dumped, with the dumped info being
bracketed by the output from each `MyModule` instance, each with
different values of the fudge factor parameter.


Whilst this ability to make modules configurable is extremely useful,
*you should aim to minimize the number of parameters your module takes*.
This helps to make the module easier to use and less error prone.
Remember that the modular structure of the pipeline means that tasks
are broken down into smaller chunks, so you should consider refactoring
complex modules into smaller orthogonal units.

Next Steps
==========
The above examples have illustrated the basic structures needed to
implement a module and load it into `flreconstruct`.

Practical modules will access the event object passed to them, process it
and then write information back into the event record. [Using the event data model in modules is covered in a dedicated tutorial](@ref workingwitheventrecords).

Modules may also need access to global data such as run conditions.
FLReconstruct uses the concept of "Services" to provide such data,
and [a tutorial on using services in modules is provided](@ref usingservices).

Modules should also always be documented so that users have a clear
picture of the task performed by the module and its configurable
parameters. [A tutorial on documenting modules using the builtin Falaise/Bayeux tools](@ref documentingflreconstructmodules) is available.

Though modules for FLReconstruct may not be directly integrated in
Falaise, for consistency and maintanability their code [must follow the Falaise coding conventions](@ref codingstandards).

