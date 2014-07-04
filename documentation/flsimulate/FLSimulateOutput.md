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

A very important concept, massively used within the Falaise simulation and data processing,
is the *geometry identifier* (GID).
A GID is an object that uniquely identifies a geometry volume in the
experimental setup. Given some dedicated rules, the description of the
virtual geometry of the experiment is able to associate such an unique GID
to each volumes of interest in the geometry (this technique is called
*geometry mapping* and is under the responsibility of the
*geometry manager* object and its embedded *GID manager* object).

What is important to know about any GID object is that it stores
two kinds of information:
 - the *geometry type* is an unique positive integer identifier that indicates
   which type of geometry volume is addressed by the GID. A *geometry type*
   is conventionaly associated to a *geometry category* which is an unique human
   readable string (the list of *geometry types and categories* supported by a given
   experimental setup is defined in a special
   configuration file).

   The table below presents some geometry categories of interest in the SuperNEMO demonstrator
   geometry:

| Category | Type | Address contents | Description | Example  |
| -------- | ---- | ---------------- | ----------- | -------- |
| `world`       | `0` | *world* number   | Conventionaly identifies the world volume   | [0:0] represents the top-level geometry volume |
| `module`       | `1000` | *module* number   | Identifies any detector module   | [1000:0] represents the demonstrator module |
| `source_strip` | `1102` | *module* and *strip* numbers  | Identifies any source foil strip | [1102:0.3] represents the fourth strip in the demonstrator module |
| `tracker_submodule` | `1200` | *module* and *side* numbers | Identifies any tracker submodule | [1200:0.1] represents the French side tracker in the demonstrator module |
| `tracker_layer`     | `1202` | *module*, *side* and *layer* numbers | Identifies any tracker layer of drift cells    |  [1202:0.1.8] represents the layer near the wall in the French side tracker |
| `drift_cell_core`   | `1204` | *module*, *side*, *layer* and *row* numbers | Identifies any tracker drift cell    | [1204:0.1.8.112] represents the cell at the French/Tunnel corner of the tracking chamber |
| `xcalo_block`   | `1232` | *module*, *side*, *wall*, *column* and *row* numbers | Identifies any X-calo block    | [1232:0.1.1.0.15] represents the block at the French/Tunnel top corner of the detector |
| `gveto_block`   | `1252` | *module*, *side*, *wall* and *column* numbers | Identifies any gamma veto block    | [1252:0.1.1.15] represents the block at the French/Tunnel top corner of the detector |
| `calorimeter_block`   | `1302` | *module*, *side*, *column*, *row* and *part* numbers | Identifies any main calorimeter block    | [1302:0.1.19.12.1] represents the front part of the scintillator block at the French/Tunnel top corner of the detector |

 - the *address* of the GID is an ordered collection of positive integer identifiers that
   informs about the effective positioning of the volume in the
   geometry hierarchy : module (only 0 for the demonstrator), side number (italian/french), source strip number (from 0 to 35),
   main clorimeter column (from 0 to 19), Geiger cell layer (from 0 to 8)...

It is thus possible to extract these information from any GID object.

Example with GID with value [1200:0.1] :
 - Q: What is your type (or category) ?

   A: The GID will respond `1200`  (this is the type associated to the `tracker_submodule` geometry category)
 - Q: What is the depth of your address ?

   A: The GID will respond `2` because the full path of
   address is made of two numbers, namely the module number (at rank `0`) and the tracker submodule number (at rank `1`).
 - Q: What is your address of rank `0` ?

   A: The GID will respond `0` (this is the *module* number, here the demonstrator module is conventionaly numbered `0`).
 - Q: What is your address of rank `1` ?

   A: The GID will respond `1` (this is the French *tracker submodule* number in the geometry convention of the experiment).

The GID object is thus the unique repository where to store the geometrical identification and localization
of a volume and particularly for Monte-Carlo truth hits.


The mctools::base_step_hit class {#flsimulateoutput_mctoolsbasestephitclass}
================================

The Bayeux/mctools Geant4 plugin used by the FLSimulate application is designed to generate
collections of *truth step hits* through the Geant4 simulation engine. The output data model
contains objects that represents such truth hits. It uses the `mctools::base_step_hit` class.
The  `mctools::base_step_hit` class inherits the `geomtools::base_hit` class.

Each object of the `mctools::base_step_hit` class contains:
 - the unique identifier of the truth hit, with respect to the collection it belongs to
   (an integer, inherited from the `geomtools::base_hit` mother class)
 - the geometry identifier (GID) of the volume where the hit was generated, if available (a `geomtools::geom_id` instance,
   inherited from the `geomtools::base_hit` mother class)
 - a collection of optional auxiliary properties (a `datatools::properties` instance,
   inherited from the `geomtools::base_hit` mother class)
 - the start position of the step hit (a `geomtools::vector_3d` instance)
 - the stop position of the step hit (a `geomtools::vector_3d` instance)
 - the start time of the step hit (a real value in unit of time)
 - the stop time of the step hit (a real value in unit of time)
 - the start momentum of the step hit (a real value in unit of energy)
 - the stop momentum of the step hit (a real value in unit of energy)
 - the energy deposit along the track segment (a real value in unit of energy)
 - the particle name (a character string provided by Geant4)

The recording of these basic attributes is optional. Users are free to parametrized only a subset
of truth step's attributes to be stored in the output data model. This is configured while combining
the sensitive detector associated to some volumes and the step hit processor attached to the
sensitive detector. Note also that the meaning and the need of these attributes
may change depending of the nature of the sensitive detector.

Note that the collection of auxiliary properties can be used to store, on user request,
additional informations like:
 - the identifier of the corresponding G4 truth hit (a positive integer value provided by Geant4)
 - the name of step processor that produced the truth hit (a character string)
 - the identifier of the G4 track (a positive integer value provided by Geant4)
 - the identifier of the parent G4 track (a positive integer value provided by Geant4)
 - the kinetic energy of the particle at the beginning of the step  (a real value in unit of energy)
 - the kinetic energy of the particle at the end of the step  (a real value in unit of energy)
 - the *primary particle* flag (a boolean value set to `1` for a primary particle, equivalent to parent track identifier set to `0`)
 - the name of the G4 creator process (a character string provided by Geant4)
 - the name of the G4 creator category (a character string provided by Geant4)
 - the *delta-ray from alpha* flag (a boolean value set to `1` for a low-energy secondary electron generated along the track of an
   an alpha particle, typically used to compute the quenching factor in some material)
 - the *entering volume* flag (a boolean value set to `1` for a particle entering a volume by Geant4)
 - the *leaving volume* flag (a boolean value set to `1` for a particle leaving a volume by Geant4)
 - the name of the G4 volume (a character string provided by Geant4)
 - the copy number of the G4 volume (a positive integer value provided by Geant4)

Given a collection of such truth hits, it is expected that users will be able to apply some
arbitrary digitization algorithm.

The mctools::simulated_data class {#flsimulateoutput_mctoolssimulateddata}
=================================

The `SD` bank created by the simulation within each event record contains
a `mctools::simulated_data` object.

Each `mctools::simulated_data` object contains:
 - the primary vertex (a `geomtools::vector_3d` instance)
 - the primary generated event (particle types and kinematics, a `genbb::primary_event` instance)
 - a collection of optional auxiliary properties (a `datatools::properties` instance)
 - a dictionary of collections of shared handles to `mctools::base_step_hit` objects. This dictionary
   owns several collection of truth hits (eventually managed through *shared handles*, see [here](@ref flreconstructpipelineoutput_thesharedhandleconcept)), depending on the parametrization  of the simulation output profiles.
   For the SuperNEMO demonstrator, we use the following collections of truth hits:
   - "calo" : calorimeter truth hits collected from the scintillator blocks in the main walls.

     These hits are built from G4 raw truth hits by the `calorimeter.hit_processor` step hit backend processor.
   - "xcalo" : calorimeter truth hits collected from the scintillator blocks in the X-walls.

     These hits are built from G4 raw truth hits by the `xcalorimeter.hit_processor` step hit backend processor.
   - "gveto" : calorimeter truth hits collected from the gamma veto scintillator blocks

     These hits are built from G4 raw truth hits by the `gveto.hit_processor` step hit backend processor.
   - "gg" : Geiger hits collected from the tracker cell drift region (considering the Geiger regime).

     These hits are built from G4 raw truth hits by the `gg.hit_processor` step hit backend processor.
   - "__visu.track" : raw truth step hits collected from various part of the detector including not onmy the sensitive
     detectors (scintillator blocks, Geiger cells) but also some non-sensitive volumes of the geometry (source foil strips...).

     These hits are built from G4 raw truth hits by many step hit backend
     processors of the `mctools::push_all_step_hit_processor`  class. They are only generated for specific  output profiles
     and not produced by default.

TODO:
 - Describe the `genbb::primary_event`
 - Describe the details of calorimeter anf Geiger truth hits
