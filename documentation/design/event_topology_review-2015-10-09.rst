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

Meeting 2015-10-09/16: discussion about the design of the *event topology* data models.

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

       // virtual std::string pattern_id() const = 0;

       // Constness of particle_track::handle_type...
       // virtual void build_particle_tracks_dictionary(std::map<std::string, particle_track::handle_type> & tracks_) const = 0;

       // virtual void build_measurement_dictionary(std::map<std::string, const base_measurement *> & meas_) const = 0;

    };

  }
}

namespace snemo {

  namespace reconstruction {

    struct measurement_driver_col_type {
       tof_driver               * TOFD;
       delta_vertices_driver    * DVD;
       angle_measurement_driver * AMD;
    };

-------------------------------------------
#include <datatools/factory_macros.h>

Dans base_topology_builder.h

namespace snemo {

  namespace reconstruction {

    class base_topology_builder
    {
      void set_measurement_driver(measurement_driver_col_type &);
      virtual datatools::handle<base_topology_pattern> create_pattern() = 0;
      virtual void build(const snemo::datamodel::particle_track_data & source_, base_topology_pattern & target_) = 0;

    private:

     // working external driver:
     measurement_driver_col_type * _drivers_;

     // Factory stuff :
     DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_topology_builder);

    };
  }
}

/** Interface macro for automated registration of a cut class in the global register */
#define FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_INTERFACE(BuilderType)           \
  public:                                       \
  virtual std::string get_type_id() const; \
  private:                                                                \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::snemo::reconstruction::base_topology_builder, BuilderType); \
  /**/

/** Implementation macro for automated registration of a cut class in the global register */
#define FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_IMPLEMENT(BuilderType,BuilderID)               \
  std::string T::get_type_id() const { return BuilderID; } \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::snemo::reconstruction::base_topology_builder, T, BuilderID); \

  /**/

Dans base_topology_builder.cc

namespace snemo {

  namespace reconstruction {

    DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_topology_builder, "snemo::reconstruction::base_topology_builder/__system__");

  }
}




Dans topology_1e_builder.h


    class topology_1e_builder : public base_topology_builder
    {
    public:
       virtual datatools::handle<base_topology_pattern> create_pattern()
       {
         datatools::handle<base_topology_pattern> h(new topology_1e_pattern);
	 return h;
       };
       virtual void build(const snemo::datamodel::particle_track_data & source_, base_topology_pattern & target_) = 0;
    private:


      FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_INTERFACE(topology_1e_builder);

    };


Dans topology_1e_builder.cc

      FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_IMPLEMENT(topology_1e_builder, "snemo::reconstruction::topology_1e_builder");

Dans topology_2e_builder.h

    class topology_2e_builder : public base_topology_builder
    {
    public:
      virtual datatools::handle<base_topology_pattern> create_pattern()
      {
        datatools::handle<base_topology_pattern> h(new topology_2e_pattern);
        return h;
      };
      virtual void build(const snemo::datamodel::particle_track_data & source_, base_topology_pattern & target_) = 0;
    private:

       FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_INTERFACE(topology_2e_builder);

    };

  }

}

Dans topology_1e_builder.cc

      FL_SNEMO_RECONSTRUCTION_TOPOLOGY_BUILDER_REGISTRATION_IMPLEMENT(topology_2e_builder, "snemo::reconstruction::topology_2e_builder")


  supported_classifications : string[2] = "1e" "2e"


  initialize(...)
  {
   map<classification, builder> class2build_dict = { { "1e", "snemo::datamodel::topology_1e_builder"} ,
     {} ...  }
 if (
    class2build_dict["1e"]   = "snemo::datamodel::topology_1e_builder";
    class2build_dict["1e?g"] = "snemo::datamodel::topology_1eNg_builder";
    class2build_dict["2e"]   = "snemo::datamodel::topology_2e_builder";
    class2build_dict["2e?g"] = "snemo::datamodel::topology_2eNg_builder";
  }

}

topology_driver::_process_algo(...)
{

  const std::string a_classification = topology_driver::_build_classification_(ptd_);
   // regex machinery...
   std::string builder_class_id = class2build_dict[a_classification];
   ...
   FB = qc(builder_class_id);
   base_topology_builder * new_builder = FB();

   base_topology_pattern * new_pattern = new_builder->create_pattern();

   snemo::datamodel::topology_data::handle_pattern h_pattern;
   h_pattern.reset(FP);
   td_.set_pattern_handle(h_pattern);

   snemo::datamodel::topology_data::handle_pattern h_pattern;
   h_pattern.reset(FP);
   td_.set_pattern_handle(h_pattern);

   FB->build(ptd_,
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

       const TOF_measurement & get_tof_2e() const;
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
