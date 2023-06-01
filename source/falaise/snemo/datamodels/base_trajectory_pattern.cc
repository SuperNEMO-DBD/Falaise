/** \file falaise/snemo/datamodels/base_trajectory_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

#include <utility>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_trajectory_pattern,
                                                      "snemo::datamodel::base_trajectory_pattern")

    base_trajectory_pattern::base_trajectory_pattern(std::string pid)
    : _pattern_id_(std::move(pid))
    {
      return;
    }

    bool base_trajectory_pattern::has_pattern_id() const
    {
      return !_pattern_id_.empty();
    }

    const std::string & base_trajectory_pattern::get_pattern_id() const
    {
      return _pattern_id_;
    }

    void base_trajectory_pattern::print_tree(std::ostream & out,
                                             const boost::property_tree::ptree & options) const
    {
      base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;
      if (!popts.title.empty()) {
        out << indent << popts.title << std::endl;
      }
      out << indent << tag << "Pattern ID = '"
          <<  get_pattern_id() << "'" << std::endl;

      out << indent << inherit_tag(popts.inherit) << "Shape : " << std::endl;
      get_shape().tree_dump(out, "", indent + (popts.inherit ? tags::skip_item() : tags::last_skip_item()));
      return;
    }
  
  } // end of namespace datamodel

} // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
