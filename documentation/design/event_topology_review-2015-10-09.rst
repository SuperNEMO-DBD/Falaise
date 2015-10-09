===================================================================
Review of the ``snemo::datamodel`` event topology class design
===================================================================

:Authors: - `S. Calvez`_
          - `X. Garrido`_
          - `F. Mauger`_
:Institution: SuperNEMO Collaboration
:Date: 2015-10-09

.. _`S. Calvez`: calvez@lal.in2p3.fr
.. _`X. Garrido`: garrido@lal.in2p3.fr
.. _`F. Mauger`: mauger@lpccaen.in2p3.fr


Introduction
============

Meeting 2015-10-09: discussion about the design of the *evet topology* data models.

Purpose:

 * review the design of the prototype ``ParticleIdentification`` module, including the data models
   and associated processing.

   URL: https://github.com/xgarrido/ParticleIdentification/tree/master/source/falaise/snemo/datamodels

 * Propose a collection of classes for (2e), (2e1a), (1eNg), (1eNg1a)... event topology channels

PRELIMINARY : To be discussed more...


Suggestions
===========

Refactorying of the ``snemo::datamodel::base_topology_measurement`` class
--------------------------------------------------------------------------------


.. code-block:: c++

 namespace snemo {

   namespace datamodel {

    class base_topology_measurement : DATATOOLS_SERIALIZABLE_CLASS,
                                  public datatools::i_tree_dumpable
    {
       virtual void register_particle_track_id(const std::string & id_)
       {
          _particle_track_ids_.push_back(id_);
	  return;
       }

       virtual const std::vector<std::string> & particle_track_ids() const
       {
         return _particle_track_ids_;
       }

    private:
      std::vector<std::string> _particle_track_ids_;
    };

Refactorying of the ``snemo::datamodel::base_topology_pattern`` class
--------------------------------------------------------------------------------


.. code-block:: c++

 namespace snemo {

   namespace datamodel {

    class base_topology_pattern : DATATOOLS_SERIALIZABLE_CLASS,
                                  public datatools::i_tree_dumpable
    {
    public:

       virtual std::string pattern_id() const = 0;

       virtual void build_particle_tracks_dictionary(std::map<std::string, particle_track::handle_type> & tracks_) const = 0;

       virtual void build_measurement_dictionary(std::map<std::string, const base_measurement *> & meas_) const = 0;

    };

  }

}

Refactorying of the ``snemo::datamodel::topology_2e_pattern`` class
--------------------------------------------------------------------------------

Refactor the  ``snemo::datamodel::topology_2e_pattern`` class  to explicitly
host a pair of electron (e- or e+) particle tracks (by handle):

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     //! \brief Base class for all trajectory objects in the SuperNEMO detector
     class topology_2e_pattern : public base_topology_pattern
     {
     public:
       typedef std::pair<particle_track::handle_type> electron_pair_type;

       virtual std::string pattern_id() const
       {
          return std::string("2e");
       }

       virtual void build_particle_tracks_dictionary(std::map<std::string, particle_track::handle_type> & tracks_) const
       {
	 tracks_["e1"] = _pair_2e_.first;
	 tracks_["e2"] = _pair_2e_.second;
         return;
       }

       virtual void build_measurement_dictionary(std::map<std::string, const base_measurement *> & meas_) const
       {
         if (_tof_2e_.is_valid()) {
	    meas_["tof_2e"] = &_tof_2e_;
	 }
         if (_delta_vertices_source_2e_.is_valid()) {
	    meas_["delta_vertices_source_2e"] = &_delta_vertices_source_2e_;
	 }
         if (_angle_2e_.is_valid()) {
	    meas_["angle_2e"] = &_angle_2e_;
	 }
         if (_energy_2e_.is_valid()) {
	    meas_["energy_2e"] = &_energy_2e_;
	 }
         return;
       }

       const electron_pair_type & get_pair_2e() const;

       electron_pair_type & grab_pair_2e();

       ...

     private:
       electron_pair_type         _pair_2e_;  //!< Handle to a trajectory fitted pattern

       TOF_measurement            _tof_2e_;   //!< Time-Of-Flight meas.
       delta_vertices_measurement _delta_vertices_source_2e_; //!< Delta vertices on source foil
       angle_measurement          _angle_2e_; //!< Angle meas.
       // get_minimal_energy(), get_maximal_energy(), get_energy_difference(), get_energy_sum()
       energy_measurement         _energy_2e_;

     };


Application to an inherited  ``snemo::datamodel::topology_2e1g_pattern`` class
--------------------------------------------------------------------------------

Create a  ``snemo::datamodel::topology_2e1g_pattern`` class:

.. code-block:: c++

 namespace snemo {

   namespace datamodel {

     //! \brief Base class for all trajectory objects in the SuperNEMO detector
     class topology_2e1g_pattern : public topology_2e_pattern
     {
     public:

       virtual std::string pattern_id() const
       {
          return std::string("2e1g");
       }

       virtual void build_particle_tracks_dictionary(std::map<std::string, particle_track::handle_type> & tracks_) const
       {
         topology_2e_pattern::build_particle_tracks_dictionary(tracks_);
 	 tracks_["g1"] = _gamma_;
         return;
       }

       virtual void build_measurement_dictionary(std::map<std::string, const base_measurement *> & meas_) const
       {
         topology_2e_pattern::build_list_of_measurements(meas_);
         if (_energy_g1_.is_valid()) {
	    meas_["energy_g1"] = &_energy_g1_;
	 }
         if (_angle_e1_g1_.is_valid()) {
	    meas_["angle_e1_g1"] = &_angle_e1_g1_;
	 }
         if (_angle_e2_g1_.is_valid()) {
	    meas_["angle_e2_g1"] = &_angle_e2_g1_;
	 }
         if (_tof_e1_g1_.is_valid()) {
	    meas_["tof_e1_g1"] = &_tof_e1_g1_;
	 }
         if (_tof_e2_g1_.is_valid()) {
	    meas_["tof_e2_g1"] = &_tof_e2_g1_;
	 }
         return;
       }

     private:

       particle_track     _g1_;
       energy_measurement _energy_g1_;
       angle_measurement  _angle_e1_g1_;
       angle_measurement  _angle_e2_g1_;
       TOF_measurement    _tof_e1_g1_;
       TOF_measurement    _tof_e2_g1_;

     };

   }

 }
