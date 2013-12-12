Working With Events in FLReconstruct
====================================

\tableofcontents

Introduction {#introduction}
============
The C++ type used to represent events in `flreconstruct` is the
@ref datatools::things class. Pipeline module classes inherit from
@ref dpp::base_module and thus must implement the
pure abstract method @ref dpp::base_module::process . This method is
called for each event, and is passed a reference to the
@ref datatools::things object representing the current event being
passed through the pipeline.

The `datatools::things` class implements an associative, hierarchical and
heterogenous collection of objects. To put it in simpler terms, it provides
a dictionary mapping string "keys" to object instances inheriting from
the the `datatools::i_serializable` pure abstract base class. It's the
dictionary like interface that provides the associativity, and the
storage of pointer-to-base-class that provides the heterogeneity (many
different concrete types). As `datatools::things` itself inherits from
`datatools::i_serializable`, it is capable of storing other
`datatools::things` instances, providing the possibility of arranging
objects in a tree-like structure.

In this tutorial, we'll look at three basic aspects of working with
`datatools::things` instances provided to the `process` method of your
custom pipeline module,

1. Reading data from the `datatools::things` instance.
2. Writing builtin objects to the instance
3. Implementing custom objects for storage in `datatools::things`



Reading Data from datatools::things Instances {#things_readingdata}
=============================================
To work with events in the pipeline we first need to implement a pipeline
module to do this. The basics of how to do this are covered in [a dedicated tutorial](@ref md_WritingFLReconstructModules) and you should familiarize
yourself with this material as this tutorial will build on it.

First of all we implement our module, build it and write a pipeline script
to use it in `flreconstruct`. Note that we have stripped all comments
except those relating to the process methods, and that the module takes
no configuration. If you require details on how to implement a basic
flreconstruct method, please refer to the [introductory tutorial](@ref md_WritingFLReconstructModules) first. We begin with the header:

\include flreconstruct/AccessThingsModule/AccessThingsModule.h

and now the implementation:

\include flreconstruct/AccessThingsModule/AccessThingsModule.cpp


Writing Data to datatools::things Instances {#things_writingdata}
===========================================


Implementing Custom Objects for Storage in datatools::things {#things_customdata}
============================================================
DEFERRED TO ALPHA3

