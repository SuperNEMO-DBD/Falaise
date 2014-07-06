FLSimulate Output {#flsimulateoutput}
=================

\tableofcontents

Introduction {#flsimulateoutput_introduction}
============

The current official simulation application defines
an output data model to represent the detector response
(SuperNEMO demonstrator module, BiPo3...).
Each *simulated* event record is implemented through the generic
`datatools::things` container object that handle a dedicated data
bank which is conventionaly named `SD` (**S** imulated **D** ata).

In the future, the simulation application will also support
the creation of the *digitization data* `DD` bank
(**D** igitization **D** ata).

Here we describe the contents of the data model produced by the SuperNEMO
Monte-Carlo program.

~~~~~
+----------------------------------+
| Event record (datatools::things) |
+----------------------------------+
          |
        +----+
        | SD |----- {this is a bank}
        +----+
         /
        /
+------+------+
| Monte-Carlo |
|   output    |
+-------------+
~~~~~

The geometry identifier concept {#flsimulateoutput_thegeometryidentifierconcept}
===============================

A very important concept, massively used within the Falaise simulation
and data processing, is the *geometry  identifier* (GID).  A GID is an
object that uniquely identifies a  geometry volume in the experimental
setup.  Given some  dedicated rules,  the description  of the  virtual
geometry of the experiment is able  to associate such an unique GID to
each volumes  of interest  in the geometry  (this technique  is called
*geometry mapping*  and is under  the responsibility of  the *geometry
manager* object and its embedded *GID manager* object).

What is important to  know about any GID object is  that it stores two
kinds of information:

 - the *geometry type*  is an unique positive  integer identifier that
   indicates which type of geometry volume  is addressed by the GID. A
   *geometry  type*   is  conventionaly  associated  to   a  *geometry
   category* which  is an  unique human readable  string (the  list of
   *geometry types  and categories* supported by  a given experimental
   setup is defined in a special configuration file).

   The  table  below presents  some  official  geometry categories  of
   interest in the SuperNEMO demonstrator geometry:

| Category | Type | Address contents | Description | Example  |
| -------- | ---- | ---------------- | ----------- | -------- |
| `"world"` | `0` | *world* number | Conventionaly identifies the world volume | [0:0] represents the top-level geometry volume |
| `"module"` | `1000` | *module* number | Identifies any detector module | [1000:0] represents the demonstrator module |
| `"source_strip"` | `1102` | *module* and *strip* numbers | Identifies any source foil strip | [1102:0.3] represents the fourth strip in the demonstrator module |
| `"tracker_submodule"` | `1200` | *module* and *side* numbers  | Identifies any tracker submodule | [1200:0.1] represents the French side tracker in the demonstrator module |
| `"tracker_layer"` | `1202` | *module*, *side* and *layer* numbers | Identifies any tracker layer of drift cells | [1202:0.1.8] represents the layer near the wall in the French side tracker |
| `"drift_cell_core"` | `1204` | *module*, *side*, *layer* and *row* numbers | Identifies any tracker drift cell | [1204:0.1.8.112] represents the cell at the French/Tunnel corner of the tracking chamber |
| `"xcalo_block"` | `1232` | *module*, *side*, *wall*, *column* and *row* numbers | Identifies any X-calo block | [1232:0.1.1.0.15] represents the block at the French/Tunnel top corner of the detector |
| `"gveto_block"` | `1252` | *module*, *side*, *wall* and *column* numbers | Identifies any gamma veto block | [1252:0.1.1.15] represents the block at the French/Tunnel top corner of the detector |
| `"calorimeter_block"` | `1302` | *module*, *side*, *column*, *row* and *part* numbers | Identifies any main calorimeter block | [1302:0.1.19.12.1] represents the front part of the scintillator block at the French/Tunnel top corner of the detector |

 - the  *address* of  the GID  is  an ordered  collection of  positive
   integer identifiers that informs about the effective positioning of
   the  volume in  the geometry  hierarchy :  module (only  0 for  the
   demonstrator),  side number  (italian/french), source  strip number
   (from 0 to 35), main clorimeter  column (from 0 to 19), Geiger cell
   layer (from 0 to 8)...

It is thus possible to extract these informations from any GID object.

Example with GID with value [1200:0.1] :
 - Q: What is your type (or category) ?

   A: The GID will respond `1200`  (this is the type associated to the `"tracker_submodule"` geometry category)
 - Q: What is the depth of your address ?

   A: The GID will respond `2` because the full path of
   address is made of two numbers, namely the *module* number (at rank `0`) and the *tracker submodule* number (at rank `1`).
 - Q: What is your address of rank `0` ?

   A: The GID will respond `0` (this is the *module* number, here the demonstrator module is conventionaly numbered `0`).
 - Q: What is your address of rank `1` ?

   A: The GID will respond `1` (this is the French *tracker submodule* number in the geometry convention of the experiment).

The  GID object  is  thus the  unique repository  where  to store  the
geometrical   identification  and   localization  of   a  volume   and
particularly for Monte-Carlo truth hits.


The  figure  below  illustrates  the  identification  of  the  volumes
traverses by  simulated electron tracks in  the SuperNEMO demonstrator
module:

![Two simulated electron tracks traversing several volumes of the detector associated to various geometry categories.](@ref images/fls_demonstrator_geom_ids_0.jpg)

From this example, we can determine the geometry identifiers (GIDs) associated to these volumes, thanks
to the *mapping* and *locator* mechanisms implemented in the geometry modelling manager:
 - The two electrons are emitted from a source strip (green segment on the right). This source strip
   volume belongs to the `"source_strip"` geometry category (type 1102). It is the last one
   placed in the source frame. Its geometry identifier is [1102:0.35] with module number 0 and source strip number 35.
 - The first truth track (bottom) traverses the drift volumes of 11 Geiger cells (blue squares)
   of which the geometry category is `"drift_cell_core"` (type 1204). All the hit cells
   are in the Italian side of the demonstrator module. The corresponding
   GIDS are:
    - [1204:0.0.0.110] is associated to the cell close to the vertex represented by a small square in the source foil; the GID address
      consists in module number O, side number 0 (Italian side), layer number 0 (near the source frame)
      and row number 110 (one of the last 9-cells rows on the Tunnel side),
    - [1204:0.0.1.110],
    - [1204:0.0.2.110],
    - [1204:0.0.3.110],
    - [1204:0.0.3.111],
    - [1204:0.0.4.111],
    - [1204:0.0.5.111],
    - [1204:0.0.6.111],
    - [1204:0.0.7.111],
    - [1204:0.0.8.111],
    - [1204:0.0.8.112] is associated to the cell at the top left corner of the Italian tracker submodule (in the "C1" C-shape).

   Then the first track terminates in the front part of a scintillator block (category `"calorimeter_block"`) of the
   Italian main wall (left purple rectangle). Its GID is [1302:0.0.19.11.1] which means:
    - module number 0 (demonstrator module),
    - side number 0 (Italian),
    - column number 19 (the last column in the calorimeter main wall on the Tunnel side on the Y axis),
    - row number 11 (not visible here on this X-Y projection),
    - part number 1 (because these scintillator blocks are modeled in two parts, the back
      part which is an extruded
      scintillator block on the photomultiplier side (part number is 0)
      and the front block which is a simple scintillator block on the tracker
      side (part number is 1)).
 - The second truth track (top) traverses the drift volumes of 2 Geiger cells (blue squares) of which the GIDs are:
    - [1204:0.0.0.111] nearest the vertex,
    - [1204:0.0.0.112] nearest the X-calo scintillator block.

   The track terminates in a scintillator block (category `"xcalo_block"`)
   at the Tunnel side of the Italian main wall (top purple rectangle).
   Its GID is [1232:0.0.1.0.8] which means:
    - module number 0 (demonstrator module),
    - side number 0 (Italian),
    - wall number 1 (Tunnel side on the Y axis),
    - column number 0 (the first column near the source frame on the X axis),
    - row number 8 (not visible here on this X-Y projection).

The mctools::base_step_hit class {#flsimulateoutput_mctoolsbasestephitclass}
================================

The Bayeux/mctools Geant4 plugin used by the FLSimulate application is designed to generate
collections of *truth step hits* through the Geant4 simulation engine. The output data model
contains objects that represents such truth hits. It uses the `mctools::base_step_hit` class.
The  `mctools::base_step_hit` class inherits the `geomtools::base_hit` class.

Each object of the `mctools::base_step_hit` class contains:
 - the unique identifier of the truth hit, with respect to the collection it belongs to
   (an integer, inherited from the `geomtools::base_hit` mother class),
 - the geometry identifier (GID) of the volume where the hit was generated, if available (a `geomtools::geom_id` instance,
   inherited from the `geomtools::base_hit` mother class),
 - a collection of optional auxiliary properties (a `datatools::properties` instance,
   inherited from the `geomtools::base_hit` mother class),
 - the start position of the step hit (a `geomtools::vector_3d` instance),
 - the stop position of the step hit (a `geomtools::vector_3d` instance),
 - the start time of the step hit (a real value in unit of time),
 - the stop time of the step hit (a real value in unit of time),
 - the start momentum of the step hit (a real value in unit of energy),
 - the stop momentum of the step hit (a real value in unit of energy),
 - the energy deposit along the track segment (a real value in unit of energy),
 - the particle name (a character string provided by Geant4).

The recording of these basic attributes is optional. Users are free to parametrized only a subset
of truth step's attributes to be stored in the output data model. This is configured while combining
the sensitive detector associated to some volumes and the step hit processor attached to the
sensitive detector. Note also that the meaning and the need of these attributes
may change depending of the nature of the sensitive detector.

Note that the collection of auxiliary properties can be used to store, on user request,
additional informations like:
 - the identifier of the corresponding G4 truth hit (a positive integer value provided by Geant4),
 - the name of step processor that produced the truth hit (a character string),
 - the identifier of the G4 track (a positive integer value provided by Geant4),
 - the identifier of the parent G4 track (a positive integer value provided by Geant4),
 - the kinetic energy of the particle at the beginning of the step  (a real value in unit of energy),
 - the kinetic energy of the particle at the end of the step  (a real value in unit of energy),
 - the *primary particle* flag (a boolean value set to `1` for a primary particle, equivalent to parent track identifier set to `0`),
 - the name of the G4 creator process (a character string provided by Geant4),
 - the name of the G4 creator category (a character string provided by Geant4),
 - the *delta-ray from alpha* flag (a boolean value set to `1` for a low-energy secondary electron generated along the track of an
   an alpha particle, typically used to compute the quenching factor in some material),
 - the *entering volume* flag (a boolean value set to `1` for a particle entering a volume by Geant4),
 - the *leaving volume* flag (a boolean value set to `1` for a particle leaving a volume by Geant4),
 - the name of the G4 volume (a character string provided by Geant4),
 - the copy number of the G4 volume (a positive integer value provided by Geant4).

Given a collection of such truth hits, it is expected that users will be able to apply some
arbitrary digitization algorithm.

The mctools::simulated_data class {#flsimulateoutput_mctoolssimulateddata}
=================================

The `SD` bank created by the simulation within each event record contains
a `mctools::simulated_data` object.

Each `mctools::simulated_data` object contains:
 - the primary vertex (a `geomtools::vector_3d` instance),
 - the primary generated event (particle types and kinematics, a `genbb::primary_event` instance),
 - a collection of optional auxiliary properties (a `datatools::properties` instance),
 - a dictionary of collections of shared handles to `mctools::base_step_hit` objects. This dictionary
   owns several collection of truth hits (eventually managed through *shared handles*, see [here](@ref flreconstructpipelineoutput_thesharedhandleconcept)), depending on the parametrization  of the simulation output profiles.
   For the SuperNEMO demonstrator, we use the following collections of truth hits:
   - `"calo"` : calorimeter truth hits collected from the scintillator blocks in the main walls.

     These hits are built from G4 raw truth hits by the `calorimeter.hit_processor` step hit backend processor.
   - `"xcalo"` : calorimeter truth hits collected from the scintillator blocks in the X-walls.

     These hits are built from G4 raw truth hits by the `xcalorimeter.hit_processor` step hit backend processor.
   - "`gveto"` : calorimeter truth hits collected from the gamma veto scintillator blocks

     These hits are built from G4 raw truth hits by the `gveto.hit_processor` step hit backend processor.
   - `"gg"` : Geiger hits collected from the tracker cell drift region (considering the Geiger regime).

     These hits are built from G4 raw truth hits by the `gg.hit_processor` step hit backend processor.
   - `"__visu.track"` : raw truth step hits collected from various part of the detector including not onmy the sensitive
     detectors (scintillator blocks, Geiger cells) but also some non-sensitive volumes of the geometry (source foil strips...).

     These hits are built from G4 raw truth hits by many step hit backend
     processors of the `mctools::push_all_step_hit_processor`  class. They are only
     generated for specific  output profiles
     and not produced by default.

Each `genbb::primary_event` object contains:
 - the time of the decay (a real value in unit of time),
 - the list of primary particles (`genbb::primary_particle` instances),
 - the name of the event generator (a character string),
 - a collection of auxiliary properties (a `datatools::properties` instance).

Each `genbb::primary_particle` object contains:
 - the type of the particle (an integer value, based on an extended Geant3 nomenclature),
 - the optional PDG particle code (an integer value),
 - the optional label describing the particle (a character string),
 - the optional mass of the particle (a real positive value),
 - the time of emission of the particle (a real value in unit of time),
 - the original momentum of the emitted particle (a `geomtools::vector_3d` instance),
 - the optional vertex from where the particle was emitted (a `geomtools::vector_3d` instance),
 - a collection of auxiliary properties (a `datatools::properties` instance).


The truth calorimeter hits {#flsimulateoutput_truthcalorimeterhits}
==========================

The `"calo"` , `"xcalo"` and `"gveto"` collections of truth hits are generated by their respective backend step
processors of the type `snemo::simulation::calorimeter_step_hit_processor`.

The principle of this algorithm is to collect all raw step hits
generated by Geant4 in a given scintillator block (sensitive volume)
and to build an unique resulting truth hit that sums up the total energy deposit
along all these raw steps. The final *calorimeter hit* records the following attributes
in the output `mctools::base_step_hit` object (see [this section](@ref flsimulateoutput_mctoolsbasestephitclass)):
 - the unique identifier of the hit in the collection,
 - the GID locating the scintillator block where the raw hits were produced (type is `1302` for main wall blocks, `1232` for X-calorimeter blocks  and `1252` for gamma veto blocks),
 - a collection of optional auxiliary properties (a `datatools::properties` instance).
 - the first position of the corner of the bounding box that
   wraps all the raw hits (a `geomtools::vector_3d` instance),
 - the second position of the opposite corner of the bounding
   box that wraps all the raw hits (a `geomtools::vector_3d` instance),
 - the first and last timestamp associated to the total energy deposit
   in the block (two real values in unit of time),
 - the total energy deposit (a real value in unit of energy).

The figure  below illustrates how a  calorimeter hit (the red  box) is
built   by   the   `snemo::simulation::calorimeter_step_hit_processor`
processor  from a  collection  of raw  G4 truth  step  hits (in  blue)
traversing a given scintillator block.

![A mctools::base_step_hit object built by the snemo::simulation::calorimeter_step_hit_processor processor for the \"calo\" collection of hits.](@ref images/fls_calorimeter_step_hit_processor_0.jpg)

The truth tracker hits {#flsimulateoutput_truthtrackerhits}
======================


The  `"gg"` collection  of  truth  hits is  generated  by a  dedicated
 backend `snemo::simulation::gg_step_hit_processor` processor.

The principle of this algorithm is to analyze all raw step hits
generated by Geant4 in the fiducial drift region of a given Geiger cell
 (sensitive volume)
and to randomize the creation of ion/electron first ionization pairs
along the truth track. The created truth pair that is located at the nearest
position to the anode wire is considered as the one that triggers the cell, others
truth pairs are simply ignored.
The final *tracker hit* records the following attributes
in the output `mctools::base_step_hit` object (see [this section](@ref flsimulateoutput_mctoolsbasestephitclass)):
 - the unique identifier of the hit in the collection,
 - the GID locating the Geiger cell where the raw tracker truth hits were produced (geometry type is `1204`),
 - a collection of optional auxiliary properties (`datatools::properties`), for example
   the position of the *minimal approach position* (MAP) and drift distance of the truth track
   with respect to the anode wire,
 - the position of the creation of the truth  ion/electron pair (`geomtools::vector_3d`),
 - the position of the impact of the Geiger avalanche on the anode wire (`geomtools::vector_3d`),
 - the time of creation of the truth ion/electron pair.

The  figure below  illustrates  how  a tracker  hit  is  built by  the
`snemo::simulation::gg_step_hit_processor` processor from a collection
of raw G4 truth  step hits (in blue) traversing the  drift region of a
given Geiger cell.


![A mctools::base_step_hit object built by the snemo::simulation::gg_step_hit_processor processor for the \"gg\" collection of hits.](@ref images/fls_gg_step_hit_processor_0.jpg)

We can  see that, due to  the randomization of the  electron/ion first
ionization pairs,  the position  of the  truth electron/ion  pair that
triggers the  cell (and thus will  be used for the  track measurement)
does not exactly  matches the minimal approach point  (MAP, in purple)
which is  theoriticaly computed  from the positions  of the  raw truth
step and  the anode  wire.  This will  result in a  small bias  in the
reconstruction  of the  drift  distance during  the calibration  step.
Together with the multiple scattering  of electrons (and positrons) in
the tracking gas  and the time resolution of  the tracker electronics,
this is  a source of experimental  error in the reconstruction  of the
charged particles' trajectories.
