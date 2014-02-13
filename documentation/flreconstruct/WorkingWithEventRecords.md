Working With Events in FLReconstruct {#workingwitheventrecords}
====================================

\tableofcontents

Introduction {#introduction}
============
The C++ type used to represent events in `flreconstruct` is the
@ref datatools::things class. Pipeline module classes inherit from
@ref dpp::base_module and thus must implement the
pure abstract method @ref dpp::base_module::process . This method is
called for each event, and is passed a reference to the mutable
@ref datatools::things object representing the current event being
passed through the pipeline.

The `datatools::things` class implements an associative, hierarchical and
heterogenous collection of objects. To put it in simpler terms, it provides
a dictionary mapping string "keys" to object instances inheriting from
the `datatools::i_serializable` pure abstract base class. It's the
dictionary like interface that provides the associativity, and the
storage of pointer-to-base-class that provides the heterogeneity (many
different concrete types). As `datatools::things` itself inherits from
`datatools::i_serializable`, it is capable of storing other
`datatools::things` instances, providing the possibility of arranging
objects in a tree-like structure.

In this tutorial, we'll look at three basic aspects of working with
`datatools::things` instances provided to the `process` method of your
custom pipeline module,

1. Reading data from the `datatools::things` instance
2. Writing builtin objects to the instance
3. Implementing custom objects for storage in `datatools::things`


Reading Data from datatools::things Instances {#things_readingdata}
=============================================
To work with events in the pipeline we first need to implement a pipeline
module to do this. The basics of how to do this are covered in [a dedicated tutorial](@ref writingflreconstructmodules) and you should familiarize
yourself with this material as this tutorial will build on it.

First of all we implement our module, build it and write a pipeline script
to use it in `flreconstruct`. Note that we have stripped all comments
except those relating to the process methods, and that the module takes
no configuration. If you require details on how to implement a basic
flreconstruct method, please refer to the [introductory tutorial](@ref writingflreconstructmodules) first. We begin with the header:

\include flreconstruct/AccessThingsModule/AccessThingsModule.h

and now the implementation:

\include flreconstruct/AccessThingsModule/AccessThingsModule.cpp

The key method to look at is `AccessThings::process` which as we've seen
before is passed a reference to the current event in the pipeline.

We begin working with the event by simply printing its name and description.
This is a trivial demonstration that the `datatools::things` interface
works, as for event date these are likely to be blank.

The second, more relevant task, is to extract the list of keys, and thus
data banks, stored in the event. Here, we use the `datatools::things::get_names`
method to fill a `std::vector` with the key names. We then iterate over this
vector to print out the key name and, by using the `datatools::things::get_entry_serial_tag` method, typename of the object it maps to.

~~~~~~{.cpp}
// Extract list of keys stored by the object
std::vector<std::string> workItemKeyList;
workItem.get_names(workItemKeyList);

// Iterate over keys, printing their name and the type of the object
// they map to
BOOST_FOREACH(std::string key, workItemKeyList) {
  std::cout << "- [key, serial_tag] : "
            << key
            << ", "
            << workItem.get_entry_serial_tag(key)
            << std::endl;
}
~~~~~~

If we know the type of the key we wish to extract, we can use the
`datatools::things::get` method to obtain a reference to it.

~~~~~~{.cpp}
try {
  const mctools::simulated_data& simData = workItem.get<mctools::simulated_data>("SD");
  simData.tree_dump();
} catch (std::logic_error& e) {
  std::cerr << "failed to grab SD bank : " << e.what() << std::endl;
  return dpp::base_module::PROCESS_ERROR;
}
~~~~~~

We know that the "SD" ( **S** imulated **D** ata) entry should map to an
instance of `mctools::simulated_data` so we use the `datatools::things::get`
method to obtain a const reference to it (const being read only). This
method takes a template argument which is the typename we want to extract,
and a function argument which is the name of the key to get. The method
will throw an exception if either

- The key does not exist.
- The key exists, but it does not map to the requested type

We therefore wrap the extraction in a try-catch block to handle both
of these potential errors. If we're able to get the reference to the object,
then we can use it directly. In this example, we simply use the
`mctools::simulated_data::tree_dump` method to dump some information on
the object to screen. You should consult the documentation of the classes
extracted to see what you can do with them. If an exception is thrown, then
we report the error to the standard error stream, and return the
`dpp::base_module::PROCESS_INVALID` flag. This will make the pipeline abort any further
processing of the event and subsequent events, but other flags are available
to handle a range of process errors.

To see the effect of this reading, we compile the above code into a shared
library just as before using the following CMake script:

\include flreconstruct/AccessThingsModule/CMakeLists.txt

and run `flreconstruct` with the following pipeline script:

To see the effect of this writing, we compile the above code into a shared
library just as before using the following CMake script:

\include flreconstruct/AccessThingsModule/AccessThingsPipeline.conf

You should see output similar to the dump modules we ran in earlier
tutorials.




Writing Data to datatools::things Instances {#things_writingdata}
===========================================
As the `datatools::things` instance is passed to pipeline modules by
non-const reference, it is directly modifiable by your module. This means
your module can store results of working with the event back into the event
for later modules to use (you can of course also delete existing data, so
be careful!).

Instances of `datatools::things` can only store objects that
inherit from the `datatools::i_serializable` abstract base class, so this
restricts the types your module can add. For now we will just look at how
to store an existing concrete class of `datatools::i_serializable` in
`datatools::things`, specifically, the `datatools::properties` class.
The use case of adding you own concrete classes of
`datatools::i_serializable` is deferred to a
[later tutorial in this guide](@ref things_customdata).

We begin by refactoring the `process` method of our module into read and
write parts, first the header

\include flreconstruct/AccessThingsModuleRW/AccessThingsModule.h

and then the implementation

\include flreconstruct/AccessThingsModuleRW/AccessThingsModule.cpp

This separation is done for clarity in this example, but it illustrates that
your `process` method need not be monolithic (and in fact shouldn't be
except for trivial cases). The `read` method is exactly as we implemented
earlier. In the `write` method, we use the `datatools::things::add` method
to add new data bank to the event holding a `datatools::properties`
instance. We pass
this method a template argument indicating the type of the data bank, and
a string function argument indicating the key under which to store the new
data bank. The method returns a reference to the newly created instance
so it can be modified in place, as we do by setting the description and
adding two properties.

To see the effect of this writing, we compile the above code into a shared
library just as before using the following CMake script:

\include flreconstruct/AccessThingsModuleRW/CMakeLists.txt

To see the effect of the writing new banks into the event, we use a pipeline
script to sandwich the module between two dump modules as follows

\include flreconstruct/AccessThingsModuleRW/AccessThingsPipeline.conf

You should see that the `PostProcess` stage results in output containing
the information written into the `ATMProperties` bank.

Implementing Custom Objects for Storage in datatools::things {#things_customdata}
============================================================
DEFERRED TO ALPHA3
