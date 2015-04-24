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

    const int      mapping::INVALID_MODULE_NUMBER;
    const int      mapping::DEMONSTRATOR_MODULE_NUMBER;
    const int32_t  mapping::GEIGER_CATEGORY_TYPE;
    const int32_t  mapping::CALO_MAIN_WALL_CATEGORY_TYPE;
    const int32_t  mapping::CALORIMETER_X_WALL_CATEGORY_TYPE;
    const int32_t  mapping::CALORIMETER_GVETO_CATEGORY_TYPE;
    const int32_t  mapping::FEB_CATEGORY_TYPE;
    const bool     mapping::THREE_WIRES_TRACKER_MODE;
    const bool     mapping::TWO_WIRES_TRACKER_MODE;
    const bool     mapping::SIDE_MODE;
    const int32_t  mapping::NUMBER_OF_CONNECTED_ROWS;
    const int32_t  mapping::TRACKER_CONTROL_BOARD_TYPE;
    const int32_t  mapping::CALORIMETER_CONTROL_BOARD_TYPE;
    const int32_t  mapping::CALO_RACK_ID;
    const int32_t  mapping::TRIGGER_RACK_ID;
    const int32_t  mapping::GEIGER_RACK_ID;
    const uint32_t mapping::MAX_NUMBER_OF_SIDE;
    const uint32_t mapping::MAX_NUMBER_OF_WALLS;
    const uint32_t mapping::MAX_NUMBER_OF_LAYER;
    const uint32_t mapping::MAX_NUMBER_OF_MAIN_CALO_COLUMN;
    const uint32_t mapping::MAX_NUMBER_OF_MAIN_CALO_ROW;
    const uint32_t mapping::MAX_NUMBER_OF_X_CALO_COLUMNS;
    const uint32_t mapping::MAX_NUMBER_OF_X_CALO_ROWS;
    const uint32_t mapping::MAX_NUMBER_OF_GVETO_COLUMNS;
    const uint32_t mapping::MAX_NUMBER_OF_GEIGER_ROW;
    const uint32_t mapping::MAX_NUMBER_OF_CRATE;
    const uint32_t mapping::MAX_NUMBER_OF_FEB_BY_CRATE;
    const int32_t  mapping::XWALL_GVETO_CRATE_ID;
    const int32_t  mapping::TRIGGER_CRATE_ID;
    const int32_t  mapping::CONTROL_BOARD_ID;
    const int32_t  mapping::TRIGGER_BOARD_ID ;
    const int32_t  mapping::BOARD_ID_SHIFT_CRATE_0_LIMIT;
    const int32_t  mapping::BOARD_ID_SHIFT_CRATE_2_LIMIT;
    const int32_t  mapping::NO_FEB_NUMBER_10_SHIFT;
    const int32_t  mapping::THREE_WIRES_CRATE_0_LIMIT;
    const int32_t  mapping::THREE_WIRES_CRATE_1_BEGINNING;
    const int32_t  mapping::THREE_WIRES_LONELY_ROW;
    const int32_t  mapping::THREE_WIRES_CRATE_1_LIMIT;
    const int32_t  mapping::THREE_WIRES_CRATE_2_BEGINNING;
    const int32_t  mapping::GEIGER_LAYER_SIZE;
    const int32_t  mapping::GEIGER_ROW_SIZE;

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
