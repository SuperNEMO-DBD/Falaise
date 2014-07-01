/// \file falaise/snemo/datamodels/event_header.cc

// Ourselves:
#include <falaise/snemo/datamodels/event_header.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(event_header, "snemo::datamodel::event_header")

    // static
    const std::string & event_header::event_header_label()
    {
      static const std::string _label("EH");
      return _label;
    }

    const datatools::event_id & event_header::get_id() const
    {
      return _id_;
    }

    datatools::event_id & event_header::grab_id()
    {
      return _id_;
    }

    void event_header::set_id(const datatools::event_id & id_)
    {
      _id_ = id_;
      return;
    }

    const datatools::properties & event_header::get_properties() const
    {
      return _properties_;
    }

    datatools::properties & event_header::grab_properties()
    {
      return _properties_;
    }

    void event_header::set_properties(const datatools::properties & properties_)
    {
      _properties_ = properties_;
      return;
    }

    const snemo::datamodel::timestamp & event_header::get_timestamp() const
    {
      return _timestamp_;
    }

    snemo::datamodel::timestamp & event_header::grab_timestamp()
    {
      return _timestamp_;
    }

    void event_header::set_timestamp(const snemo::datamodel::timestamp & timestamp_)
    {
      _timestamp_ = timestamp_;
      return;
    }

    event_header::generation_type event_header::get_generation() const
    {
      return _generation_;
    }

    void event_header::set_generation(generation_type generation_)
    {
      _generation_ = generation_;
      return;
    }

    bool event_header::is_real() const
    {
      return _generation_ == GENERATION_REAL;
    }

    bool event_header::is_simulated() const
    {
      return _generation_ == GENERATION_SIMULATED;
    }

    event_header::event_header()
    {
      _generation_ = GENERATION_INVALID;
      return;
    }

    event_header::~event_header()
    {
      return;
    }

    void event_header::clear()
    {
      _properties_.clear();
      _timestamp_.invalidate();
      _generation_ = GENERATION_INVALID;
      _id_.clear();
      return;
    }

    void event_header::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      if (! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Id : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _id_.tree_dump(out_, "", indent_oss.str());
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Timestamp : " << _timestamp_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Properties : ";
      if (_properties_.size() == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump(out_, "", indent_oss.str());
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Generation : " << _generation_;
      if (is_simulated()) {
        out_ << ' ' << "[simulated]";
      }
      if (is_real()) {
        out_ << ' ' << "[real]";
      }
      out_ << std::endl;

      return;
    }

    void event_header::dump() const
    {
      tree_dump(std::clog, event_header::SERIAL_TAG);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
