Contributing to Falaise Development {#developingfalaise}
===================================

\tableofcontents

Source Code Management
----------------------
Falaise source code is managed under [Git](https://git-scm.com), with a
[repository hosted on GitHub](https://github.com/SuperNEMO-DBD/Falaise) for
official release management, integration of contributions and issue tracking.
Please consult the main [Contribution Guide](@ref md_CONTRIBUTING)
for details on how to create your own working copy, branches and submit
Pull Requests. Please also see the [Quickstart Guide](@ref md_README)
for instructions on how to build and test Falaise from source.


Coding Style and Guidelines
---------------------------

Falaise uses C++ as its main implementation language, and currently
uses the C++11 standard. The standard references should be consulted for
further information on features and good practice:

- [Official ISO C++ Site](https://isocpp.org)
- [cppreference](http://en.cppreference.com/w/)
- [ISO C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

Falaise also implements a basic style and layout convention
to help provide uniform and readable source code. Please see the
[Falaise Style Guide](@ref codingstandards) for full details.


Compilation and Testing
-----------------------

Falaise uses the [CMake](https://cmake.org) build tool to describe and
generated scripts for compiling, testing and installing Falaise. Please
consult the main [CMake Documentation](https://cmake.org/documentation/) for information
on the system.

It is required that all code compiles without warnings on the main supported
platforms and compilers (macOS/Ubuntu/CentOS, clang/GCC). Warnings emitted by
inclusion of third party dependencies should be reviewed and if considered
benign, may be protected by the relevant `pragma` directives to supress
warnings.

All new code and triaged issues must have Unit Tests to exercise functionality
or reproduce the issue as needed. Falaise supplies the [Catch](https://github.com/philsquared/Catch)
library for authoring unit tests and fixtures. Please consult [Catch's documentation](https://github.com/philsquared/Catch/blob/master/docs)
for a basic guide to using its API. In Falaise, tests for the core
Falaise library should be placed in `source/falaise/testing` and the
source files added to the `FalaiseLibrary_TESTS_CATCH` CMake list in
`source/falaise/CMakeLists.txt`.

Documentation
-------------

Falaise uses [Doxygen](http://www.stack.nl/~dimitri/doxygen/) to mark up
and document all C++ code. All new code must be documented using this
system, and contributions that add or improve documentation to the existing
code are welcome.

User guides or more general documentation can be written in [Markdown](http://www.stack.nl/~dimitri/doxygen/manual/markdown.html)
to make writing and online presentation clearer.

