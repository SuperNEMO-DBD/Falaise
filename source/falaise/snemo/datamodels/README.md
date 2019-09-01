# SuperNEMO (Event)DataModel Library

Call this (Event)DataModel as likely to be separate from (Raw)DataModel
library.

# Namespace
Currently `snemo::datamodel`, will become `snedm` or similar.

# Classes
Note: detailed inheritance not given yet, but can be assumed that all
data model classes inherit from `datatools::i_serializable` so that they
can be stored in `datatools::things` and persisted.

## Fundamental models
These model the lowest level data elements not composed from other datamodel
types.

- `calibrated_calorimeter_hit` (to become `CalorimeterHit` or similar)
  - semi-POD type, with attributes
    - Energy, Time, plus associated errors
  - Attributes by inheritance from `geomtools::base_hit`
    - Geometry ID
    - "Hit" ID
    - A set of properties (but we want to get rid of this!)
- `calibrated_tracker_hit` (to become `TrackerHit` or similar)
  - semi-POD type with attributes
    - Radius, Z, Time, plus associates errors, Cell position, calibration/raw traits
  - Attributes by inheritance from `geomtools::base_hit`
    - Geometry ID
    - "Hit" ID
    - A set of properties (but we want to get rid of this!)
- `base_trajectory_pattern`
  - pABC for classes modelling a "trajectory"
  - One attribute: `string` giving "id" of pattern
  - Interface allows retrieval of
    - pattern attribute
    - (pure virtual) returns `const i_shape1d&` (presumably the model for the trajectory)
- `helix_trajectory_pattern`
  - Concrete class of `base_trajectory_pattern`
  - Holds a `geomtools::helix_3d`
- `line_trajectory_pattern`
  - Concrete class of `base_trajectory_pattern`
  - Holds a `geomtools::line_3d`
- `polyline_trajectory_pattern`
  - Concrete class of `base_trajectory_pattern`
  - Holds a `geomtools::polyline_3d`
- `timestamp`
  - POD-type with attributes
    - seconds part of time
    - picoseconds part of time (picoseconds is the shortest time resolution of
      any electronic component in the DAQ)

## Composed Types
These model collections of, or associations between, the fundamental types.

- `event_record` (maybe change)
  - Type alias to `datatools::things`
- `event_header`
  - POD type other than inheritance from `i_xxx` interfaces, with attributes
    - `datatools::event_id`
    - `generation_type` (simulated/real etc)
    - `timestamp`
    - `datatools::properties` (semi-relevant, but ideally separate since it's independent metadata)
- `calibrated_data`
  - POD type other than inheritance from `i_xxx` interfaces, with attributes
    - `vector` of `datatools::handle`s to `calibrated_calorimeter_hit`
    - `vector` of `datatools::handle`s to `calibrated_tracker_hit`
- `tracker_cluster`
  - POD type (effectively a `vector`) with attributes
    - `vector` of `datatools::handle` to `calibrated_tracker_hit`
  - Attributes by inheritance from `geomtools::base_hit`
    - Geometry ID
    - "Hit" ID
    - A set of properties (but we want to get rid of this!)
- `tracker_trajectory`
  - `datatools::handle` to `tracker_cluster` (the "fitted" cluster)
  - `datatools::handle` to `base_trajectory_pattern` (the pattern "fitted" to the cluster)
  - Attributes by inheritance from `geomtools::base_hit`
    - Geometry ID
    - "Hit" ID
    - A set of properties (but we want to get rid of this!)
- `particle_track`
  - POD(?) type with attributes
    - enum representing electric charge
    - `datatools::handle` to `tracker_trajectory`
    - `vector` of `datatools::handle` to `geomtools::blur_spot` (models vertices)
    - `vector` of `datatools::handle` to `calibrated_calorimeter_hit`
  - Attributes by inheritance from `geomtools::base_hit`
    - Geometry ID
    - "Hit" ID
    - A set of properties (but we want to get rid of this!)
- `tracker_clustering_solution`
  - semi-POD type with attributes
    - `int32_t` as "solution id"
    - `vector` of `datatools::handle` to `tracker_cluster` (calculated clusters)
    - `vector` of `datatools::handle` to `calibrated_tracker_hit` (unclustered hits)
    - `datatools::properties` (but would like to get rid!)
    - `map` between `int32_t` and `datatools::handle` to `tracker_cluster`
      - appears used to associate hit to the cluster it's in
- `tracker_trajectory_solution`
  - Semi-POD type with attributes
    - `int32_t` as "solution id"
    - `datatools::handle` to `tracker_clustering_solution` (the "fitted" clustering solution)
    - `vector` of `datatools::handle` to `tracker_trajectory` ("fits" to the above cluster)
    - `vector` of `datatools::handle` to `tracker_cluster` ("unfitted" clusters)
- `tracker_clustering_data`
  - semi-POD type with attributes
    - `vector` of `datatools::handle` to `tracker_clustering_solution`
    - `datatools::handle` to `tracker_clustering_solution`
      - By definition, this is empty, or is == to a handle in the `vector`
- `tracker_trajectory_data`
  - semi-POD type with attributes
    - `vector` of `datatools::handle` to `tracker_trajectory_solution`
    - `datatools::handle` to `tracker_trajectory_solution`
      - By definition, this is empty, or is == to a handle in the `vector`
- `particle_track_data`
  - POD type with attributes
    - `vector` of `datatools::handle` to `particle_track`
    - `vector` of `datatools::handle` to `calibrated_calorimeter_hit` (hits not associated to a particle_track)

So can roughly see that

- `_data` classes are pure "holders" or very light associations (typically holding vector of `_solution`)
- `_solution` classes tend to hold
  - An instance of what they "solve" for (though usually as a `handle`)
  - A `handle` to the input data
  - A `handle` to left over data

Also note that use of `datatools::handle` makes ownership difficult to track.
Whilst `handle`s are designed to share ownership, this requires care in knowing
who and when can call things like `reset` to delete the held instance. `handle`
will be o.k. in that it throws an exception if a null handle is accessed, but still
need clarity to reduce this likelihood.

## POD vs Logic Classes
Which classes are true POD (i.e. `struct`s) vs those that implement
useful operations on the data they hold?

Can logic classes be split into POD and free functions?

# Serialization
Currently only Boost-based. Relevant files are:

- `boost_io/`
  - One `.ipp` file for each serialized type
  - This only needs to define the `serialize` member/free function for the type
- `the_serializable.cc`
  - Boost macros for all(?) serialized types
  - Purpose/need to be documented