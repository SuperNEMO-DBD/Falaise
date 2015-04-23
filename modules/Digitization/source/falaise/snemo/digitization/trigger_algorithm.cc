// snemo/digitization/trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_algorithm.h>

namespace snemo {
  
  namespace digitization {


    trigger_algorithm::trigger_algorithm()
    {
      _initialized_ = false;
     _electronic_mapping_ = 0;
      return;
    }

    trigger_algorithm::~trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    bool trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void trigger_algorithm::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized ! ")

      _initialized_ = true;
      return;
    }

    void trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }

    void trigger_algorithm::convert()
    {


      return;
    }
   
    void trigger_algorithm::process()
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
