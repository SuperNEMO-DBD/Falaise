Official FLReconstruct Modules {#supportedflreconstructmodules}
==============================

\tableofcontents

As discussed in [Using the FLReconstruct Application](@ref usingflreconstruct),
the `flreconstruct` application takes an input file from `flsimulate`
(and later raw data), and passes each event through a sequence of modules that
perform reconstruction of the event and, optionally, then writes the
results out to file. Falaise supplies a stock set of modules to perform
common tasks and to construct the pipeline required for official
reconstruction of events from simulated/raw data. Here we document
these modules, describing their purpose and the data they work with
and create.

As explained in the [section on working with events](@ref workingwitheventrecords),
FLSimulate and FLReconstruct use the @ref datatools::things class as
the event data model for all experiments. This class provides a
dictionary-like interface allowing pipeline modules to read
and write data "banks" to/from the event using a string "key". Downstream
modules in the pipeline may use data written to the event by
upstream modules (e.g. a fitting module may need clustering data),
and thus need to know the keys under which any data banks it
requires are stored.


Core Falaise Data Banks
-----------------------

| Bank key | Bank type | Description | Creator |
| -------- | --------- | ----------- | ------- |
| EH | snemo::datamodel::event_header | Data about event | FLSimulate (not implemented yet) |
| SD | mctools::simulated_data | Simulated primary event, verted and truth hits | FLSimulate |
| CD | snemo::datamodel::calibrated_data | Calorimeter and tracker calibrated hits | Calibration module |
| TCD | snemo::datamodel::tracker_clustering_data | Clusters of tracker hits | Tracker clustering algorithm |
| TTD | snemo::datamodel::tracker_trajectory_data | Trajectories fitted to clusters | Track fitting algorithm |
| PTD | snemo::datamodel::particle_track_data | Charged particle tracks | Charged particle measurement algorithm |

The SD bank is created by the FLSimulate application.
The other banks are created by specific algorithms/modules embedded in the FLReconstruct pipeline.

Core Falaise Reconstruction Modules
-----------------------------------

| Module name | Library/Plugin | Required Input Bank | Output Bank | Description |
| ----------- | -------------- | ------------------- | ----------- | ----------- |
| snemo::processing::mock_tracker_s2c_module | Falaise | SD | CD | Digitizes and calibrates tracker simulated truth hits (mock) |
| snemo::processing::mock_calorimeter_s2c_module | Falaise | SD | CD | Digitization/calibration of the calorimeter simulated truth hits (mock) |
| snemo::reconstruction::mock_tracker_clustering_module | Falaise_MockTrackerClusterizer | CD | TCD | Basic clusterization of tracker hits (mock) |
| snemo::reconstruction::cat_tracker_clustering_module | Falaise_CAT | CD | TCD | Clusterization of tracker hits (CAT) |
| snemo::reconstruction::sultan_tracker_clustering_module | Falaise_CAT | CD | TCD | Clusterization of tracker hits (SULTAN) |
| snemo::reconstruction::trackfit_tracker_fitting_module | TrackFit, Falaise_TrackFit | TCD | TTD | Linear/helix fitting of tracks |
| snemo::reconstruction::charged_particle_tracking_module | Falaise_ChargedParticleTracking | CD, TTD | PTD | Measurements of charged particles' tracks |
| dpp::dump_module | Bayeux | SD, CD, TCD, TTD | none | Print the event record |
| snemo::visualization::visu_toy_module | Falaise_VisuToy | SD, CD, TCD, TTD | none | Basic event display |

Note that the mock digitization/calibration modules are a temporary feature.
Digitization functionality will be moved into `flsimulate` as the DAQ design
evolves. Calibration will be refactored into a dedicated task.

Configuration of reconstruction modules
---------------------------------------

As with other modules, the behaviour of the stock Falaise modules can
be configured using `datatools::properties` markup in text files. The
current defaults are provided in the following locations:

| Module name | Path to the default configuration file |
| ----------- | -------------------------------------- |
| snemo::processing::mock_tracker_s2c_module | \@falaise.resources:config/snemo/common/reconstruction/1.0/mock_tracker_s2c.conf |
| snemo::processing::mock_calorimeter_s2c_module | \@falaise.resources:config/snemo/demonstrator/reconstruction/1.0/mock_calorimeter_s2c.conf |
| snemo::reconstruction::mock_tracker_clustering_module | \@falaise.resources:modules/MockTrackerClusterizer/config/snemo/common/reconstruction/1.0/mock_tracker_clusterizer.conf |
| snemo::reconstruction::cat_tracker_clustering_module | \@falaise.resources:modules/CAT/config/snemo/common/reconstruction/1.0/CAT.conf |
| snemo::reconstruction::sultan_tracker_clustering_module | \@falaise.resources:modules/CAT/config/snemo/common/reconstruction/1.0/SULTAN.conf |
| snemo::reconstruction::trackfit_tracker_fitting_module | \@falaise.resources:modules/TrackFit/snemo/common/reconstruction/1.0/trackfit.conf |
| snemo::reconstruction::charged_particle_tracking_module | \@falaise.resources:modules/ChargedParticleTracking/snemo/common/reconstruction/1.0/charged_particle_tracking.conf |

