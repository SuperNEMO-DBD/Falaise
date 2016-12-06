// snemo/electronics/electronics_service.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves :
#include <snemo/electronics/mapping.h>

// Standard library :
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This project :
#include <snemo/electronics/manager.h>

namespace snemo {

  namespace electronics {

    electronics_service::electronics_service()
    {
      return;
    }

    electronics_service::~electronics_service()
    {
      if (this->electronics_service::is_initialized()) {
        this->electronics_service::reset();
      }
      return;
    }

    const std::string & electronics_service::get_geo_label() const
    {
      return _Geo_label_;
    }

    void electronics_service::set_geo_label(const std::string & label_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Electronics service '" << get_name() << "' is already initialized ! ");
      _Geo_label_ = label_;
      return;
    }

    bool electronics_service::has_geometry_manager() const
    {
      return _geometry_manager_ != nullptr;
    }

    void electronics_service::set_geometry_manager(const  geomtools::manager & geo_mgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Electronics service '" << get_name() << "' is already initialized ! ");
      DT_THROW_IF(!geo_mgr_->is_initialized(),
                  std::logic_error,
                  "Geometry manager is not initialized ! ");
      _geometry_manager_ = &geo_mgr_;
      return;
    }

    const geomtools::manager & electronics_service::get_geometry_manager() const
    {
      DT_THROW_IF(_geometry_manager_ = nullptr, std::logic_error,
                  "Geometry manager is not set!");
      return *_geometry_manager_;
    }

    bool electronics_service::has_electronics_manager() const
    {
      return _electronics_manager_ != nullptr;
    }

    const manager & electronics_service::get_electronics_manager() const
    {
      DT_THROW_IF(_electronics_manager_ = nullptr, std::logic_error,
                  "Electronics manager is not set!");
      return *_electronics_manager_;
    }

    bool electronics_service::is_initialized() const
    {
      return _initialized_;
    }

    int electronics_service::initialize(const datatools::properties & config_,
                                        datatools::service_dict_type & service_dict_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Electronics service '" << get_name() << "' is already initialized ! ");

      base_service::common_initialize(a_config);

      if (!has_geometry_manager()) {
        if (_Geo_label.empty()) {
          if (config_.has_key("Geo_label")) {
            set_geo_label(config_.fetch_key("Geo_label"));
          }
        }
        const geomtools::geometry_service & geoserv = datatools::get(service_dict_, _Geo_label_);
        set_geometry_manager(geoserv.get_geom_manager());
      }

      //Init the elec manager

      _initialized_ = true;
      return EXIT_SUCCESS;
    }

    int electronics_service::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Electronics service '" << get_name() << "' is not initialized ! ");
      _initialized_ = false;

      if (_electronics_manager_ != nullptr) {
        delete _electronics_manager_;
        _electronics_manager_ = nullptr;
      }

      _geometry_manager_ = nullptr;

      return EXIT_SUCCESS;
    }

    void electronics_service::tree_dump(std::ostream & a_out ,
                                        const std::string & a_title,
                                        const std::string & a_indent,
                                        bool a_inherit) const
    {
      this->base_service::tree_dump(a_out, a_title, a_indent, true);
      a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
            << "Electronics manager : " << _electronics_manager_ << " " << std::endl;
      return;
    }

  } // end of namespace electronics

} // end of namespace snemo
