/// \file falaise/snemo/digitization/digitization_driver.cc

// Ourselves:
#include <snemo/digitization/digitization_driver.h>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
// #include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace digitization {

    digitization_driver::digitization_driver()
    {
      _initialized_ = false;
      return;
    }

    digitization_driver::~digitization_driver()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void digitization_driver::set_geometry_manager(const geomtools::manager & geo_mgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized/locked !");
      _geometry_manager_ = & geo_mgr_;
      return;
    }

    const geomtools::manager & digitization_driver::get_geometry_manager() const
    {
      DT_THROW_IF (!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    }


    bool digitization_driver::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    void digitization_driver::initialize(const datatools::properties & /* setup_ */ )
    {
      DT_THROW_IF (!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      
      // do something

      _initialized_ = true;
      return;
    }

    bool digitization_driver::is_initialized() const
    {
      return _initialized_;
    }
    
    void digitization_driver::reset()
    {
      return;
    }

    void digitization_driver::process_digitization_algorithms(const mctools::signal::signal_data & /*SSD_*/)
    {
      
      
      
      
      return;
    }

  }  // end of namespace digitization

}  // end of namespace snemo
/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::digitization::digitization_driver, ocd_)
{
  ocd_.set_class_name("snemo::digitization::digitization_driver");
  ocd_.set_class_description("A driver class for the Digitization algorithm");
  ocd_.set_class_library("Falaise_Digitization");
  ocd_.set_class_documentation("The driver manager for the Digitization algorithms \n"
                               );


  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::digitization::digitization_driver,
                               "snemo::digitization::digitization_driver")
