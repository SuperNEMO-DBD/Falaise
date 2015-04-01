// snemo/digitization/sd_to_calo_signal_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/sd_to_calo_signal_algo.h>

namespace snemo {
  
  namespace digitization {

    sd_to_calo_signal_algo::sd_to_calo_signal_algo()
    {
      _initialized_ = false;
      return;
    }

    sd_to_calo_signal_algo::~sd_to_calo_signal_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void sd_to_calo_signal_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to calo signal algorithm is already initialized ! ");
      
      _initialized_ = true;
      return;
    }
    
    bool sd_to_calo_signal_algo::is_initialized() const
    {
      return _initialized_;
    }

    void sd_to_calo_signal_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is already initialized ! ");
      _initialized_ = false;
      return;
    }

    int sd_to_calo_signal_algo::process(const mctools::simulated_data & sd_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

	_process(sd_);

      }

      catch (std::exception & error) {
	DT_LOG_FATAL(logging, error.what());
	error_code = EXIT_FAILURE;
      }
      
      catch (...) {
	DT_LOG_FATAL(logging, "Unexpected error!");
	error_code = EXIT_FAILURE;
      }

      return error_code;
    }

    int sd_to_calo_signal_algo::_process(const mctools::simulated_data & sd_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

      }

      catch (std::exception & error) {
	DT_LOG_FATAL(logging, error.what());
	error_code = EXIT_FAILURE;
      }

      catch (...) {
	DT_LOG_FATAL(logging, "Unexpected error!");
	error_code = EXIT_FAILURE;
      }

      return error_code;
    }

  } // end of namespace digitization

} // end of namespace snemo
