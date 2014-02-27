Falaise TrackFit plugin
=======================

This  plugin for  Falaise  implement some  processing  module for  the
SuperNEMO reconstruction pipeline.

It uses the TrackFit algorithm, a  library dedicated to the linear and
helix  fitting of  clusters of  tracker Geiger  hits in  the SuperNEMO
experiment.

The module  uses the  "CD" bank  (the default  name from  the <Falaise
srouce dir>/source/falaise/snemo/datamodels/data_model.h  header file)
as  the   source  of  input   calibrated  Geiger  hits,   through  the
snemo::datamodels::calibrated_data class  and its  embedded collection
of  snemo::datamodels::calibrated_tracker_hit   objects.   The  output
consists     in     the     "TCD"    bank     which     contains     a
snemo::datamodels::tracker_clustering_data object.  This object stores
possible clustering solutions.


Authors:

 * François Mauger <mauger@lpccaen.in2p3.fr>
 * Xavier Garrido <garrido@lal.in2p3.fr>


UML
---


+-------------------------+
| tracker_clustering_data |            +-----------------------------+
+-------------------------+          * | tracker_clustering_solution |
| - solutions             -----------<>+-----------------------------+
+-------------------------+            | - solution_id               |
                              +--------- - clusters                  |
                              |        | - unclustered_hits          ----+
     +-----------------+ *    |        +-----------------------------+   |
     | tracker_cluster |<>----+                                          |
     +-----------------+            +------------------------+           |
     | - hits          -----------<>| calibrated_tracker_hit | *         |
     +-----------------+       1..* +------------------------+<>---------+
                                    +------------------------+
