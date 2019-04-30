Writing FLReconstruct Modules {#writingflreconstructmodules}
=============================

\tableofcontents

Introduction to the writing of FLReconstruct modules {#introduction}
========================================================================

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref usingflreconstruct).

FLReconstruct uses a [pipeline pattern](http://en.wikipedia.org/wiki/Pipeline_%28software%29) to process events. You can view this as a production line with each stage on the line
performing some operation on the event. Each stage in the pipeline is called
a "pipeline module" (or just "module") and is implemented as a C++ class.
The FLReconstruct application can load new modules at runtime using a ["plugin" mechanism](http://en.wikipedia.org/wiki/Plug-in_%28computing%29). Scripting, as demonstrated in the
[tutorial on using FLReconstruct](@ref usingflreconstruct), is used to load new modules from plugins,
select the modules to use in the pipeline, and configure each module.

In this tutorial we will see how to implement our own modules for
use in the FLReconstruct pipeline. This will cover

1. Writing a basic C++ module class
2. Compiling the class into a plugin for use by FLReconstruct
3. Scripting for use of the plugin by FLReconstruct
4. Implementing runtime module configuration

Getting your module to actually do something with the events that are
passed to it is deferred to [a later tutorial](@ref workingwitheventrecords).

Implementing a Minimal flreconstruct Module {#minimalmodule}
===========================================
Creating the Module Source Code {#minimalmodule_sources}
-------------------------------
We begin by creating an empty directory to hold the source code for
our example module, which we'll name "MyModule"

```console
$ cd MyWorkSpace
$ mkdir MyModule
$ cd MyModule
$ ls
$
```

You are free to organise the source code under this directory as you see
fit. In this very simple case we will just place all files in the `MyModule`
directory without any subdirectories. We start by creating the implementation file,
for the C++ class, which we'll name `MyModule.cpp`

\include flreconstruct/MyModule/MyModule.cpp

Here we can see the minimal interface and infrastructure required by a module class for `flreconstruct`.
The class must implement:

- A default constructor
- A non-default constructor taking parameters:
  - `falaise::config::property_set const&`, the configuration supplied to the module by the pipeline script
  - `datatools::service_manager&`, `flreconstruct`'s service provider
- A public member function `process` taking a single `datatools::things&` input
  parameter, and returning a @ref falaise::processing::status enumeration.

To make the plugin we'll build from this code loadable by `flreconstruct` we must also use the
@ref FALAISE_REGISTER_MODULE macro, passing it the class's typename. This will
also become a string that can be used to create a module of this type in
an `flreconstruct` pipeline script.

The non-default constructor is responsible for initializing the module using,
if required, the information supplied in the @ref falaise::config::property_set
and @ref datatools::service_manager objects. Our basic module doesn't require
any configuration or service information so we simply ignore these arguments.
Later tutorials will cover [module configuration](@ref minimalconfigurablemodule)
and [use of services by modules](@ref usingservices).

The `process` member function performs the actual
operation on the event, which is represented by a @ref datatools::things
instance. It is passed via non-const reference so `process` can both read and
write data to the event. As noted above, [a later tutorial](@ref workingwitheventrecords)
will cover the interface and use of @ref datatools::things. We therefore don't do anything
with the event, and simply write a message to standard output so that we'll be able to
see the method being called in `flreconstruct`. `process` **must** return a
[processing exit code](@ref falaise::processing::status). In this case,
our processing is always successful, so we return `falaise::processing::status::PROCESS_OK`.

Building the Loadable Shared Library {#minimalmodulebuilding}
----------------------------------
With the source code for `MyModule` in place we need to build a shared
library from it that `flreconstruct` can load at runtime to make
`MyModule` usable in a pipeline. As `MyModule` uses components
from Falaise, the compilation needs to use its headers, libraries and dependencies.
The simplest way to set this up is to use [CMake](https://cmake.org) to build the
shared library and make use of Falaise's [find_package](https://cmake.org/cmake/help/v3.9/command/find_package.html) support.

To do this, we add a CMake script alongside the sources:

```console
$ ls
MyModule.cpp
$ touch CMakeLists.txt
$ ls
CMakeLists.txt MyModule.cpp
$
```

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
linking of the `MyModule` target will use the correct compiler and
linker flags for use of Falaise. The `flreconstruct` application makes a default set of
libraries available, and if you require use of additional ones, CMake
must be set up to find and use these. This is documented [later in this tutorial](@ref additionallibraries).

For more detailed documentation on CMake, please refer to its
[online help](https://cmake.org/cmake/help/latest/).

To build the library, we first create a so-called *build directory* to hold
the files generated by the compilation to isolate them from the source code.
This means we can very quickly delete and recreate the build without worrying
about deleting the primary sources (it also helps to avoid accidental commits of
local build artifacts to Git!). This directory can be wherever you
like, but it's usually most convenient to create it alongside the
directory in which the sources reside. In this example we have the directory
structure:

```console
$ pwd
/path/to/MyWorkSpace
$ tree .
.
`-- MyModule
  |-- CMakeLists.txt
  `-- MyModule.cpp

1 directory, 2 files
$
```

so we create the build directory under `/path/to/MyWorkSpace` as

```console
$ mkdir MyModule-build
$ tree .
.
|-- MyModule
|  |-- CMakeLists.txt
|  `-- MyModule.cpp
`-- MyModule-build

2 directories, 2 files
$
```

The first step of the build is to change into the build directory and
run `cmake` to configure the build of `MyModule`:

```
$ cd MyModule-build
$ cmake -DCMAKE_PREFIX_PATH=/where/Falaise/is ../MyModule
```

Here, the `CMAKE_PREFIX_PATH` argument should be the directory under which Falaise
was installed. If you installed Falaise using `brew` and are using the `snemo-shell`
environment then you will not need to set this.
The last argument `../MyModule` points CMake to the directory holding the
`CMakeLists.txt` file for the project we want to build, in this case our
custom module.

Running the command will produce output that is highly system dependent,
but you should see something along the lines of

```console
$ cmake -DCMAKE_PREFIX_PATH=/where/Falaise/is ../MyModule
-- The C compiler identification is AppleClang 10.0.0.10001044
-- The CXX compiler identification is AppleClang 10.0.0.10001044
-- Check for working C compiler: /Library/Developer/CommandLineTools/usr/bin/cc
-- Check for working C compiler: /Library/Developer/CommandLineTools/usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /Library/Developer/CommandLineTools/usr/bin/c++
-- Check for working CXX compiler: /Library/Developer/CommandLineTools/usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
...
-- Configuring done
-- Generating done
-- Build files have been written to: /..../MyModule-build
$
```

The exact output will depend on which compiler and platform you are using.
However, the last three lines are common apart from the path, and indicate a
successful configuration. Listing the contents of the directory shows
that CMake has generated a Makefile for us:

```console
$ ls
CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
$
```

To build the library for our module we simply run `make`:

```console
$ make
Scanning dependencies of target MyModule
[ 50%] Building CXX object CMakeFiles/MyModule.dir/MyModule.cpp.o
[100%] Linking CXX shared library libMyModule.dylib
[100%] Built target MyModule
$
```

If the build succeeds, we now have the shared library present in our
build directory:

```console
$ ls
CMakeCache.txt   CMakeFiles     Makefile      cmake_install.cmake libMyModule.dylib
$
```

Note that the extension of the shared library is platform dependent (`.dylib` for Mac, `.so` on Linux).
With the library built, we now need to make `flreconstruct` aware of
it so we can use `MyModule` in a pipeline.


Running flreconstruct With a Custom Module {#minimalmodulerunning}
------------------------------------------
To use our new module in `flreconstruct` we need to tell the application
about it before using it in a pipeline. We do this through the pipeline
script we pass to `flreconstruct` via

1. Adding a new section named `flreconstruct.plugins` which tells `flreconstruct` about libraries to be loaded.
2. Adding a section declaring our module

We create a script named `MyModulePipeline.conf` in our project directory:

```console
$ pwd
/path/to/MyWorkSpace/MyModule
$ ls
CMakeLists.txt MyModule.cpp MyModulePipeline.conf
```

This script takes the same basic form as shown in the [tutorial on using flreconstruct](@ref usingflreconstruct):

\include flreconstruct/MyModule/MyModulePipeline.conf

The `plugins` key in the `flreconstruct.plugins` section is a list of
strings naming the libraries to be loaded by `flreconstruct` at startup.
These are taken as the "basename" of the library, from which the full
physical file to be loaded, `lib<basename>.{so,dylib}`, is constructed.
`flreconstruct` only searches for plugin libraries in its builtin
location by default, so custom modules must set the `<basename>.directory` property to tell
it the path under which their `<basename>` library is located.

In the above example, `MyModule.directory : string ="."` tells `flreconstruct`
to look in the current working directory, i.e. the directory from which it was run, for the `MyModule`
plugin. This is convenient for testing a local build of a module, as we can run `flreconstruct`
directly from the build directory of our module and it will locate the library immediately.
You can also specify absolute paths, e.g.

```ini
[name="flreconstruct.plugins" type="flreconstruct::section"]
plugins : string[1] = "MyModule"
MyModule.directory : string = "/path/to/MyWorkSpace/MyModule-build"
```

or paths containing environment variables which will be expanded automatically, e.g.

```ini
[name="flreconstruct.plugins" type="flreconstruct::section"]
plugins : string[1] = "MyModule"
MyModule.directory : string = "${MYMODULE_PATH}"
```

With the loading of the custom module in place, we can use it in the
script as we did for the builtin modules. As we did in in the [trivial pipeline example for flreconstruct](@ref trivial_pipeline),
we can simply declare the main pipeline module as being of the `MyModule`
type, hence the line

```ini
[name="pipeline" type="MyModule"]
```

Note that the `type` key value must always be the full typename
of the module, as used in the @ref FALAISE_REGISTER_MODULE macro.
Remember that in `MyModule.cpp` we called the macro as:

```cpp
FALAISE_REGISTER_MODULE(MyModule);
```

thus `type` is just "MyModule".

We can now run `flreconstruct` with `MyModulePipeline.conf` as the pipeline
script. Because we've specified the location of the `MyModule`
library as the working directory, we first change to the directory in which this library resides,
namely our build directory. We also need to have a file to process, so
we run `flsimulate` first to create a simple file of one event (NB in
the following, we assume you have `flsimulate` and `flreconstruct` in your
`PATH`).

```console
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
```

We can see that `flreconstruct` loaded the `MyModule` library,
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
outputs a fixed message which can only be changed by modifying the
code and recompiling the module. In most use cases hard-coding like this
is sufficient, but if your module has parameters that may change
frequently (e.g. a threshold that requires optimization), it is easy
to make them configurable at runtime through the pipeline script.
To demonstrate this, we'll modify the `MyModule` class from earlier to
have a single `std::string` type data member and make this configurable
from the pipeline script.

Adding a Configurable Data Member {#minimalconfigurablemodulecpp}
---------------------------------
To add a configurable data member to `MyModule`, we modify the code
as follows:

\include flreconstruct/MyModuleConfigurable/MyModule.cpp

The key changes are:

1. `std::string` data member `message`
2. Use of the data member in the `process` member function
3. Use of the falaise::config::property_set instance `ps` passed to the user-defined
constructor to extract configuration information

Here, `message` is our configurable parameter, and is initialized in the `MyModule` constructor
using the @ref falaise::config::property_set::get member function. We supply `std::string` as
the template argument as that is the type we need, and `message` as the parameter ID to extract.
This ID does not have to match the name of the data member, but it is useful to do so for clarity.

As configuration is always done through the constructor, you can then use configured data members
just like any other. In this case we simply report the value of `message` to standard output in
the `process` member function.


Building a Loadable Shared Library for a Configurable Module
------------------------------------------------------------
No special build setup is needed for a configurable module, so you can
use the CMake script
[exactly as given for the basic module above](@ref minimalmodulebuilding).
If you've made the changes as above, simply rebuild!


Configuring MyModule from the Pipeline Script {#minimalconfigurablemodulescript}
----------------------------------------------
In the preceding section, we saw that module configuration is passed to
a module through an instance of the @ref falaise::config::property_set class.
This instance is created by `flreconstruct` for the module from the
properties, if any, supplied in the section of the pipeline script
defining the module. To begin with, we can use the pipeline script from
earlier to run the configurable module, simply adding the required string parameter
`message` to its section:

\include flreconstruct/MyModuleConfigurable/MyModulePipeline.conf

The key name `message` and its type must match that looked for by `MyModule`'s constructor
in the supplied @ref property_set. Allowed key/types and their mappings to C++
types are documented in [a later section](@ref configurationbestpractices).
The script can be run in `flreconstruct` as before:

```console
$ cd /path/to/MyWorkSpace/MyModule-build
$ ls
CMakeCache.txt cmake_install.cmake Makefile
CMakeFiles   libMyModule.so    MyModuleTest.brio
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::_init():467] Automatic loading of library 'MyModule'...
MyModule::process says 'hello'
$
```

We can see that the module has been run using the supplied value for the
parameter. To change the `message` parameter, we simply update its value,
e.g.

```ini
[name="pipeline" type="MyModule"]
message : string = "goodbye"
```
Having add the key, we can rerun with the updated pipeline script:

```console
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModulePipeline.conf
[notice:void datatools::library_loader::_init():467] Automatic loading of library 'MyModule'...
MyModule::process says 'goodbye'
$
```

and see that the parameter has been changed to the value defined in
the script. Keys are bound to the section they are defined in, so we can use the
same module type multiple times but with different parameters. For example,
try the following pipeline script:

\include flreconstruct/MyModuleConfigurable/MyModulePipeline2.conf

You should see each event being dumped, with the dumped info being
bracketed by the output from each `MyModule` instance, each with
different values of the message parameter.

Both `flreconstruct` and @ref property_set work together to check that needed parameters
are supplied and of the correct type. For example, if we *did not* supply the `message`
parameter:

\include flreconstruct/MyModuleConfigurable/MyModuleMissingParam.conf

then `flreconstruct` will error out and tell us what happened:

```console
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModuleMissingParam.conf
[notice:void datatools::library_loader::_init():467] Automatic loading of library 'MyModule'...
[fatal:falaise::exit_code FLReconstruct::do_pipeline(const FLReconstruct::FLReconstructParams &):156] Failed to initialize pipeline : initialization of module 'pipeline' (type 'MyModule') failed with exception:
- missing_key_error: property_set does not hold a key 'message'
- config:
`-- <no property>

$
```

Equally, if we supply the parameter but it has the wrong type:

```ini
[name="pipeline" type="MyModule"]
message : integer = 42
```

then a similar error would be reported:

```console
$ flreconstruct -i MyModuleTest.brio -p ../MyModule/MyModuleWrongType.conf
[notice:void datatools::library_loader::_init():467] Automatic loading of library 'MyModule'...
[fatal:falaise::exit_code FLReconstruct::do_pipeline(const FLReconstruct::FLReconstructParams &):156] Failed to initialize pipeline : initialization of module 'pipeline' (type 'MyModule') failedwith exception:
- wrong_type_error: value at 'message' is not of requested type
- config:
`-- Name : 'message'
    |-- Type  : integer (scalar)
    `-- Value : 42

$
```

Additional methods for configuration and validation are covered in the
[following section](@ref configurationbestpractices).



Best Practices for Module Configuration {#configurationbestpractices}
---------------------------------------
Whilst the ability to make modules configurable is extremely useful,
*you should aim to minimize the number of parameters your module takes*.
This helps to make the module easier to use and less error prone.
Remember that the modular structure of the pipeline means that tasks
are broken down into smaller chunks, so you should consider refactoring
complex modules into smaller orthogonal units.

An important restriction on configurable parameters is that they can only
be of types understood by @ref falaise::config::property_set and the underlying
@ref datatools::properties configuration language.


| C++ Type                           | `property_set` accessor                                  | properties script syntax          |
| ---------------------------------- | -------------------------------------------------------- | --------------------------------- |
| `std::string`                      | `auto x = ps.get<std::string>("key");`                   | key : string = "hello"            |
| `int`                              | `auto x = ps.get<int>("key");`                           | key : integer = 42                |
| `double`                           | `auto x = ps.get<double>("key");`                        | key : real = 3.14                 |
| `bool`                             | `auto x = ps.get<bool>("key");`                          | key : boolean = true              |
| `std::vector<std::string>`         | `auto x = ps.get<std::vector<std::string>>("key");`      | key : string[2] = "hello" "world" |
| `std::vector<int>`                 | `auto x = ps.get<std::vector<int>>("key");`              | key : int[2] = 1 2                |
| `std::vector<double>`              | `auto x = ps.get<std::vector<double>>("key");`           | key : real[2] = 3.14 4.13         |
| `std::vector<bool>`                | `auto x = ps.get<std::vector<bool>>("key");`             | key : bool[2] = true false        |
| @ref falaise::config::path         | `auto x = ps.get<falaise::config::path>("key");`         | key : string as path = "/tmp/foo" |
| @ref falaise::config::quantity_t   | `auto x = ps.get<falaise::config::length_t>("key");`     | key : real as length = 3.14 mm    |
| @ref falaise::config::property_set | `auto x = ps.get<falaise::config::property_set>("key");` | _see below_                       |

The last item handles the case of nested configurations, for example

```ini
[name="nested" type="NestedModule"]
a.x : int = 1
a.y : int = 3
b.x : int = 2
b.y : int = 4
```

The keys can be extracted individually from the resultant @ref falaise::config::property_set, e.g.

```cpp
auto x = ps.get<int>("a.x");
```

However, nested configurations typically imply structured data, with periods indicating the nesting level.
Each level can be extracted into its own set of properties, e.g.

```cpp
auto a = ps.get<falaise::config::property_set>("a"); // a now holds key-values x=1, y=3
auto b = ps.get<falaise::config::property_set>("b"); // b now holds key-values x=2, y=4
```

with subsequent handling as required. A restriction on nesting is that it _cannot_ support
configurations such as

```ini
[name="nested" type="BadlyNested"]
a : int = 1
a.x : real = 3.14
```

as the key "a" is ambiguous. You should not use this form in any case as it generally
indicates bad design.

When using @ref falaise::config::property_set, you have several methods to _validate_
the configuration supplied to your module. By _validation_, we mean checking the configuration
supplies:

1. The required parameters...
2. ... of the correct type ...
3. .. in the correct value range

All configuration and validation must be handled in the module's constructor, with exceptions
thrown if an validation check fails. The first two checks can be handled automatically by
@ref falaise::config::property_set through its `get` member functions.

Parameters may be _required_, i.e. there is no sensible default, or _optional_, i.e. where we
may wish to adjust the default. A required parameter is validated for
existence and correct type by the single parameter `get` member function, e.g.

```cpp
class MyModule {
 public:
  MyModule(falaise::config::property_set const& ps, datatools::service_manager&)
   : message( ps.get<std::string>("message") )
  {}
  // other code omitted
 private:
  std::string message;
};
```

If the `ps` instance does not hold a parameter "message", or holds it with a type other than `std::string`,
then an exception is thrown and will be handled automatically by `flreconstruct`.

An optional parameter is validated in the same way, but we use the two parameter form of `get`, e.g:

```cpp
class MyModule {
 public:
  MyModule(falaise::config::property_set const& ps, datatools::service_manager&)
   : myparam( ps.get<int>("myparam", 42) )
  {}
  // other code omitted
 private:
  int myparam;
};
```

Here, if the `ps` instance does not hold a parameter "myparam" then the `myparam` data member will
be initialized to `42`. If `ps` holds parameter "myparam" of type `int` then `myparam` will be set
to its value. If `ps` holds parameter "myparam" and it is _not_ of type `int`, then an exception is
thrown (and handled by `flreconstruct` as above). Both forms are particularly useful for parameters
that supply physical quantities such as lengths. See the documentation on Falaise's [System of Units](@ref falaise_units)
for further information on their use to assist with dimensional and scaling correctness.

Additional validation tasks such as bounds checking must be handled manually, and generally
within the body of the module's constructor. For example, if we have a required integer parameter
that must be even, we could validate this via:

```cpp
class MyModule {
 public:
  MyModule(falaise::config::property_set const& ps, datatools::service_manager&)
   : myparam( ps.get<int>("myparam") )
  {
    if(myparam%2 != 0) {
      throw std::out_of_range{"value for 'myparam' parameter is not even"};
    }
  }
  // other code omitted
 private:
  int myparam;
};
```

You should prefer to initialize parameter values in the constructor's initializer list, with
further validation, if required, in the constructor body. Errors must be handled by throwing
an exception derived from `std::exception`.



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

```cmake
# Find Falaise first, which ensures we use correct ROOT
find_package(Falaise REQUIRED)

# Find ROOT after Falaise, which guarantees use of same ROOT, but configure extra components
# in this case, TMVA.
find_package(ROOT REQUIRED TMVA)
```

The module can then be linked to the additional library by adding
it in the `target_link_libraries` command:

```cmake
target_link_libraries(MyModule PUBLIC Falaise::FalaiseModule ${ROOT_TMVA_LIBRARY})
```

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

