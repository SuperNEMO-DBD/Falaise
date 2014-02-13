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
Each event stores data relevant to that event, but use and interpretation
of this data may require other data which is "global"; that is, data common
to a set of events (e.g. a detector run). As this data is common to an
event set, it is not stored in the events received via the
@ref dpp::base_module::process method because this would cause

1. Duplication of data, leading to storage and RAM issues.
2. Each module needing this data would have to extract it before
processing each event, causing inefficiencies.

Instead, common data is supplied through a collection of *Services*.
Your module can access these via the @ref datatools::service_manager
reference supplied to its `initialize` method by the flreconstruct
application. From this, you can get and hold references to a current
service.

In this tutorial, we will see how to implement access and use of the
@ref geomtools::geometry_service. This will cover

1. Accessing and storing the service
2. Description of available services (DEFERRED)

As such, using the services with event data is deferred to individual
tutorials.


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
CMakeLists.txt  MyModule.h              MyModulePipeline.conf
MyModule.cpp    MyModulePipeline2.conf
$
~~~~~

The `CMakeLists.txt` build script and `.conf` pipeline scripts are
identical to the earlier tutorial. We begin by modifying the header file
to the following

\include flreconstruct/UseServicesModule/MyModule.h

All we have added is a forward declaration to the type of the service
we will will use, in this case @ref geomtools::manager, and a `const`
pointer to the service as a data member.

The main change comes in the implementation, where we extend the
`initialize` method to check for and obtain the service:

\include flreconstruct/UseServicesModule/MyModule.cpp

Note the main updates which are:

1. The constructor initializes the pointer to null.
2. The `reset` method nulls the pointer, which is fine because we don't own the service.
3. The `initialize` method queries the supplied @ref datatools::service_manager instance for the presence of a service named "geometry".
4. If the @ref datatools::service_manager supplied the service, it is
extracted from the manager as a reference-to @ref geomtools::geometry_service.
5. At present, @ref geomtools::geometry_service is just a thin holder of
an underlying @ref geomtools::manager instance, so a const pointer to this
is extracted from the service into the data member.

Though the service is not used directly, we call the `tree_dump` methods
of the @ref geomtools::geometry_service and @ref geomtools::manager instances to demonstrate that these hold something.

The `process` method is also updated to output the address of the
@ref geomtools::manager instance, indicating that it should be useful.

Don't worry about the two step process of getting a service and then
something within the service (the @ref geomtools::manager). Sometimes
the interface of a service will be used directly, in other cases
it may provide a convenience wrapper around additional but related tools.


Building and Running a Service-Aware Module
-------------------------------------------

No special steps are needed to build or run a service-aware module,
so you can follow the [build instructions from the original example](@ref minimalmodulebuilding) and [run the module in the same way](@ref minimalmodulerunning).


Available Services {#useservices_servicelist}
==================
TODO

