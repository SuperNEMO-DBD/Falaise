// Ourselves
#include "FLReconstructUtils.h"

// Third Party
// - Boost
#include "boost/program_options.hpp"
// - Bayeux
#include "bayeux/datatools/logger.h"

namespace FLReconstruct {

  std::string get_data_type_label(data_type dt_)
  {
    if (dt_ == DATA_REAL) return "Real";
    if (dt_ == DATA_MC) return "MC";
    return "";
  }

  data_type get_data_type(const std::string & label_)
  {
    if (label_ == get_data_type_label(DATA_REAL)) return DATA_REAL;
    if (label_ == get_data_type_label(DATA_MC)) return DATA_MC;
    return DATA_UNKNOWN;
  }

} // end of namespace FLReconstruct

// - Validation of verbosity command line arguments. must exist inside
// the datatools namespace.
// TODO : refactor operator>> into datatools, though can't do this
// for validator (bpo dependency not wanted)
namespace datatools {

  std::istream& operator>>(std::istream& in, datatools::logger::priority& p) {
    std::string s;
    in >> s;
    p = datatools::logger::get_priority(s);
    return in;
  }

  //! validate logging argument
  void validate(boost::any& v,
                std::vector<std::string> const& values,
                datatools::logger::priority* /*target_type*/,
                int) {
    namespace bpo = boost::program_options;
    // Make sure no previous assignment to v was made
    bpo::validators::check_first_occurrence(v);

    // Extract first string from values, If there is more than one string
    // it's an error and an exception will be thrown
    std::string const& s = bpo::validators::get_single_string(values);
    datatools::logger::priority p = datatools::logger::get_priority(s);
    if(p != datatools::logger::PRIO_UNDEFINED) {
      v = boost::any(p);
    } else {
      throw bpo::validation_error(bpo::validation_error::invalid_option_value);
    }
  }

}
