// falaise/snemo/datamodels/unified_digitized_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/unified_digitized_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(unified_digitized_data,
                                                      "snemo::datamodel::unified_digitized_data")


    unified_digitized_data::unified_digitized_data()
    {
      return;
    }

    unified_digitized_data::~unified_digitized_data()
    {
      reset();
      return;
    }

    const datatools::properties & unified_digitized_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & unified_digitized_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    void unified_digitized_data::reset()
    {
      _auxiliaries_.clear();
      return;
    }

    void unified_digitized_data::clear()
    {
      reset();
      return;
    }

    void unified_digitized_data::tree_dump(std::ostream & out_,
                                           const std::string & title_,
                                           const std::string & indent_,
                                           bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if(! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      // Auxiliary properties:
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Auxiliary properties : ";
        if(_auxiliaries_.size() == 0) {
          out_ << "<empty>";
        }
        out_ << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _auxiliaries_.tree_dump(out_, "", indent_oss.str());
        }
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "The end." << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
