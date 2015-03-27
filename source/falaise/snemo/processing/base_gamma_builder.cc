/// \file falaise/snemo/processing/base_gamma_builder.cc

// Ourselves:
#include <snemo/processing/base_gamma_builder.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project:
#include <snemo/geometry/locator_plugin.h>
#include <snemo/geometry/calo_locator.h>
#include <snemo/geometry/xcalo_locator.h>
#include <snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace processing {

    datatools::logger::priority base_gamma_builder::get_logging_priority() const
    {
      return _logging_priority;
    }

    void base_gamma_builder::set_logging_priority(datatools::logger::priority priority_)
    {
      _logging_priority = priority_;
      return;
    }

    const std::string & base_gamma_builder::get_id() const
    {
      return _id_;
    }

    const snemo::geometry::calo_locator & base_gamma_builder::get_calo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_calo_locator();
    }

    const snemo::geometry::xcalo_locator & base_gamma_builder::get_xcalo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_xcalo_locator();
    }

    const snemo::geometry::gveto_locator & base_gamma_builder::get_gveto_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_gveto_locator();
    }

    bool base_gamma_builder::is_initialized() const
    {
      return _initialized_;
    }

    void base_gamma_builder::_set_initialized(bool i_)
    {
      _initialized_ = i_;
      return;
    }

    void base_gamma_builder::_initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver '" << get_id() << "' is already initialized !");

      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(! get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      //Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration(setup_);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      // Get geometry locator plugin
      const geomtools::manager & geo_mgr = get_geometry_manager();
      std::string locator_plugin_name;
      if (setup_.has_key("locator_plugin_name")) {
        locator_plugin_name = setup_.fetch_string("locator_plugin_name");
      } else {
        // If no locator plugin name is set, then search for the first one
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
            DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      DT_THROW_IF(! geo_mgr.has_plugin(locator_plugin_name) ||
                  ! geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name),
                  std::logic_error,
                  "Found no locator plugin named '" << locator_plugin_name << "'");
      _locator_plugin_ = &geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
      return;
    }

    void base_gamma_builder::_reset()
    {
      _set_initialized(false);
      this->base_gamma_builder::_set_defaults();
      return;
    }

    void base_gamma_builder::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized/locked !");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & base_gamma_builder::get_geometry_manager() const
    {
      DT_THROW_IF (! has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    }


    bool base_gamma_builder::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    void base_gamma_builder::_set_defaults()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
      _geometry_manager_ = 0;
      _locator_plugin_ = 0;
      return;
    }

      // Constructor
    base_gamma_builder::base_gamma_builder(const std::string & id_)
    {
      _id_ = id_;
      _set_initialized(false);
      _set_defaults();
      return;
    }

    base_gamma_builder::~base_gamma_builder()
    {
      if (is_initialized()) {
        _reset();
      }
      return;
    }

    // void base_gamma_builder::tree_dump(std::ostream & out_,
    //                                     const std::string & title_,
    //                                     const std::string & indent_,
    //                                     bool inherit_) const
    // {
    //   std::string indent;
    //   if (! indent_.empty()) {
    //     indent = indent_;
    //   }
    //   if ( ! title_.empty() ) {
    //     out_ << indent << title_ << std::endl;
    //   }

    //   out_ << indent << datatools::i_tree_dumpable::tag
    //        << "Logging          : '"
    //        << datatools::logger::get_priority_label(_logging_priority) << "'"
    //        << std::endl;
    //   out_ << indent << datatools::i_tree_dumpable::tag
    //        << "Initialized      : " << is_initialized() << std::endl;
    //   out_ << indent << datatools::i_tree_dumpable::tag

    //   out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
    //        << "End." << std::endl;
    //   return;
    // } // end of base_gamma_builder::tree_dump


    int base_gamma_builder::process(snemo::datamodel::particle_track_data & ptd_)
    {
      int status = 0;
      DT_THROW_IF (! is_initialized(), std::logic_error, "Gamma builder '" << _id_ << "' is not initialized !");

      status = _process_algo(ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of '" << _id_ << "' algorithm has failed !");
        return status;
      }
      return status;
    }

    // // static
    // void base_gamma_builder::ocd_support(datatools::object_configuration_description & ocd_,
    //                                       const std::string & prefix_)
    // {

    //   datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_);

    //   return;
    // }

  }  // end of namespace processing

}  // end of namespace snemo
