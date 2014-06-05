Falaise ChargedParticleTracking plugin
======================================

This plugin for Falaise implements some processing module for the SuperNEMO
reconstruction pipeline.

It determines several "physical" quantities from a fitted trajectory such as
electric charge (asumming particle comes from the source foil), it extrapolates
the trajectory vertices and finally, associates trajectory to calorimeter hits :
the module provides a physical interpretation of the fitted trajectories given
the SuperNEMO geometry.

The module uses the ``"TTD"`` bank (the default name from the ``<Falaise source
dir>/source/falaise/snemo/datamodels/data_model.h`` header file) as the source
of input tracker trajectories, through the
``snemo::datamodels::tracker_trajectory_data`` class and its embedded collection
of ``snemo::datamodels::tracker_trajectory`` objects.  The output consists in
the ``"PTD"`` bank *i.e.* ``snemo::datamodels::particle_track_data`` which
contains a collection of ``snemo::datamodels::particle_track``.

Authors:

 * Xavier Garrido <garrido@lal.in2p3.fr>
