Unit Testing for Falaise using Catch
====================================

All functions and classes within Falaise must have accompanying unit tests
that exercise and verify the specifications of their interfaces. To help
developers easily write and run tests, Falaise uses the [Catch](https://github.com/philsquared/Catch)
framework for authoring C++ tests, and the [CTest](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html)
component of CMake to run them.

To enable the build and running of tests in Falaise, the CMake variable
`FALAISE_ENABLE_TESTING` must be set to `ON`. This may be done on the command
line via a `-D` argument to CMake or through the Curses/GUI CMake interfaces.
It can be set/unset either on the initial configuration, or in subsequent
reconfigurations.


Writing Catch-based Unit Tests for Falaise
==========================================

We defer documentation on writing unit tests with Catch to the comprehensive
<A HREF="https://github.com/philsquared/Catch/blob/master/docs/tutorial.md">Tutorial</A>
and
[Reference Guides](https://github.com/philsquared/Catch/tree/master/docs) available
on Catch's github site. However, there are a couple of coding and structure issues to be
aware of to ensure your tests integrate easily with Falaise.

To speed up compile times, Falaise precompiles a main program for Catch into the `FLCatch`
static library that is linked to tests together with the `Falaise` library itself. The structure
of Falaise's Catch tests thus only requires inclusion of the Catch header, and the `CATCH_CONFIG_MAIN`
macro should *not* be defined. Therefore most tests should take the form:

```cpp
#include "catch.hpp"   // Just the Catch API

#include "falaise/mymodule/myinterfacetotest.h"    // Declaration(s) for what needs testing

TEST_CASE("Test myinterface", "[falaise][mymodule]") {
...
}
```

As shown in the above example, you should also use Catch's
<A HREF="https://github.com/philsquared/Catch/blob/master/docs/test-cases-and-sections.md#tags">Tags facility</A>
to name cases according to the submodule they are part of the aspect of the interface they are testing.

BY default, the `main` program built for Catch testing does not run any of Falaise's
setup/teardown functions for self-location. If your tests require this functionality,
you should add the calls at the start/end of your test cases as needed, e.g.

```cpp
TEST_CASE("Test resources", "[falaise][mymodule]") {
  falaise::initialize();
  ...

  falaise::terminate();
}
```

Addition of the calls to Falaise's Catch-main program is being reviewed.


Integrating Tests into Falaise's Testing System
===============================================
Integrating Core `libFalaise` Tests
-----------------------------------
To let Falaise know about your Catch-based tests, simply add the file to the list of Catch tests declared
for the module in its `.cmake` file located in `source/falaise` (e.g. `source/falaise/snemo.cmake`)
In the transition to full Catch based tests, Falaise modules need to define two variables to list source
files for tests, as shown in the following example:

```cmake
... the "flmymodule_module.cmake" file...

# List old, non-Catch tests in this variable
set(FalaiseLibrary_TESTS
  ...
  test_myoldtest.cxx
  ..
  )

# List new Catch based tests in this one
set(FalaiseLibrary_TESTS_CATCH
  ...
  test_myinterfacetest.cxx
  ...
  )

```

As you migrate your tests to Catch, simply move the file from the old list
to the new. Falaise's CMake system will automatically pick up the test lists and compile
test programs for them named `falaise-<sourcefilename>`. All test executables are output to the
`BuildProducts/bin/fltests` subdirectory of Falaise's build directory.

Falaise's CMake system will take care of the compilation and addition of the executables to
the testing system.


Integrating Core Plugin Module Tests
------------------------------------
If your plugin module is a core module, i.e. located under a `modules/yourplugin` directory
in the source tree of Falaise, then Catch may also be used for testing. You can write your
tests exactly as described above using the catch guides, but to add them into the testing
system a few lines are needed in your CMake script. Say you have source code for your tests
in a file `test_myplugin_someclass.cxx`, then to compile this into a test to be run, add the
following commands to the CMake script that builds it:

```cmake
# Assumes this script is at the same directory level as the source file...

# Build the executable
add_executable(falaise-test_myplugin_someclass test_myplugin_someclass.cxx)

# Link it to Falaise and the Catch library that supplies main()
target_link_libraries(falaise-test_myplugin_someclass Falaise FLCatch)

# Declare the test to CTest so that the executable is run as part of testing
add_test(NAME falaise-test_myplugin_someclass
  COMMAND falaise-test_myplugin_someclass
  )

# Output the test executable to the dedicated binary directory for module tests
set_target_properties(${_testname}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
  )
```

The test will be run as part of the standard CTest run, and you free to set additional
properties on the test as your use case demands.


Running Falaise's Tests with CMake/CTest
========================================

When Falaise's testing is enabled, CMake will build the executables and add them to the
tests known by CTest. There are three main ways to run the tests after building Falaise
itself.

CMake will create a target in the buildtool that can be "built" to run all the tests and
report their success/failure. For single mode tools like Make and Ninja this is the `test`
target, which may be "built" by:

```console
$ make test
```

or

```console
$ ninja test
```

In IDEs like Xcode, a new build spec/template is created, usually named "RUN_TEST" or similar.
This may be built/run using the tools in the IDE.

The second way to run tests, independent of the buildtool, is to run CMake's [`ctest` executable](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html)
directly in Falaise's build directory. This is useful if you find tests are failing and wish to run individual tests
to inspect the failures in detail. Please consult the [ctest manual](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html)
for full details on the command line options to select individual tests and verbosity levels.

Finally, the test programs are fully functional command line executables, so may be run directly.
Tests with a Catch supplied main provide a command line interface for listing and working with
individual test cases in the program. Simply run

```console
$ ./BuildProducts/bin/fltests/<testname> --help
```

for a list of options.

