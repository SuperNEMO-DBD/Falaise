Using Services in FLReconstruct Modules {#usingservices}
=======================================

\tableofcontents

Introduction {#useservices_introduction}
============

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref usingflreconstruct). You should also be familiar with composing and using
pipeline modules, as covered in [Writing FLReconstruct Modules](@ref writingflreconstructmodules).

In FLReconstruct, modules are the basic "unit" for processing events.
Each event stores data relevant to that event, but creation, use, and interpretation
of this data may require other data which common to a set of events
(e.g. a detector run).

Common data is supplied through a *Services*.
Your module can access any services it needs via the @ref datatools::service_manager
reference supplied to its constructor by the flreconstruct
application. From this, you can get a smart pointer to a current
service.

In this tutorial, we will see how to implement a module requiring the
@ref snemo::geometry_svc service.

Implementing a Service-Aware Module {#useservices_module}
===================================

Source Code for Service-Awareness
---------------------------------
This tutorial builds on the basic module implemented in [Writing FLReconstruct Modules](@ref writingflreconstructmodules). You should
therefore have a workspace and module sources arranged as

~~~~~
$ cd MyWorkspace
$ ls
MyModule
$ cd MyModule
$ ls
CMakeLists.txt         MyModulePipeline.conf
MyModule.cpp           MyModulePipeline2.conf
$
~~~~~

The `CMakeLists.txt` build script and `.conf` pipeline scripts are
identical to the earlier tutorial. We begin by modifying the module's
implementation as follows:

\include flreconstruct/UseServicesModule/MyModule.cpp

The changes are pretty simple:

1. The service is held as a data member of type `snemo::service_handle<snemo::geometry_svc>`
   - @ref snemo::service_handle is the "smart pointer" holding the service implementation
   - @ref snemo::geometry_svc is the type of the service implementation we need.
2. The service is initialized by in `MyModule`'s constructor from the supplied @ref datatools::service_manager instance.
3. The service is used in the `process` member function, here for illustration only by
   calling the `tree_dump` member function of the held @ref snemo::geometry_svc instance.

You do not need to worry about checking that the service is correctly constructed and valid.
@ref snemo::service_handle will throw exceptions if:

- A valid service of the requested type cannot be constructed
- The service is accessed (e.g. `geosvc->tree_dump(std::cout)`) and is not valid

The `flreconstruct` pipeline with handle these errors and report them on the command line.

Building and Running a Service-Aware Module
-------------------------------------------

No special steps are needed to build or run a service-aware module,
so you can follow the [build instructions from the original example](@ref minimalmodulebuilding) and [run the module in the same way](@ref minimalmodulerunning).


Available Services {#useservices_servicelist}
==================
- @ref snemo::geometry_svc

