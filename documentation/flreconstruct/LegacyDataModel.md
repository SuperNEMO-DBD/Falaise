Implementing Custom Objects for Storage in datatools::things {#legacy_things_customdata}
============================================================
**Please note that this section is now deprecated as Falaise is
migrating to a full [ROOT](https://root.cern.ch)-based serialization
system. During this migration phase, requests for new data types must
be made via [Issues on Falaise's GitHub](https:/github.com/supernemo-dbd/Falaise/issues).
The documentation below is retained only for reference.**

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
This is required for your data type to be integrated into the official
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

