Falaise MockTrackerClusterizer plugin
=====================================

This  plugin for  Falaise  implement some  processing  module for  the
SuperNEMO reconstruction pipeline.

It uses  the very  simple MockTrackerClusterizer algorithm  to perform
the clusterization of  simulated tracker Geiger hits  in the SuperNEMO
experiment.

The  module  uses  the  ``"CD"``  bank  (the  default  name  from  the
``<Falaise  source dir>/source/falaise/snemo/datamodels/data_model.h``
header file)  as the source  of input calibrated Geiger  hits, through
the  ``snemo::datamodels::calibrated_data``  class  and  its  embedded
collection  of ``snemo::datamodels::calibrated_tracker_hit``  objects.
The  output   consists  in  the   ``"TCD"``  bank  which   contains  a
``snemo::datamodels::tracker_clustering_data``  object.   This  object
stores possible clustering solutions.

BEWARE:

This module does not work on real data, only on simulated data because
it relies  on the natural ordering  of true particles' steps  from the
Geant4 simulation engine.

Authors:

 * Xavier Garrido <garrido@lal.in2p3.fr>
