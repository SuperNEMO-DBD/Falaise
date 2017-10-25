// topology_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/topology_data.h>

namespace snemo {

  namespace datamodel {

    bool topology_data::has_pattern() const
    {
      return _pattern_.has_data();
    }

    void topology_data::set_pattern_handle(const handle_pattern & pattern_handle_)
    {
      _pattern_ = pattern_handle_;
      return;
    }

    void topology_data::detach_pattern()
    {
      _pattern_.reset();
      return;
    }

    topology_data::handle_pattern & topology_data::grab_pattern_handle()
    {
      return _pattern_;
    }

    const topology_data::handle_pattern & topology_data::get_pattern_handle() const
    {
      return _pattern_;
    }

    base_topology_pattern & topology_data::grab_pattern()
    {
      return _pattern_.grab();
    }

    const base_topology_pattern & topology_data::get_pattern() const
    {
      return _pattern_.get();
    }

    datatools::properties & topology_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & topology_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    topology_data::topology_data()
    {
      return;
    }

    topology_data::~topology_data()
    {
      this->reset();
      return;
    }

    void topology_data::reset()
    {
      this->clear();
      return;
    }

    void topology_data::clear()
    {
      detach_pattern();
      _auxiliaries_.clear();
      return;
    }

    void topology_data::tree_dump(std::ostream      & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      if (! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Pattern : " << (has_pattern() ? "Yes" : "No")
           << std::endl;
      if (has_pattern()) {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        get_pattern().tree_dump(out_, "", indent_oss.str());
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : ";
      if (_auxiliaries_.empty()) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
        _auxiliaries_.tree_dump(out_, "", indent_oss.str());
      }

      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_data, "snemo::datamodel::topology_data")

  } // end of namespace model

} // end of namespace snemo
