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
@ref datatools::things object representing the current event by the
pipeline.


Working with datatools::things
==============================
To work with events in the pipeline we first need to implement a pipeline
module to do this. The basics of how to do this are covered in [a dedicated tutorial](@ref md_WritingFLReconstructModules) and you should familiarize
yourself with this material as this tutorial will build on it.

First of all we implement our module, build it and write a pipeline script
to use it in `flreconstruct`. The basic files follow, and note that we have
stripped all extraneous comments except those relating to the process
method.

