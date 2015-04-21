// snemo/digitization/electronic_mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// - Bayeux/datatools:
#include <datatools/exception.h>

// Ourselves
#include <snemo/digitization/electronic_mapping.h>

namespace snemo {
  
  namespace digitization {

    electronic_mapping::electronic_mapping()
    {
      _initialized_ = false;
    }

    electronic_mapping::~electronic_mapping()
    { 
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void electronic_mapping::initialize()
    {
      _initialized_ = true;
      return;
    }

    bool electronic_mapping::is_initialized()
    {
      return _initialized_;
    }
    
    void electronic_mapping::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }

	       
  } // end of namespace digitization

} // end of namespace snemo
