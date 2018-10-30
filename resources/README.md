Falaise Resource Files
======================

This directory contains the tree of configuration files for simualtion
and reconstruction. It is organized as follows:

genbb
-----

Definitions of Bayeux/GenBB event generators. These only describe
the particles/kinematics rather than vertices. Definitions are
organized into files based on an overall category such as Double Beta,
Backgrouns, Calibration and Generic.

It also provides generators than can be modified using the Bayeux
"variants" system.


geometry
--------

"Common" Bayeux/geomtools models. Only has PMT and tracker cells in
at the moment. See also snemo/demonstrator/geometry.


materials
---------

"Common" Bayeux/materials definitions for use in geometry/simulation.


snemo/demonstrator
------------------

Geometry, reconstruction and simulation for the SuperNEMO Demonstrator
detector.

- snemo/demonstrator/geometry
  - full Bayeux/geomtools model for the detector
- snemo/demonstrator/reconstruction
  - current flreconstruct pipelines for reconstruction
- snemo/demonstrator/vertexes
  - Bayeux/genvtx definitions for vertex generation in the detector
  - Effectively coupled to snemo/demonstrator/geometry
- snemo/demonstrator/geant4_control
  - Geant4 configuration for the detector, used in flsimulate
  - Uses geometry, vertexes, genbb and materials either directly
    or via other clients using them.

urn
---
Indirection mechanism for resource files. To be reviewed.


Note of Naming/Formatting
=========================

Files have mostly been renamed with `.conf` extension. There is no
official extension for Bayeux/datatools properties/multiproperties
files, which all of these are (barring a few `.csv` tables). The
`.conf` extension is clear on purpose, and most text editors will
recognise this and give a reasonable syntax highlighting scheme.

Properties/Multiproperties files only have 0/1 levels of formal
hierarchy for key/value pairs. However, developers *may* use a pseudo-hierarchy introduced
by key name, e.g.

```
TopLevel.type : string = "SubLevel"
SubLevel.X.name  : string = "foo"
SubLevel.X.value : int = 1
SubLevel.Y.name  : string = "boo"
SubLevel.Y.value : int = 2
```

which is equivalent to a hierarchy:

```
TopLevel: {
  SubLevel : {
    X : {
     name = "foo"
     value = 1
    }
    Y : {
      name = "bar"
      value = 2
    }
 }
```

We therefore *try* and account for this by grouping keys by name
and using indents to highlight hierarchy. For example, the above
would be formatted as:

```
TopLevel.type : string = "SubLevel"
  SubLevel.X.name  : string = "foo"
  SubLevel.X.value : int = 1
  SubLevel.Y.name  : string = "boo"
  SubLevel.Y.value : int = 2
```

with further indents as required.

Properties/Multiproperties parsers also have a very basic preprocessor
known as "variants". These introduce conditional blocks which should
be formatted following C/C++ conditionals to indicate structure and
depth, e.g.

```
TopLevel.type : string = "SubLevel"
  SubLevel.X.name  : string = "foo"
  #@variant_if (somecondition|true)
    SubLevel.X.value : int = 1
  #@variant_endif
```

with further indent as required.

These guidelines are still in development.
