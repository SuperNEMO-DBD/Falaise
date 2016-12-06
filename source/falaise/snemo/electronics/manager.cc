// snemo/electronics/manager.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves :
#include <snemo/electronics/manager.h>

// Standard library :
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project :

namespace snemo {

  namespace electronics {

    manager::manager()
    {
      return;
    }

    manager::~manager()
    {
      if (this->manager::is_initialized()) {
        this->manager::reset();
      }
      return;
    }

    bool manager::has_geometry_manager() const
    {
      return _geometry_manager_ != nullptr;
    }

    void manager::set_geometry_manager(const geomtools::manager & geo_mgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Electronics manager is already initialized ! ");
      DT_THROW_IF(!geo_mgr_.is_initialized(),
                  std::logic_error,
                  "Geometry manager is not initialized ! ");
      _geometry_manager_ = &geo_mgr_;
      return;
    }

    const geomtools::manager & manager::get_geometry_manager() const
    {
      DT_THROW_IF(_geometry_manager_ == nullptr, std::logic_error,
                  "Geometry manager is not set!");
      return *_geometry_manager_;
    }

    bool manager::has_cabling() const
    {
      if (_no_cabling_) return false;
      if (!_cabling_.is_initialized()) return false;
      return true;
    }

    bool manager::is_initialized() const
    {
      return _initialized_;
    }

    void manager::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Electronics manager is already initialized ! ");

      if (config_.has_key("no_cabling")) {
        _no_cabling_ = config_.fetch_boolean("no_cabling");
      }

      if (!_no_cabling_) {
        datatools::properties cabling_config;
        config_.export_and_rename_starting_with(cabling_config, "cabling.", "");
        _init_cabling_(cabling_config);
      }

      _initialized_ = true;
      return;
    }

    void manager::_init_cabling_(const datatools::properties & cabling_config_)
    {
      // TO BE DONE...
      DT_THROW_IF(!has_geometry_manager(), std::logic_error,
                  "No geometry manager is available from the electronics manager!");
      int module_number = 0;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      // fetch from config...

      _cabling_.set_logging(logging);
      _cabling_.set_geo_manager(get_geometry_manager());
      _cabling_.set_module_number(module_number);
      _cabling_.initialize();
      return;
    }

    void manager::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Electronics manager is not initialized ! ");
      _initialized_ = false;
      _reset_cabling_();
      _geometry_manager_ = nullptr;
      _no_cabling_ = false;
      return;
    }

    void manager::_reset_cabling_()
    {
      _cabling_.reset();
      return;
    }

    void manager::tree_dump(std::ostream & a_out ,
                            const std::string & a_title,
                            const std::string & a_indent,
                            bool a_inherit) const
    {

      a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
            << "Cabling : " << (is_initialized() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  } // end of namespace electronics

} // end of namespace snemo
