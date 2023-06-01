Writing FLReconstruct Filters {#writingflreconstructfilters}
=============================

\tableofcontents

Introduction to the writing of FLReconstruct filters {#writingflreconstructfilters_introduction}
========================================================================

If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref usingflreconstruct).

FLReconstruct uses a [pipeline pattern](http://en.wikipedia.org/wiki/Pipeline_%28software%29) to process events. You can view this as a production line with each stage on the line
performing some operation on the event. In a previous tutorial, we saw
how to [implement modules that read/write data from/to the event](@ref writingflreconstructmodules).
These "processing" modules are not the only operation that can be performed
on events, and in this tutorial we will cover _filters_, a type of module that can accept or reject
events for further processing based on event data, covering

1. Writing a basic C++ filter module
2. Scripting use of the filter by FLReconstruct

Like processing modules, _filters_ are compiled into shared libraries and
loaded by `flreconstruct` at runtime, and may be made user configurable from
pipeline scripts. These topics are already covered in detail in the
[processing module tutorial](@ref writingflreconstructmodules) and are identical for _filters_, so please refer back to that for full details.

Using event data to accept or reject the event in a filter is deferred to [a later tutorial](@ref workingwitheventrecords).

Implementing a Minimal flreconstruct Filter {#writingflreconstructfilters_minimalfilter}
===========================================
Creating the Filter Source Code {#writingflreconstructfilters_minimalfilter_sources}
-------------------------------
We begin by creating an empty directory to hold the source code for
our example filter, which we'll name "MyFilter"

```console
$ cd MyWorkSpace
$ mkdir MyFilter
$ cd MyFilter
$ ls
$
```

You are free to organise the source code under this directory as you see
fit. In this very simple case we will just place all files in the `MyFilter`
directory without any subdirectories. We start by creating the implementation file,
for the C++ class, which we'll name `MyFilter.cpp`

\include flreconstruct/MyFilter/MyFilter.cpp

Here we can see the interface and infrastructure required by a filter class for `flreconstruct`.
The class must implement:

- A default constructor
- A non-default constructor taking parameters:
  - `falaise::property_set const&`, the configuration supplied to the module by the pipeline script
  - `datatools::service_manager&`, `flreconstruct`'s service provider
- A public member function `filter` taking a single `const datatools::things&` input
  parameter, and returning a boolean indicatating acceptance/rejection (`true`/`false`) of the event.

To make the plugin we'll build from this code loadable by `flreconstruct` we must also use the
@ref FALAISE_REGISTER_FILTER macro, passing it the class's typename. This will
also become a string that can be used to create a filter of this type in
an `flreconstruct` pipeline script.

The non-default constructor is responsible for initializing the filter using,
if required, the information supplied in the @ref falaise::property_set
and @ref datatools::service_manager objects. We don't use this in `MyFilter`
to keep things simple, but filters can be configured just as processing
modules are as shown in the [tutorial on implementing and using configurable
parameters](@ref minimalconfigurablemodule}).

The `filter` member function performs the actual operation on the event, which is
represented by a @ref datatools::things instance. It is passed via const reference
because a filter only ever needs to read data from the event. As noted above, [a later tutorial](@ref workingwitheventrecords)
will cover the interface and use of @ref datatools::things to read data from
the event. Instead of using the event data, we simply keep a count of
the number of events processed and reject any that have an odd numbered count.

Building and Running a Loadable Shared Library for a Filter
------------------------------------------------------------
Like processing modules, filter modules are compiled into shared libraries that
`flreconstruct` loads at runtime via configuration in the pipeline script.
Filters are built and run identically to processing modules, so please see
the [earlier tutorial for full details here](@ref minimalmodulebuilding).
The implementation of the CMake script is identical:

\include flreconstruct/MyFilter/CMakeLists.txt

and the filter plugin library is also built in the same fashion:

```
$ mkdir MyFilter-build
$ cd MyFilter-build
$ cmake -DCMAKE_PREFIX_PATH=/where/Falaise/is ..
$ make
$ ls
... libMyFilter.so
```

The resulting plugin may be used in a `flreconstruct` pipeline script as follows:

\include flreconstruct/MyFilter/MyFilterPipeline.conf

All filters provide and require two `string` parameters:

- `on_accept`: name of the module events **accepted** by the filter should be passed to
- `on_reject`: name of the module events **rejected** by the filter should be passed to

In the above example, we have set both `on_accept` and `on_reject` to be
simple `dpp::dump_module`s to print events to screen. The `indent` parameter is different for each module so you can see in the printout
which path the event flowed down, and you should see these alternate for
each event as the counter goes from odd to even and back again. If you
write the pipeline results to and output `.brio` file, you will still
have to all events written to file.

To implement removal of events from the pipeline and hence output file,
a special builtin module @ref falaise::processing::black_hole_module is
provided. This can be placed on one branch of a filter where it will simply
stop any further processing of events that flow into it, including their
output to file. Running the following script:

\include flreconstruct/MyFilter/MyFilterPipeline2.conf

and outputing the results to a `.brio` file will only store events in the
file that are accepted by the filter. @ref falaise::processing::black_hole_module
is intended for typical "cut flow" work where events rejected by a filter are no longer of any interest. However, the `on_reject` module can be any other module or filter, allowing
you to build up detailed event processing workflows.


Next Steps
==========
The above examples have illustrated the basic structures needed to
implement a filter module and load it into `flreconstruct`.

Practical filters will access the event object passed to them and
read data from it to make the accept/reject decision.
[Using the event data model in modules is covered in a dedicated tutorial](@ref workingwitheventrecords).

