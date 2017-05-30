Working With Events in FLReconstruct {#workingwitheventrecords}
====================================

\tableofcontents

Introduction to event record {#workingwitheventrecords_introduction}
====================================
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


Reading Data from datatools::things Instances {#workingwitheventrecords_things_readingdata}
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
works, as for event data these are likely to be blank.

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
As discussed above, the `datatools::things` object can store
instances of any type inheriting from `datatools::i_serializable`.
If the builtin types provided by Falaise and Bayeux do not meet
your needs, you can implement a new custom class derived from
`datatools::i_serializable`.

In this example, we will implement a simple custom class and add it
into the `datatools::things` event record. This class **must** inherit
from the pure abstract base class `datatools::i_serializable` and hence
**must**:

- Provide a `public` default constructor
- Provide a `public` virtual destructor
- Concretely implement the `datatools::i_serializable::get_serial_tag()`
pure virtual method

We therefore begin by writing the header file, which we'll name
`MyDataType.h`:

\include flreconstruct/AccessThingsModuleCustom/MyDataType.h

Note the inheritance from `datatools::i_serializable` and the use
of the `DATATOOLS_SERIALIZATION_DECLARATION` macro, which declares the
`get_serial_tag` method for us. We have also provided concrete methods
to implement this type as a simple increment-only counter.
Note also the use of Doxygen markup to document the file and methods.
This is required for your data type is to be integrated into the official
mainline pipeline.

With the header in place we can create the implementation file, which we'll
name `MyDataType.cpp`

\include flreconstruct/AccessThingsModuleCustom/MyDataType.cpp

Here we've implemented the trivial constructor/destructor and the counter
implementation, and added the `DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION`
macro. This, together with the use of `DATATOOLS_SERIALIZATION_DECLARATION`
in the header file provides the **minimal** boilerplate allowing the
class to be stored in `datatools::things`. Additional work is needed to
make the type fully serializable to/from a file, and this is described in a
[later section](@ref things_customdata_serialization).

To use this type in the pipeline, we update the implementation of the
`AccessThings` module as follows for the header:

\include flreconstruct/AccessThingsModuleCustom/AccessThingsModule.h

and the implementation:

\include flreconstruct/AccessThingsModuleCustom/AccessThingsModule.cpp

Note the inclusion of the `MyDataType` header. We use the `datatools::things::add` method to add a `MyDataType` bank to the event.
To compile the new type into a loadable module, we simply add the header
and implementation to the `add_library` call in the `CMakeLists.txt` script:

\include flreconstruct/AccessThingsModuleCustom/CMakeLists.txt

To see the effect of writing our own type, we compile the above code
into a shared library using the above CMake script and then use the following pipeline script to sandwich the module between two dump modules

\include flreconstruct/AccessThingsModuleCustom/AccessThingsPipeline.conf

You should see that the `PostProcess` stage results in output containing
the information written into the `ATMCounter` bank.


Serializing Custom Objects to Persistant Files/Archives {#things_customdata_serialization}
=======================================================

As it stands, our custom `MyDataType` data object is storable in a `datatools::things`
instance, but is *not* capable of being written to file, even though `datatools::things`
is. If you try and run the `AccessThingsModule` in a pipeline and output to a file,
e.g.

```console
$ flreconstruct -i test.brio -p AccessThingsPipeline.conf -o test-reco.brio
```

an exception will be thrown when trying to write the `datatools::things` instance to the file
(Mac OS X case shown):

```console
...
libc++abi.dylib: terminating with uncaught exception of type boost::archive::archive_exception: unregistered class - derived class not registered or exported
Abort trap: 6
$
```

This occurs because the underlying Boost serialization system does not know
how to persist the `MyDataType` class to file. To make `MyDataType`
persistable by the serialization system, we need to use a couple of macros and
one function implementation to

* Declare and define the serialization/persistant interfaces and implementation
* Register this code with the Boost serialization system so it can be loaded from
the plugin library.

The first addition is to the `MyDataType` header, where we add a call to the
[BOOST_CLASS_EXPORT_KEY](http://www.boost.org/doc/libs/1_60_0/libs/serialization/doc/traits.html#export)
macro post the class declaration. We defer detailed explanation of this macro to the [Boost Documentation](http://www.boost.org/doc/libs/1_60_0/libs/serialization/doc/traits.html#export)
suffice to say that this is needed to ensure templated serialization code
is instantiated and to register an identifier for the class in the serialization
system.

\include flreconstruct/AccessThingsModuleSerialization/MyDataType.h

The argument to the macro should be the class name, including a fully
qualified namespace if the class is placed inside one.

To isolate the serialization specific code from the main logic of the class,
the serialization implementation is written into a dedicated source file:

\include flreconstruct/AccessThingsModuleSerialization/MyDataTypeSerialization.cpp

As documented, this roughly splits into two sections:

1. Implementing the required read/write `MyDataType::serialize` member function, templated on
   the type of format (e.g. XML, BRIO) the class will be serialized to. The
   important points to note are that any base class of `MyDataType` must be serialized first,
   followed by the data members in order. Boost's serialization library provides many helper
   functions to create the key-value pairs in the archive, and [its documentation](http://www.boost.org/doc/libs/1_60_0/libs/serialization/doc/index.html)
   should be consulted for further details here.
2. Calling the implementation counterpart of `BOOST_CLASS_EXPORT_KEY`, the `BOOST_CLASS_EXPORT_IMPLEMENT` macro.
   This must be called with the same argument as `BOOST_CLASS_EXPORT_KEY`, and *must* come after inclusion
   of `Bayeux`'s `bayeux/datatools/archives_instantiation.h` header, with lists support file formats.
   This ordering ensures `MyDataType` can be serialized automatically to all supported file formats.

This source file is simply added to the inputs to the library:

\include flreconstruct/AccessThingsModuleSerialization/CMakeLists.txt

Once compiled, we can run as in previous examples, but this time we will not see an exception being thrown as
`MyDataType` is known to the serialization system and is written into the output file. However, if we try to
confirm this by using `flreconstruct`'s default dump-to-stdout, we see the exception again:

```console
$ flreconstruct -i test-reco.brio
libc++abi.dylib: terminating with uncaught exception of type boost::archive::archive_exception: unregistered class
Abort trap: 6
```

This might have been expected as the code for serializing `MyDataType` is held in the `AccessThingsModule` library
and this is not loaded by default in flreconstruct. When using custom data types, you must remember to add them to
the list of libraries to be loaded, for example via a script:

\include flreconstruct/AccessThingsModuleSerialization/AccessThingsDump.conf

If we now run with this script, we can see that our custom type was indeed serialized to the output file:

```console
$ flreconstruct -i test-reco.brio -p ../AccessThingsDump.conf
[notice:void datatools::library_loader::init():449] Automatic loading of library 'AccessThingsModule'...
|-- Bank 'ATMCounter' : "MyDataType"
`-- Bank 'SD' : "mctools::simulated_data"
...
```

This has only covered the basics of making your data objects serializable. More advanced topics are deferred to later tutorials.

**TODOS:**
* Clarify split of serialization code and the exact purpose/behaviour of the `BOOST_EXPORT_...` macros.
* Organization of code/files when we have more than one data class
* Organization of code/files if we want our data objects to used (and serialized) as data members of other objects.

