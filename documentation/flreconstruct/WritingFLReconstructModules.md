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

1. Writing a basic C++ module class
2. Compiling the class into a plugin for use by FLReconstruct
3. Scripting the loading and use of the plugin in FLReconstruct
4. Implementing runtime module configuration

Getting your module to actually do something with the events that are
passed to it is deferred to [a later tutorial](@ref workingwitheventrecords).

Implementing a Minimal flreconstruct Module {#minimalmodule}
===========================================
Creating the Module Source Code {#minimalmodule_sources}
-------------------------------
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
directory without any subdirectories. We start by creating the implementation file,
for the C++ class, which we'll name `MyModule.cpp`

\include flreconstruct/MyModule/MyModule.cpp

Here we can see the minimal interface and infrastructure required by a module class for `flreconstruct`.
The class must:

- A default constructor
- A constructor taking parameters:
  - `std::string const&`, the `name` given to the module in the `flreconstruct` script
  - `datatools::properties const&`, the set of properties set for the module
    in the pipeline script
  - `datatools::service_manager&`, `flreconstruct`'s service provider.
- A public member function `process` taking a single `datatools::things&` input
  parameter, and returning a @ref falaise::processing::status enumeration.

To make the compiled module loadable by `flreconstruct` we must also use the
@ref FALAISE_REGISTER_MODULE macro, passing it the class's typename, and a string
that can be used to create a module of this type in an `flreconstruct` pipeline
script. This key can be any string you like, but for clarity and uniqueness we strongly
recommend it match the class name.

The `process` member function will perform the actual
operation on the event, which is represented as a @ref datatools::things
class, and passed to the method via non-const reference.
As noted above, a later tutorial will cover the interface and use of
@ref datatools::things . We therefore don't do anything with the event,
and simply write a message to standard output so that we'll be able to
see the method being called in `flreconstruct`. `process`  **must** return a
[processing exit code](@ref falaise::processing::status). In this case,
our processing is always successful, so we return @ref falaise::processing::status::PROCESS_OK.

Building the Loadable Shared Library {#minimalmodulebuilding}
----------------------------------
With the source code for `MyModule` in place we need to build a shared
library from it that `flreconstruct` can load at runtime to make
`MyModule` available for use in the pipeline. As `MyModule` uses components
from Falaise, the compilation needs to use its headers,
libraries and dependencies available. The simplest way to set this up
is to use CMake to build the shared library and make use of Falaise's
[find_package](https://cmake.org/cmake/help/v3.9/command/find_package.html) support.

To do this, we add a CMake script alongside the sources:

~~~~~~
$ ls
MyModule.cpp
$ touch CMakeLists.txt
$ ls
CMakeLists.txt MyModule.cpp
$
~~~~~~

The implementation of `CMakeLists.txt` is very straightforward:

\include flreconstruct/MyModule/CMakeLists.txt

Comments begin with a `#`. The first two commands simply setup CMake and
the compiler for us. The `find_package` command will locate Falaise for us,
and we supply the `REQUIRED` argument to ensure CMake will
fail if a Falaise install cannot be found.
The `add_library` command creates the actual shared library.
Breaking the arguments to `add_library` down one by one:

1. `MyModule` : the name of the library, which will be used to create the
on disk name. For example, on Linux, this will output a library file `libMyModule.so`, and on Mac OS X a library file `libMyModule.dylib`.
2. `SHARED` : the type of the library, in this case a dynamic library.
3. `MyModule.cpp` : all the sources need to build the library.

Finally, the `target_link_libraries` command links the shared library to
Falaise's `Falaise::FalaiseModule` target. This ensures that compilation and
linking of the `MyModule` target will have the correct compiler and
linking setup. The `flreconstruct` application makes a default set of
libraries available, and if you require additional functionality CMake
must be set up to find and use these. This is documented [later in this tutorial](@ref additionallibraries).

For more detailed documentation on CMake, please refer to the
[online help](https://cmake.org/cmake/help/latest/).

To build the library, we first create a so-called *build directory* to
isolate the binary files from the source code. This means we can very quickly
delete and recreate the build without worrying about deleting the primary
sources (it also helps to avoid accidental commits of local build artifacts
to Git!). This directory can be wherever you
like, but it's usually most convenient to create it alongside the
directory in which the sources reside. In this example we have the directory
structure:

~~~~~~
$ pwd
/path/to/MyWorkSpace
$ tree .
.
`-- MyModule
  |-- CMakeLists.txt
  `-- MyModule.cpp

1 directory, 2 files
$
~~~~~~

so we can create the build directory under `/path/to/MyWorkSpace` as

~~~~~~
$ mkdir MyModule-build
$ tree .
.
|-- MyModule
|  |-- CMakeLists.txt
|  `-- MyModule.cpp
`-- MyModule-build

2 directories, 2 files
$
~~~~~~

The first step of the build is to change into the build directory and
run `cmake` to configure the build of `MyModule`:

~~~~~~
$ cd MyModule-build
$ cmake -DCMAKE_PREFIX_PATH=/where/Falaise/is ../MyModule
~~~~~~

Here, the `CMAKE_PREFIX_PATH` argument should be the directory under which Falaise
was installed. If you installed Falaise using `brew` and are using `cmake` from `brew`
then you will not need to set this.
The argument `../MyModule` points CMake to the directory holding the
`CMakeLists.txt` file for the project we want to build, in this case our
custom module.

Running the command will produce output that is highly system dependent,
but you should see something along the lines of

~~~~~~
$ cmake -DCMAKE_PREFIX_PATH=/where/Falaise/is ../MyModule
-- The C compiler identification is AppleClang 9.0.0.9000039
-- The CXX compiler identification is AppleClang 9.0.0.9000039
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
...
-- Configuring done
-- Generating done
-- Build files have been written to: /..../MyModule-build
$
~~~~~~

The exact output will depend on which compiler and platform you are using.
However, the last three lines are common (apart from the path), and indicate a
successful configuration. Listing the contents of the directory shows
that CMake has generated a Makefile for us:

~~~~~~~
$ ls
CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
$
~~~~~~~

To build the library for our module we therefore simply run make:

~~~~~~
$ make
Scanning dependencies of target MyModule
[100%] Building CXX object CMakeFiles/MyModule.dir/MyModule.cpp.o
Linking CXX shared library libMyModule.dylib
[100%] Built target MyModule
$
~~~~~~

If the build succeeds, we now have the shared library present in our
build directory:

~~~~~~
$ ls
CMakeCache.txt   CMakeFiles     Makefile      cmake_install.cmake libMyModule.dylib
$
~~~~~~

Note that the extension of the shared library is platform dependent (`dylib` for Mac, `so` on Linux).
With the library built, we now need to make `flreconstruct` aware of
it and to use it in the pipeline.


Running flreconstruct With a Custom Module {#minimalmodulerunning}
------------------------------------------
To use our new module in `flreconstruct` we need to tell the application
about it and then use it in the pipeline. We do this via the pipeline
script we pass to `flreconstruct` via

1. Adding a new section named `flreconstruct.plugins` which tells `flreconstruct` about libraries to be loaded.
2. Declaring a section for our module

We create a script named `MyModulePipeline.conf` in our project directory:

~~~~~~
$ pwd
/path/to/MyWorkSpace/MyModule
$ ls
CMakeLists.txt MyModule.cpp MyModulePipeline.conf
~~~~~~

This script takes the same basic form as shown in the [tutorial on using flreconstruct](@ref usingflreconstruct):

\include flreconstruct/MyModule/MyModulePipeline.conf

The `plugins` key in the `flreconstruct.plugins` section is a list of
strings naming the libraries to be loaded by `flreconstruct` at startup.
These are taken as the "basename" of the library, from which the full
physical file to be loaded `lib<basename>.{so,dylib}` is constructed.
By default, `flreconstruct` only searches for plugin libraries in its builtin
location, so custom modules must supply the `.directory` attribute to tell
it the path under which the library is located.

In the above example, `MyModule.directory : string ='.'` tells `flreconstruct`
to look in the current working directory, i.e. the directory from which it was run.
This is convenient for testing a local build of a module, as we can run `flreconstruct`
directly from the build directory of our module and it will locate the library immediately.
You can also specify absolute paths, e.g.

~~~~~~
[name="flreconstruct.plugins" type="flreconstruct::section"]
plugins : string[1] = "MyModule"
MyModule.directory : string = "/path/to/MyWorkSpace/MyModule-build"
~~~~~~

or paths containing environment variables which will be expanded automatically, e.g.

~~~~~~
[name="flreconstruct.plugins" type="flreconstruct::section"]
plugins : string[1] = "MyModule"
MyModule.directory : string = "${MYMODULE_PATH}"
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
the @ref FALAISE_REGISTER_MODULE macro called in the source code
for the module. Remember that in `MyModule.cpp` we called
the macro as:

~~~~~~{.cpp}
FALAISE_REGISTER_MODULE(MyModule,"MyModule");
~~~~~~

This is why we recommend registering your modules with a key equal
to their C++ typename. It makes it absolutely clear which module is
to be constructed and minimizes potential name clashes.

We can now run `flreconstruct` with `MyModulePipeline.conf` as the pipeline
script. Because we've specified the location of the `MyModule`
library as the working directory, we first change to the directory in which this library resides,
namely our build directory. We also need to have a file to process, so
we run `flsimulate` first to create a simple file of one event (NB in
the following, we assume you have `flsimulate` and `flreconstruct` in your
`PATH`).

~~~~~~
$ cd /path/to/MyWorkSpace/MyModule-build
$ ls
CMakeCache.txt CMakeFiles cmake_install.cmake libMyModule.dylib Makefile
$ flsimulate -o MyModuleTest.brio
....
$ ls
CMakeCache.txt cmake_install.cmake Makefile
CMakeFiles   libMyModule.dylib    MyModuleTest.brio
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():449] Automatic loading of library 'MyModule'...
MyModule::process called!
$
~~~~~~

So we can see that `flreconstruct` loaded the `MyModule` library,
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
have a single `std::string` type data member and make this configurable.

Adding a Configurable Data Member {#minimalconfigurablemodulecpp}
---------------------------------
To add a configurable data member to `MyModule`, we modify the code
as follows:

\include flreconstruct/MyModuleConfigurable/MyModule.cpp

The key changes are:

1. `std::string` data member `message`, in-class initialized with a sensible
default value
2. Use of the data member in the `process` member function
3. Use of the `ps` datatools::properties instance passed to the
`initialize` method to extract the requested value for data member, if any

The first two items are simply management tasks, and the third is where
the main configuration is performed.

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
CMakeCache.txt cmake_install.cmake Makefile
CMakeFiles   libMyModule.so    MyModuleTest.brio
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():449] Automatic loading of library 'MyModule'...
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
fudge_factor : real = 3.14
~~~~~~

where the key name `fudge_factor` must match that looked for in the
`MyModule::initialize` method. How to document parameters is covered in
[a later tutorial](@ref documentingflreconstructmodules).
The format of `datatools::properties` key entries is
described in the documentation of that class.

Having add the key, we can rerun with the updated pipeline script:

~~~~~~
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::init():449] Automatic loading of library 'MyModule'...
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


Using Additional Libraries in Your Module {#additionallibraries}
=========================================

The `flreconstruct` program provides the needed libraries to run core modules,
specifically the minimal set:

- Falaise
- Bayeux
- [Boost](https://www.boost.org)
 - filesystem
 - system
 - serialization
 - iostreams
 - regex
- [GSL](https://www.gnu.org/software/gsl/)
- [CLHEP](http://proj-clhep.web.cern.ch/proj-clhep/)
- [ROOT](https://root.cern.ch/doc/v612/modules.html)
 - Core
 - RIO
 - Hist
 - MathCore
 - Matrix
 - Net
 - Tree
 - Thread
- [Qt5 QtCore](https://doc.qt.io/qt-5.10/qtcore-index.html)

Linking your module to the `Falaise::FalaiseModule` target in `target_link_libraries`
ensures that your module uses the appropriate headers at compile time, and
the correct symbols at runtime. If your module requires use of additional libraries,
then you will need to get CMake to locate these and then link them to your module.

In the most common case of using non-core libraries from the ROOT package, then the `find_package`
step would be modified to:

~~~~~
# Find Falaise first, which ensures we use correct ROOT
find_package(Falaise REQUIRED)

# Find ROOT after Falaise, which guarantees use of same ROOT, but configure extra components
# in this case, TMVA.
find_package(ROOT REQUIRED TMVA)
~~~~~

The module can then be linked to the additional library by adding
it in the `target_link_libraries` command:

~~~~~
target_link_libraries(MyModule PUBLIC Falaise::FalaiseModule ${ROOT_TMVA_LIBRARY})
~~~~~

For other packages, `find_package` followed by `target_link_libraries`
can be used in the same fashion.

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
Falaise, for consistency and maintanability their code should use the [Falaise coding standards](https://github.com/SuperNEMO-DBD/Falaise/blob/develop/documentation/development/CodingStandards.md#)

