Falaise GammaClustering plugin
==============================

This plugin for Falaise implements a gamma tracko-clustering algorithm for the
SuperNEMO reconstruction pipeline.

Given a list of unassociated calorimeter hits from the
``snemo::datamodel::particle_track_data`` bank, the algorithm clusters first the
neighbouring calorimeter hits and then, try to connect clusters (mainly from one
calorimeter wall to the other) using Time-Of-Flight measurements.

The module uses the ``"PTD"`` bank (the default name from the ``<Falaise source
dir>/source/falaise/snemo/datamodels/data_model.h`` header file) as the source
of unassociated calorimeter hits, through the
``snemo::datamodels::particle_track_data`` class.  The plugin enrichs the
``"PTD"`` bank by adding new gamma through the
``snemo::datamodels::particle_track`` object. For each gamma particle track, a
list of associated calorimeter hits + calorimeter vertices is stored.

Authors:

 * Steven Calvez  <calvez@lal.in2p3.fr>
 * Xavier Garrido <garrido@lal.in2p3.fr>
