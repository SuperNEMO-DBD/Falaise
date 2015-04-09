// snemo/digitization/calo_feb.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_feb.h>

// Third party:
// - Bayeux/datatools: 
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_feb, "snemo::digitalization::calo_feb")

    calo_feb::calo_feb()
    {
      _locked_ = false;
      return;
    }

    calo_feb::~calo_feb()
    {
      reset();
      return;
    }

    void calo_feb::set_header()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter FEB is locked !) ");

      
      return;
    }
    
    void calo_feb::set_data()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter FEB is locked !) ");
      return;
    }

    void calo_feb::set_all_low_thresholds(double & low_threshold_value_)
    {
      for (int i = LOW_THRESHOLD_SAMLONG_0; i <= LOW_THRESHOLD_SAMLONG_7; i++)
	{
	  _thresholds_[i] = low_threshold_value_;
	  std::clog << "DEBUG : low threshold iteration : " << i << std::endl;
	}
      return;
    }

    void calo_feb::set_all_high_thresholds(double & high_threshold_value_)
    {
      for (int i = HIGH_THRESHOLD_SAMLONG_0; i <= HIGH_THRESHOLD_SAMLONG_7; i++)
	{
	  _thresholds_[i] = high_threshold_value_;
	  std::clog << "DEBUG : high threshold iteration : " << i << std::endl;
	}
      return;
    }


    bool calo_feb::is_locked() const
    {
      return _locked_;
    }

    void calo_feb::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter FEB is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void calo_feb::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Calorimeter FEB is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    bool calo_feb::is_valid() const
    {
      return 1;
    }

    void calo_feb::reset()
    {
      if(is_locked())
	{
	  unlock();
	}
      return;
    }

    void calo_feb::tree_dump (std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
    {
      // out_ << indent_ << datatools::i_tree_dumpable::tag
      //      << "Clock tick (25 ns)  : " << _clocktick_25ns_ << std::endl;
      return;
    }
    
    void calo_feb::_check()
    {
    }

  } // end of namespace digitization

} // end of namespace snemo
