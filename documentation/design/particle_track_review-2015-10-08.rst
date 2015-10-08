===================================================================
Review of the ``snemo::datamodel::particle_track`` class design
===================================================================

:Authors: - `S. Calvez`_
          - `X. Garrido`_
          - `F. Mauger`_
:Institution: SuperNEMO Collaboration
:Date: 2015-10-08

.. _`S. Calvez`: calvez@lal.in2p3.fr
.. _`X. Garrido`: garrido@lal.in2p3.fr
.. _`F. Mauger`: mauger@lpccaen.in2p3.fr


Introduction
============

Meeting 2015-10-08: discussion about the design of the *particle track* data model:

 * for charged particle in the tracker (results of CAT+TrackFit reconstruction algos)
 * for gamma rays in the calorimeter (results of the GammaTracking reconstruction algo)

Implement an unique interface starting from the legacy ``particle_track``
and ``tracker_trajectory`` classes.

We are doing suggestions below to redesign these data models in a more uniform way.


Suggestions
===========

Refactorying of the ``snemo::datamodel::tracker_trajectory`` class
------------------------------------------------------------------

Implement  a new  ``snemo::datamodel::base_trajectory`` class  to host
the  trajectory pattern  (currently  supported  patterns are:  *line*,
*polyline* or *helix*):

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     //! \brief Base class for all trajectory objects in the SuperNEMO detector
     class base_trajectory : public geomtools::base_hit
     {
     public:
       typedef datatools::handle<base_trajectory> handle_type;
       ...
     private:
       handle_pattern _pattern_; //!< Handle to a trajectory fitted pattern
     };

   }

 }

Refactor     the    ``snemo::datamodel::tracker_trajectory``     class
explicitely  inherited from  the ``snemo::datamodel::base_trajectory``
class:

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     /// \brief A trajectory for a charged particle traversing the tracking chamber
     ///        and made of Geiger calibrated hits referenced by handles
     class tracker_trajectory : public base_trajectory
     {
       ...
     private:
       handle_cluster          _cluster_; ///< Handle to the fitted cluster
       orphans_collection_type _orphans_; ///< Collection of orphan Geiger hit handles
     };

   }

 }

Introducing of the ``snemo::datamodel::calorimeter_trajectory`` class
----------------------------------------------------------------------

Implement a new  ``snemo::datamodel::calorimeter_trajectory`` class to
host  the  trajectory  pattern  for  gamma  rays  in  the  calorimeter
(+X-walls and Gamma-veto) (from  the ``base_trajectory`` mother class,
the expected supported patterns are: *line*, *polyline*):

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     /// \brief A trajectory for a particle hitting a set of optical modules
     ///        in the calorimeter. Calorimeter hits are stored by handles
     class calorimeter_trajectory : public  base_trajectory
     {
       ...
     private:
       calibrated_calorimeter_hit::collection_type _hits_; //!< Collection of the fitted calorimeter hit handles
     };

   }

 }

Refactorying of the ``snemo::datamodel::particle_track`` class
-------------------------------------------------------------------

The ``_trajectory_`` attributes should refer to any of the *tracker* or *calorimeter* trajectory models.

The meaning of ``_associated_calorimeter_hits_`` depends on the trajectory: to be discussed

  * tracker: the legacy *fitted track to hit calo block* association
  * calorimeter: the list of hit calo blocks participating to the *gamma* path

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     class particle_track : public geomtools::base_hit
     {
       ...
      private:
       base_trajectory::handle_type _trajectory_;         //!< Handle to the fitted trajectory
       charge_type                  _charge_from_source_; //!< Particle charge
       vertex_collection_type       _vertices_;           //!< Collection of vertices
       calibrated_calorimeter_hit::collection_type _associated_calorimeter_hits_; //!< Collection of associated calorimeter hits
     };

   }

 }


Caution
----------------------------------------------------------------------

Special care should be done to adapt the serialization features to this new class layout: to be discussed.
