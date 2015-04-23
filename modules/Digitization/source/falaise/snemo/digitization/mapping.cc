// snemo/digitization/mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <string>

// Boost :
#include <boost/scoped_ptr.hpp>

// Ourselves:
#include <falaise/snemo/digitization/mapping.h>

namespace snemo {

  namespace digitization {

    const std::string & mapping::geiger_type()
    {
      static boost::scoped_ptr<std::string> _label;
      if (!_label) {
	_label.reset(new std::string("geiger"));
      }
      return *_label;
    }
    const std::string & mapping::main_calo_type()
    {
      static boost::scoped_ptr<std::string> _label;
      if (!_label) {
	_label.reset(new std::string("mcalo"));
      }
      return *_label;
    }
    const std::string & mapping::x_wall_type()
    {
      static boost::scoped_ptr<std::string> _label;
      if (!_label) {
	_label.reset(new std::string("xwall"));
      }
      return *_label;
    }
    const std::string & mapping::gveto_type()
    {
    static boost::scoped_ptr<std::string> _label;
      if (!_label) {
	_label.reset(new std::string("gveto"));
      }
      return *_label;
    }

  } // end of namespace digitization

} // end of namespace snemo
