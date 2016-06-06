Using Mock Calibration for Reconstruction {#usingmockcalibration}
=========================================

\tableofcontents

Introduction {#mockcalo_intro}
============

Falaise currently only provides a mocking model for digitization
and calibration of data after the Geant4 part of the simulation has
completed. This is due to the developing nature of the physical electronic
system at the time of writing, and thus the digitization/calibration is
currently provided as an optional "preprocessing" step within
`flreconstruct`.

This allows a basic model to be used, and does not prevent further
validation and implementation of new digitization and calibration models.

Using the Mock Calibration Models {#mockcalo_using}
=================================

To use the mocking model as the first step of the pipeline, two modules
need to be run; `snemo::processing::mock_tracker_s2c_module` and
`snemo::processing::mock_calorimeter_s2c_module`. The following example
script shows how these may be run

\include flreconstruct/UsingMockCalibration/MockCalibratePipeline.conf

This simply chains together the two modules, with a simple dump module
as the last stage to demonstrate that the "CD" (**C** alibrated **D** ata)
data bank is added. It is this bank that should be used for any
reconstruction or analysis task, and it should be read as an instance
of `snemo::datamodel::calibrated_data`. The documentation of that object
should be consulted for information on accessing Geiger and Calorimeter
data.

You can replace the dump module in the above example with a chain of your
own reconstruction and analysis modules.

Future Development {#mockcalo_future}
==================

The mocking modules implement both digitization and calibration steps.
In future, the digitization step will be refactored into `flsimulate` as
the intent is for the simulation to output data in an identical
format to the physical experiment(s). The calibration step will be
refactored into a core module of `flreconstruct`.

Neither of these refactorings affect you if you wish to develop and
propose new digitization or calibration models. You should however take
care to not overwrite the "official" data banks for simulated and
calibrated data ("SD" and "CD" respectively). This is allow easy
comparison and validation of any new model with the "official" one, as
both "official" and "new" data banks will be available to downstream
modules.

Specific Topics for Development of Digitization {#mockcalo_digidevel}
-----------------------------------------------
You should implement custom digitization as
[a processing module in flreconstruct](@ref writingflreconstructmodules),
using the ["SD" data bank](@ref mctools::simulated_data) output
from `flsimulate` as the input. You should create
[any needed custom types](@ref writingflreconstructmodules)
to represent the digitized data and write these as a new bank in the
`datatools::things` instance representing the event.

TODO : Define bank name for digital data.

Specific Topics for Development of Calibration {#mockcalo_calodevel}
----------------------------------------------
You should implement custom calibration as
[a processing module in flreconstruct](@ref writingflreconstructmodules).
Your calibration model **must** output data in the form of an instance of
`snemo::datamodel::calibrated_data`. You would write this into the
`datatools::things` event instance as a new data bank named, say,
"CD_NewModel" to avoid clashes with the official "CD" bank. Downstream modules can access both banks for comparison and validation.
