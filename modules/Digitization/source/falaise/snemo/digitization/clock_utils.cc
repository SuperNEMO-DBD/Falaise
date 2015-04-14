// snemo/digitization/clock_utils.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves
#include <snemo/digitization/clock_utils.h>

// - Boost:
#include <boost/cstdint.hpp>

// GSL:
#include <bayeux/mygsl/rng.h>

namespace snemo {
  
  namespace digitization {

    clock_utils::clock_utils()
    {
      _initialized_ = false;
    }

    clock_utils::~clock_utils()
    { 
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void clock_utils::initialize()
    {
      _initialized_ = true;
      return;
    }

    bool clock_utils::is_initialized()
    {
      return _initialized_;
    }
    
    void clock_utils::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }

    int32_t clock_utils::get_clocktick_25_ref()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      return _clocktick_25_ref_;
    }
    int32_t clock_utils::get_clocktick_800_ref()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      return _clocktick_800_ref_;
    }
    double clock_utils::get_clocktick_25_shift()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      return _clocktick_25_shift_;
    }
    double clock_utils::get_clocktick_800_shift()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      return _clocktick_800_shift_;
    }
			
    void clock_utils::compute_clockticks_ref(mygsl::rng & prng_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      _randomize_clockticks_ref(prng_);
      _randomize_clockticks_shift(prng_);
      _clocktick_800_shift_ = _clocktick_25_ref_ % NUMBER_OF_25_CLOCK_IN_800 * MAIN_CLOCKTICK * CLHEP::nanosecond + _clocktick_25_shift_;
      
      return;
    }

    void clock_utils::_randomize_clockticks_ref(mygsl::rng & prng_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");  
      _clocktick_25_ref_ = prng_.uniform_int(INT32_MAX);
      _clocktick_800_ref_ = prng_.uniform_int(INT32_MAX);
      return;
    }

    void clock_utils::_randomize_clockticks_shift(mygsl::rng & prng_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Clock utils is not initialized ! ");
      _clocktick_25_shift_ = prng_.flat(0.0, MAIN_CLOCKTICK);
      return;
    }		
    
	       
  } // end of namespace digitization

} // end of namespace snemo
