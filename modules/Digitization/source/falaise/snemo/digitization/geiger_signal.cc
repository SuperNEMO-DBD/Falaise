// snemo/digitization/geiger_signal.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_signal.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_signal, "snemo::digitalization::geiger_signal")
    
    geiger_signal::geiger_signal()
    {
      _locked_ = false;;
      return;
    }

    geiger_signal::~geiger_signal()
    {
      reset();
      return;
    }

    bool geiger_signal::is_locked() const
    {
      return _locked_;
    }

    void geiger_signal::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger signal is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void geiger_signal::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Geiger signal is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    bool geiger_signal::is_valid() const
    {
      return true;
    }

    void geiger_signal::reset()
    {
      if(is_locked())
	{
	  unlock();
	}
      geomtools::base_hit::reset();
      return;
    }

    void geiger_signal::tree_dump (std::ostream & out_,
				   const std::string & title_,
				   const std::string & indent_,
				   bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      // out_ << indent_ << datatools::i_tree_dumpable::tag
      //      << "Clock tick (800 ns)  : " << _clocktick_800ns_ << std::endl;

      return;
    }
    
    void geiger_signal::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, " is not valid ! ");
    }


  } // end of namespace digitization

} // end of namespace snemo
