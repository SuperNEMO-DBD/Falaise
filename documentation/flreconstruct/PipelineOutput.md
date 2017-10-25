FLReconstruct Pipeline Output {#flreconstructpipelineoutput}
=============================

\tableofcontents


Introduction {#flreconstructpipelineoutput_introduction}
============

The  current  official  reconstruction pipeline  consists  in  several
steps, each responsible of  the elaboration of additional informations
stored  in dedicated  banks  in  the event  record.   We  saw in
 [the tutorial   on FLSimulate output](@ref flsimulateoutput)  that   the
Monte-Carlo program  creates a new  data bank  in the event  record (a
`datatools::things` object) which is  keyed `SD` (**S** imulated **D** ata).

Once the  FLSimulate output file is  generated, it can be  used as the
input of the  FLReconstruct program. We describe here  the contents of
the *reconstruction data banks* added by the reconstruction pipeline.

~~~~~
    +----------------------------------+
    | Event record (datatools::things) |
    +----------------------------------+
          |
          +----------------+------+-------+-------+- - - - ?
          |                |      |       |       |
        +----+     :     +----+ +-----+ +-----+ +-----+
        | SD |     :     | CD | | TCD | | TTD | | PTD |
        +----+     :     +----+ +-----+ +-----+ +-----+
          /        :          \    \      /     /
         /         :           \    \    /     /
 +------+------+   :         +--+----+--+-----+-+
 | Monte-Carlo |   :         |  Reconstruction  |
 |   output    |   :         |     outputs      |
 +-------------+   :         +------------------+
~~~~~

The shared handle concept {#flreconstructpipelineoutput_thesharedhandleconcept}
=========================

Before to describe the SuperNEMO reconstruction banks, we explain the  *shared handle* concept,
a special kind of reference to an arbitrary object.
A *shared handle* is implemented through the `datatools::handle<T>` template class, where `T`
is an arbitrary class. It is based on the template `boost::shared_ptr<T>` *smart pointer* class
from the Boost library (note: this concept is also available in C++11, however we don't use this implementation in the SuperNEMO software yet).

The *shared handle* is a special kind of pointer which is responsible of the memory management
of the object it handles. More, it is able to share the handle to this object with as many as
other *shared handles* of the same type. As long as at least one *shared handle*
points to the object, the object is still managed in memory and thus accessible.
When, and only when, the last *shared handle* on a given object is destroyed,
the object itself is finally discarded from the memory.
This mechanism is implemented using some hidden reference counter.

Below we represent a shared handle [h] that originaly points
to an object which has been dynamically
allocated in memory (typically through the `new` C++ operator):
~~~~~
          +-------[1]
[h]------>| Object |
          +--------+
~~~~~
As it is the only shared handle refering to this object (see the [1] counter on the top right
corner of the box), when the [h] handle is destroyed (we use here the [~h]  symbol),
this automatically discards the object (using the ~Object to represent the deletion process):
~~~~~
          +------[1=>0]
[~h]~ ~ ~ | ~Object |            ======>   {nothing is left in memory}
          +---------+
~~~~~

Now suppose we have a second shared handle [h'] that now enters the game and points to the same
object than  [h] :
~~~~~
          +-------[2]
[h]------>| Object |<------[h']
          +--------+
~~~~~
If we destroy  [h], the object will be not destroyed because there is still one shared handle
that refers to it, namely [h'], which now becomes the responsible of the memory management
of the object:
~~~~~
          +-----[2=>1]                     +-------[1]
[~h]~ ~ ~ | Object |<------[h']  ======>   | Object |<------[h']
          +--------+                       +--------+
~~~~~


Many SuperNEMO data models make use of this technique to manipulate data structures of interest:
hits, clusters of hits, tracks... whatever data banks refer to these objects. This enables
the sharing of such objects from within various data banks. We don't have to care about who or what is responsible
of the memory management.

The illustration below illustrates
a typical case where a `CD` and a `TCD` banks refer to the same pool of Geiger hits (the [h?]
represent *shared handle* objects):

~~~~~
+----------------------------------+
| Event record (datatools::things) |
+----------------------------------+
  |
  +-------------------------+
  |                         |
+----+                    +-----+
| CD |                    | TCD |
+----+                    +-----+
  |                         |
  |                         +---------------[h0]-------------[h1]
  |                         |                |                |
  |                         |                v                v
  |                  +-------------+   +------------+   +------------+
  |                  | Unclustered |   | Cluster #0 |   | Cluster #1 |
  |                  +-------------+   +------------+   +------------+
  |                         |                |                |
  |      +----------[2]     |                |                |
 [ha]--->| GG Hit #0 |<----[hA]              |                |
  |      +-----------+                       |                |
  |      +----------[2]                      |                |
 [hb]--->| GG Hit #1 |<---------------------[hB]              |
  |      +-----------+                       |                |
  |      +----------[2]                      |                |
 [hc]--->| GG Hit #2 |<---------------------[hC]              |
  |      +-----------+                       |                |
  |      +----------[3]                      |                |
 [hd]--->| GG Hit #3 |<---------------------[hD]    /--------[hE]
  |      +-----------+<----------------------------/          |
  |      +----------[2]                                       |
 [he]--->| GG Hit #4 |<--------------------------------------[hF]
  |      +-----------+                                        |
  |      +----------[2]                                       |
 [hf]--->| GG Hit #5 |<--------------------------------------[hG]
         +-----------+
~~~~~
If we destroy the `CD` bank from the event record, this will destroy
the shared handles ha, hb, hc, hd, he and hf. However, as Geiger hits objects
from #0 to #5 are also refered by other shared handles from the `TCD` bank, they are not
deleted :
~~~~~
+----------------------------------+
| Event record (datatools::things) |
+----------------------------------+
                            |
                          +-----+
                          | TCD |
                          +-----+
                            |
                            +---------------[h0]-------------[h1]
                            |                |                |
                            |                v                v
                     +-------------+   +------------+   +------------+
                     | Unclustered |   | Cluster #0 |   | Cluster #1 |
                     +-------------+   +------------+   +------------+
                            |                |                |
         +----------[1]     |                |                |
         | GG Hit #0 |<----[hA]              |                |
         +-----------+                       |                |
         +----------[1]                      |                |
         | GG Hit #1 |<---------------------[hB]              |
         +-----------+                       |                |
         +----------[1]                      |                |
         | GG Hit #2 |<---------------------[hC]              |
         +-----------+                       |                |
         +----------[2]                      |                |
         | GG Hit #3 |<---------------------[hD]    /--------[hE]
         +-----------+<----------------------------/          |
         +----------[1]                                       |
         | GG Hit #4 |<--------------------------------------[hF]
         +-----------+                                        |
         +----------[1]                                       |
         | GG Hit #5 |<--------------------------------------[hG]
         +-----------+
~~~~~
If we now remove the cluster #0 from the `TCD` bank, the Geiger hits #1 and #2
are destroyed but the other ones are preserved, protected by their shared handles:
~~~~~
+----------------------------------+
| Event record (datatools::things) |
+----------------------------------+
                            |
                          +-----+
                          | TCD |
                          +-----+
                            |
                            +---------------[h1]
                            |                |
                            |                v
                     +-------------+   +------------+
                     | Unclustered |   | Cluster #1 |
                     +-------------+   +------------+
                            |                |
         +----------[1]     |                |
         | GG Hit #0 |<----[hA]              |
         +-----------+                       |
         +----------[1]                      |
         | GG Hit #3 |<---------------------[hE]
         +-----------+                       |
         +----------[1]                      |
         | GG Hit #4 |<---------------------[hF]
         +-----------+                       |
         +----------[1]                      |
         | GG Hit #5 |<---------------------[hG]
         +-----------+
~~~~~

That means that even if we completely remove some banks from the event record, as long as
some shared handles will point to a given object from other data banks,
the object will be preserved from deletion and will remain usable
at any processing stage of the pipeline. This technique is non only safe with respect to memory management,
it is also fully supported by the Boost/Serialization library which is the base of the Falaise I/O system.


Mock digitization and calibration {#flreconstructpipelineoutput_mockdigitizationandcalibration}
=================================

Both mock digitization and calibration modules `snemo::processing::mock_tracker_s2c_module` and
`snemo::processing::mock_calorimeter_s2c_module` create and fill the `CD` bank (**C** alibrated **D** ata). The `CD` bank contains a `snemo::datamodel::calibrated_data` object.

Each `snemo::datamodel::calibrated_data` object contains:
 - a collection of *shared handles* on calibrated calorimeter hits,
   each of them representing a hit in scintillator
   blocks (main walls, X-walls and gamma veto). A calibrated calorimeter hit is implemented as
   a `snemo::datamodel::calibrated_calorimeter_hit` object which contains:
    - the unique identifier of the hit (an integer, inherited from the `geomtools::base_hit` mother class)
    - the geometry identifier (GID) of the hit scintillator block (a `geomtools::geom_id` instance,
      inherited from the `geomtools::base_hit` mother class)
    - a collection of auxiliary properties (a `datatools::properties` instance,
      inherited from the `geomtools::base_hit` mother class)
    - the energy deposited in the calorimeter block (a real positive value in unit of energy)
    - the error on the energy deposited in the calorimeter block (a real positive value in unit of energy)
    - the time of the hit in the calorimeter block (a real value in unit of time)
    - the error on the time of the hit in the calorimeter block (a real positive value in unit of time)
 - a collection of shared handles on calibrated tracker hits (Geiger),
   each of them representing a hit in Geiger
   drift cells. A calibrated tracker hit is implemented as a
   `snemo::datamodel::calibrated_tracker_hit` object which contains:
    - the unique identifier of the hit (an integer, inherited from the `geomtools::base_hit` mother class)
    - the geometry identifier (GID) of the hit drift cell (a `geomtools::geom_id` instance,
      inherited from the `geomtools::base_hit` mother class)
    - a collection of auxiliary properties (a `datatools::properties` instance,
      inherited from the `geomtools::base_hit` mother class)
    - the transverse drift distance within the cell coordinates system (a real positive value in unit of length)
    - the error on the transverse drift distance within the cell coordinates system (a real positive value in unit of length)
    - the longitudinal position within the cell coordinates system (a real value in unit of length)
    - the error on the longitudinal position within the cell coordinates system (a real positive value in unit of length)
    - the (X, Y) position of the anode wire within the SuperNEMO module coordinates system (two real values in unit of length)
    - the delayed time of the hit (a real value in unit of time)
    - the error on the delayed time of the hit (a real positive value in unit of time)
 - a collection of auxiliary properties (a `datatools::properties` instance).

Tracker clustering {#flreconstructpipelineoutput_trackerclustering}
==================

Whatever tracker clustering module is used (`snemo::reconstruction::mock_tracker_clustering_module`,
`snemo::reconstruction::cat_tracker_clustering_module` or
`snemo::reconstruction::sultan_tracker_clustering_module`),
it create and fill the `TCD` bank (**T** racker **C** lustering **D** ata). The `TCD` bank contains
a `snemo::datamodel::tracker_clustering_data` object.

Each `snemo::datamodel::tracker_clustering_data` object contains:
 - a collection of shared handles on `snemo::datamodel::tracker_clustering_solution` objects
 - a shared handle on the default `snemo::datamodel::tracker_clustering_solution` object
   (ideally the best solution among the former collection, using some arbitrary criterion: P-value, chi2...
    depending on the way the clustering module ranks different possible solutions).
 - a collection of auxiliary properties (a `datatools::properties` instance).
It is expected that in the case of several clustering solutions are computed by the clustering algorithm,
we are able to determine which one should be used as the best one. For the current release, only one
clustering solution is computed so it is automatically selected as the default one.


Each `snemo::datamodel::tracker_clustering_solution` object contains:
 - the unique identifier of the solution (an integer)
 - a collection of shared handles on `snemo::datamodel::tracker_cluster` objects
 - a collection of shared handles on unclustered hits (referencing the calibrated tracker hits
   in the `CD` bank that are not associated to any cluster for whatever reason: isolated hits, not in time...)
 - a collection of auxiliary properties (`datatools::properties`).

Each `snemo::datamodel::tracker_cluster` object contains:
 - the unique identifier of the hit (an integer, inherited from the `geomtools::base_hit` mother class)
 - the geometry identifier (GID) of the tracker submodule where the cluster lies (a `geomtools::geom_id` instance,
   inherited from the `geomtools::base_hit` mother class)
 - a collection of auxiliary properties (a `datatools::properties` instance,
   inherited from the `geomtools::base_hit` mother class)
 - a collection of shared handles on `snemo::datamodel::calibrated_tracker_hit` objects
   (referencing the calibrated tracker hits in the `CD` bank)


Track fitting {#flreconstructpipelineoutput_trackfitting}
=============

A track fitting algorithm tries to fit a line or an helix to a given cluster provided from the  `TCD` bank.
It creates and fills the `TTD` bank (**T** racker **T** rajectory **D** ata). The `TTD` bank contains
a `snemo::datamodel::tracker_trajectory_data` object.

Each `snemo::datamodel::tracker_trajectory_data` object contains:
 - a collection of shared handles on `snemo::datamodel::tracker_trajectory_solution` objects
 - a shared handle on the default `snemo::datamodel::tracker_trajectory_solution` object
   (ideally the best solution among the former collection, using some arbitrary criterion depending
   on the way the fitting module ranks different possible solutions)
 - a collection of auxiliary properties (a `datatools::properties` instance).

Each `snemo::datamodel::tracker_trajectory_solution` object contains:
 - the unique identifier of the solution (an integer)
 - the shared handle to the clustering solution of which the clusters of hits have
   been fitted
 - a collection of shared handles on `snemo::datamodel::tracker_trajectory` objects associated
   to fitted clusters
 - a collection of shared handles on unfitted clusters
 - a collection of auxiliary properties (a `datatools::properties` instance).

Each `snemo::datamodel::tracker_trajectory` object contains:
 - the unique identifier of the trajectory (an integer, inherited from the `geomtools::base_hit` mother class)
 - the geometry identifier (GID) of the tracker submodule where the trajectory lies (a `geomtools::geom_id` instance,
   inherited from the `geomtools::base_hit` mother class)
 - a collection of auxiliary properties (a `datatools::properties` instance,
   inherited from the `geomtools::base_hit` mother class)
 - the shared handle to the cluster associated to the fitted trajectory
 - a collection of shared handles to orphan Geiger hits in the cluster that were not taken into account
   by the fit
 - a shared handle to the trajectory pattern (line or helix)

Two types of trajectory patterns are supported:
 - the `snemo::datamodel::line_trajectory_pattern` class for linear fitting,
 - the `snemo::datamodel::helix_trajectory_pattern` class for helical fitting.

Both classes inherit the `snemo::datamodel::base_trajectory_pattern` classes.

A `snemo::datamodel::line_trajectory_pattern` contains:
 - the pattern identifier, a label that identifies the type of trajectory pattern (value is `"line"`)
 - a `geomtools::line_3d` object representing the fitted linear trajectory in the reference frame
   of the tracking chamber. It contains:
    - the first and last points defining a linear segment.

A `snemo::datamodel::helix_trajectory_pattern` contains:
 - the pattern identifier, a label that identifies the type of trajectory pattern (value is `"helix"`)
 - a `geomtools::helix_3d` object representing the fitted helical trajectory in the reference frame
   of the tracking chamber. It contains:
    - the center of the helix,
    - the radius of the helix,
    - the pitch of the helix,
    - the first and last reduced angles of the helical segment.


Charged particle measurements {#flreconstructpipelineoutput_chargedparticlemeasurements}
=============================

A module dedicated to measurements of charged particles tries to compute some
useful informations associated to a given trajectory.
The `ChargedParticleTracking` plugin provides such a module: `snemo::reconstruction::charged_particle_tracking_module`.
This module is able to perform several measurements of interest on the trajectories that have been fitted
during the previous track fitting process:
 - determine the charge associated to a trajectory (exploiting the possible curvature of the track)
 - compute various vertexes of interest associated to the trajectory:
   - impact(s) on the source foil
   - impact(s) on the entrance surface of scintillator blocks in the  main calorimeter walls,
     the X-calo or the gamma veto

It used the `CD` and `TTD` banks as its input
and creates and fills the `PTD` bank (**P** article **T** rack **D** ata). The `PTD` bank contains
a `snemo::datamodel::particle_track_data` object.

TODO:
 - Describe the `snemo::datamodel::particle_track_data` object.
 - Describe the `snemo::datamodel::particle_track_solution` object.
 - Describe the `snemo::datamodel::particle_track` object.
 - Describe the `geomtools::blur_spot` object.

Topology measurements {#flreconstructpipelineoutput_topologymeasurements}
=====================

While all previous data banks determine and compute new information at the
level of the individual particles involved in an event, topology measurements are
performed at the event scale. The **T** opology **D** data bank contains
information relevant only on the full event scale, such as the Time-Of-Flight measurement
(computed only when at least two particles are involved) or the vertex of the event.

Each `snemo::datamodel::topology_data` object contains:
 - a collection of auxiliary properties (a `datatools::properties` instance)
 - a shared handle to the topology pattern.

Several types of topology patterns are supported:
 - the `snemo::datamodel::topology_1e_pattern` class is used for single electron event,
 - the `snemo::datamodel::topology_1e1a_pattern` class is used for 1 electron +
   1 alpha event,
 - the `snemo::datamodel::topology_1eNg_pattern` class is used for 1 electron +
   N gammas event,
 - the `snemo::datamodel::topology_2e_pattern` class is used for 2 electrons event,
 - the `snemo::datamodel::topology_2eNg_pattern` class is used for 2 electrons +
   N gammas event.

The classes inherit from the `snemo::datamodel::base_topology_pattern` class which
contains :
 - a dictionary of shared handles on `snemo::datamodel::particle_track` belonging
   to the event. Each particle track object is key-mapped with a unique
   name like `e1` and `e2` for two-electrons topology pattern.
 - a dictionary of shared handles on
   `snemo::datamodel::base_topology_measurement` holding the different
   topological measurements performed on the event given its pattern.

The `snemo::datamodel::base_topology_pattern` holds the topological data as well
as a link to the particle tracks whereas the inherited classes from
`snemo::datamodel::base_topology_pattern` only provide an *interface* and do not
hold the data.

Several topological measurements, all inherited from `snemo::datamodel::base_topology_measurement`, are provided :
- a `snemo::datamodel::tof_measurement`, which contains Time-Of-Flight-related
  measurements:
  - a list of internal probabilities,
  - a list of external probabilities.
- a `snemo::datamodel::energy_measurement`, which contains the total energy of the event
  as well as the energy associated to every individual particle,
- a `snemo::datamodel::angle_measurement`, which contains the angle between two
  given particles,
- a `snemo::datamodel::vertex_measurement` which contains the 3D position of the
  event vertex (determined by selecting the hypothesis with the best Χ² probability) and the errors on the extrapolated vertices.

These topological measurements are only performed when relevant. For instance,
TOF measurements do not make any sense for 1 electron topology patterns.
