/* event_browser_module.cc */

#include <falaise/snemo/view/event_browser_module.h>
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/event_browser_ctrl.h>
#include <falaise/snemo/view/options_manager.h>

#include <falaise/snemo/io/event_server.h>

#include <falaise/snemo/detector/detector_manager.h>

#include <sstream>

// Third party:
// - ROOT
#include <TApplication.h>
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// SuperNEMO services
#include <falaise/snemo/processing/services.h>

namespace snemo {

  namespace visualization {

    namespace view {

      // Registration instantiation macro :
      DPP_MODULE_REGISTRATION_IMPLEMENT(event_browser_module,
                                        "snemo::visualization::view::event_browser_module");

      void event_browser_module::set_geometry_manager(const geomtools::manager & geometry_manager_)
      {
        DT_THROW_IF(_geometry_manager_ != 0 && _geometry_manager_->is_initialized(),
                    std::logic_error, "Embedded geometry manager is already initialized !");
        _geometry_manager_ = &geometry_manager_;
        return;
      }

      // Constructor :
      event_browser_module::event_browser_module(datatools::logger::priority logging_priority_)
        : dpp::base_module(logging_priority_)
      {
        _geometry_manager_   = 0;
        _event_browser_      = 0;
        _event_browser_ctrl_ = 0;
        return;
      }

      // Destructor :
      event_browser_module::~event_browser_module()
      {
        if (is_initialized()) event_browser_module::reset();
        return;
      }

      // Initialization :
      void event_browser_module::initialize(const datatools::properties  & config_,
                                            datatools::service_manager   & service_manager_,
                                            dpp::module_handle_dict_type & /*module_dict_*/)
      {
        DT_THROW_IF(is_initialized(), std::logic_error,
                    "Module '" << get_name() << "' is already initialized ! ");

        this->base_module::_common_initialize(config_);

        std::string geo_label = snemo::processing::service_info::default_geometry_service_label();
        if (config_.has_key("Geo_label")) {
          geo_label = config_.fetch_string("Geo_label");
        }
        if (_geometry_manager_ == 0) {
          // Access to the Geometry Service :
          DT_THROW_IF(geo_label.empty(), std::logic_error,
                      "Module '" << get_name() << "' has no valid 'Geo_label' property !");
          DT_THROW_IF(! service_manager_.has(geo_label) ||
                      ! service_manager_.is_a<geomtools::geometry_service>(geo_label),
                      std::logic_error,
                      "Module '" << get_name() << "' has no '" << geo_label << "' service !");
          // Fetch a reference to the geometry service :
          geomtools::geometry_service & Geo
            = service_manager_.get<geomtools::geometry_service>(geo_label);
          // Request for a reference to the geometry manager and installation
          // in the simulation manager :
          set_geometry_manager(Geo.get_geom_manager());
        }
        DT_THROW_IF(_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");

        // Event browser settings
        if (config_.has_key("browser.logging.priority")) {
          datatools::logger::priority p
            = datatools::logger::get_priority(config_.fetch_string("browser.logging.priority"));
          if (p != datatools::logger::PRIO_UNDEFINED) {
            options_manager::get_instance().set_logging_priority(p);
          }
        }

        if (config_.has_key("browser.style_config_file")) {
          std::string path = config_.fetch_string("browser.style_config_file");
          datatools::fetch_path_with_env(path);
          options_manager::get_instance().set_style_config_file(path);
        }

        // Allocate internal resources :
        _initialize_event_browser();

        // Initialization stops here.
        _set_initialized(true);
        return;
      }

      // Reset :
      void event_browser_module::reset()
      {
        DT_LOG_TRACE(get_logging_priority(), "Entering...");
        DT_THROW_IF(! is_initialized(),
                    std::logic_error,
                    "Module '" << get_name() << "' is not initialized !");

        if (_event_browser_ctrl_ != 0) {
          // Destruction of the thread synchronization object :
          _event_browser_ctrl_->set_stop_requested();
          delete _event_browser_ctrl_;
          _event_browser_ctrl_ = 0;
        }

        // Destroy internal resources :
        _terminate_event_browser();

        // Blank the module with default neutral values :
        _geometry_manager_ = 0;
        _Geo_label_        = "";

        _set_initialized(false);

        DT_LOG_TRACE(get_logging_priority(), "Exiting...");
        return;
      }

      // Processing :
      dpp::base_module::process_status event_browser_module::process(datatools::things & event_record_)
      {
        DT_THROW_IF(! is_initialized(), std::logic_error,
                    "Module '" << get_name() << "' is not initialized !");

        dpp::base_module::process_status status = _show_event(event_record_);
        return status;
      }

      void event_browser_module::_initialize_event_browser()
      {
        // Build detector manager
        detector::detector_manager & detector_mgr = detector::detector_manager::get_instance();
        // Use the geometry service as external geometry manager
        detector_mgr.set_external_geometry_manager(*const_cast<geomtools::manager *>(_geometry_manager_));
        detector_mgr.initialize();
        detector_mgr.construct();

        // Allocate the ROOT application :
        new TApplication("ROOT application", 0, 0);

        // Allocate the event browser :
        // Get the screen dimensions
        int position_x, position_y;
        unsigned int screen_width, screen_height;

        gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
                                 position_x, position_y,
                                 screen_width, screen_height);

        const int height = static_cast<int>(0.75*screen_height);
        const int width  = static_cast<int>(0.75*screen_width);

        _event_browser_ = new event_browser(gClient->GetRoot(), width, height);
        _event_browser_->initialize();

        // Set event server mode to external
        _event_browser_->grab_event_server().set_initialized(true);
        _event_browser_->grab_event_server().set_external(true);

        // Install an event controler in the browser:
        if (_event_browser_ctrl_ == 0) {
          DT_LOG_TRACE(get_logging_priority(), "Allocating the 'event_browser_ctrl' object...");
          _event_browser_ctrl_ = new event_browser_ctrl(*_event_browser_);
          DT_LOG_TRACE(get_logging_priority(), "Install the 'event_browser_ctrl' object "
                       << "in the event browser...");
          _event_browser_->set_thread_ctrl(*_event_browser_ctrl_);
          DT_LOG_TRACE(get_logging_priority(), "New 'event_browser_ctrl' object is allocated.");
        }

        return;
      }

      void event_browser_module::_terminate_event_browser()
      {
        if (_event_browser_ctrl_ != 0) {
          delete _event_browser_ctrl_;
          _event_browser_ctrl_ = 0;
        }

        // ! This is done by ROOT and its memory management
        // if (_event_browser_ !=  0)
        //   {
        //     delete _event_browser_;
        //     _event_browser_ = 0;
        //   }

        return;
      }

      dpp::base_module::process_status event_browser_module::_show_event(io::event_record & event_record_)
      {
        //        static int debug_counter = 0;

        DT_LOG_TRACE(get_logging_priority(), "Pass the event record object "
                     << "to the event server...");
        _event_browser_->grab_event_server().set_external_event(event_record_);

        {
          {
            DT_LOG_TRACE(get_logging_priority(), "Acquire the event control lock...");
            boost::mutex::scoped_lock lock(*_event_browser_ctrl_->event_mutex);

            if (_event_browser_ctrl_->browser_thread == 0) {
              DT_LOG_TRACE(get_logging_priority(), "Starting the ROOT application from its own thread...");
              _event_browser_ctrl_->start();
              DT_LOG_TRACE(get_logging_priority(), "ROOT application thread started.");
              DT_LOG_TRACE(get_logging_priority(), "Now wait for ROOT to run an event...");
            }

            DT_LOG_TRACE(get_logging_priority(),
                         "Notify that event control is now available for the ROOT application thread...");
            _event_browser_ctrl_->event_availability_status = event_browser_ctrl::AVAILABLE_FOR_ROOT;
            _event_browser_ctrl_->event_available_condition->notify_one();
          }

          // Wait for the release of the event control by the ROOT process :
          {
            DT_LOG_TRACE(get_logging_priority(),
                         "Wait for the release of the event control by the ROOT application thread...");
            boost::mutex::scoped_lock lock(*_event_browser_ctrl_->event_mutex);
            while(_event_browser_ctrl_->event_availability_status == event_browser_ctrl::AVAILABLE_FOR_ROOT) {
              _event_browser_ctrl_->event_available_condition->wait(*_event_browser_ctrl_->event_mutex);
            }

            DT_LOG_TRACE(get_logging_priority(), "Ok ! The event control is released "
                         << "by the ROOT application thread...");
          }
        }

        // debug_counter++;
        // if (debug_counter >= 3) return FAILURE;

        return dpp::base_module::PROCESS_SUCCESS;
      }

    } // end if namespace view

  }  // end of namespace visualization

}  // end of namespace snemo

// end of event_browser_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
