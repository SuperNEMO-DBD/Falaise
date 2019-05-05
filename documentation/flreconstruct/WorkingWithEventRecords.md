Working With Events in FLReconstruct {#workingwitheventrecords}
====================================

\tableofcontents

Introduction to Event Data {#workingwitheventrecords_introduction}
==========================
Events in `flreconstruct` are represented by instances of the @ref datatools::things
class. The `process` member function of pipeline modules is passed
a non-const is called for each event, and is passed a @ref datatools::things
reference to the current event.

The @ref datatools::things class is nothing more than a key-value dictionary
with `std::string` keys pointing to objects representing different parts
of the event data. Each data model object is a class inheriting from @ref
datatools::i_serializable, allowing many different concrete data models
to be stored.  As `datatools::things` itself inherits from
`datatools::i_serializable`, it is capable of storing other
`datatools::things` instances, allowing a tree-like structure of data
if required.

In this tutorial, we'll look at three basic aspects of working with
@ref datatools::things instances provided to the `process` method of your
custom pipeline module,

1. Reading data from the @ref datatools::things instance
2. Writing builtin objects to the instance
3. (DEPRECATED) Implementing custom objects for storage in @ref datatools::things


Reading Event Data {#workingwitheventrecords_things_readingdata}
==================
To work with events in the pipeline we first need to implement a pipeline
module to do this. The basics of how to do this are covered in [a dedicated tutorial](@ref writingflreconstructmodules) and you should familiarize
yourself with this material as this tutorial will build on it.

First of all we implement our module, build it and write a pipeline script
to use it in `flreconstruct`. Note that we have stripped all comments
except those relating to the process methods, and that the module takes
no configuration. If you require details on how to implement a basic
flreconstruct method, please refer to the [introductory tutorial](@ref writingflreconstructmodules) first.

\include flreconstruct/AccessThingsModule/AccessThingsModule.cpp

We begin working with the event by simply printing its name and description.
This is a trivial demonstration that the @ref datatools::things interface
works, as for event data these are likely to be blank.

The second, more relevant task, is to extract the list of keys, and thus
data banks, stored in the event. Here, we use the `datatools::things::get_names`
method to fill a `std::vector` with the key names. We then iterate over this
vector to print out the key name and, by using the `datatools::things::get_entry_serial_tag` method, typename of the object it maps to.

~~~~~~{.cpp}
// Extract list of keys stored by the object
std::vector<std::string> keys;
event.get_names(keys);

// Print each key and its concrete type
for (const auto& k : keys) {
  std::cout << "- [key, serial_tag] : "
      << k
      << ", "
      << event.get_entry_serial_tag(k)
      << std::endl;
}
~~~~~~

If we know the type of the key we wish to extract, we can use the
@ref datatools::things::get member function to obtain a const reference to it.

~~~~~~{.cpp}
auto& simData = event.get<mctools::simulated_data>("SD");
simData.tree_dump();
~~~~~~

This member function takes a template argument which is the typename we want to extract,
and a function argument which is the name of the key to get.
In this case, we want to extract the "SD" ( **S** imulated **D** ata) bank
and that this should be of type @ref mctools::simulated_data.
Use of `get` will throw an exception if either

- The key does not exist.
- The key exists, but it does not map to the requested type

with `flreconstruct` handling the exception and reporting to standard error.

Once extracted, the data can be used as required by your module.
In this example, we simply use the
@ref mctools::simulated_data::tree_dump member function to dump some information on
the object to screen.

Finally, we return @ref falaise::processing::status::PROCESS_OK to indicate that
we've processed the event without error.

To see the reading of the data in action, we compile the above code into a shared
library just as before using the following CMake script:

\include flreconstruct/AccessThingsModule/CMakeLists.txt

and run `flreconstruct` with the following pipeline script:

\include flreconstruct/AccessThingsModule/AccessThingsPipeline.conf

You should see output similar to the dump modules we ran in earlier
tutorials.


Writing Data to the Event {#things_writingdata}
=========================
As the @ref datatools::things instance is passed to pipeline modules by
non-const reference, data can be both read from, and written to, it. This means
your module can store results of working with the event back into the event
for later modules to use (you can of course also delete existing data, so
be careful!).

As @ref datatools::things can only store objects that
inherit from the @ref datatools::i_serializable abstract base class,
the types of data your module can add are restricted. For now we will just look at how
to store an existing concrete class of @ref datatools::i_serializable in
@ref datatools::things, specifically, the @ref datatools::properties class.
The use case of adding you own concrete classes of
@ref datatools::i_serializable is deprecated, but a
[later tutorial in this guide covers the basics](@ref things_customdata).

We begin by refactoring the `process` method of our module into read and
write parts:

\include flreconstruct/AccessThingsModuleRW/AccessThingsModule.cpp

This separation is done for clarity in this example, but it illustrates that
your `process` method need not be monolithic (and in fact shouldn't be
except for trivial cases). The `read` member function just prints the held keys as
we did before. In the `write` member function, we use @ref datatools::things::add
to add new data bank to the event holding a @ref datatools::properties
instance. We pass this a template argument indicating the type of the data bank, and
a string function argument indicating the key under which to store the new
data bank. A reference to the newly created instance is returned
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
**Please note that this section is now deprecated as Falaise is
migrating to a full [ROOT](https://root.cern.ch)-based serialization
system. During this migration phase, requests for new data types must
be made via [Issues on Falaise's GitHub](https:/github.com/supernemo-dbd/Falaise/issues).
The legacy documentation may be consulted online.**

