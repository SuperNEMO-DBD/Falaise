// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/trigger_digitized_hit.cc
 */

// This project:
#include <falaise/snemo/datamodels/trigger_digitized_hit.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {
  namespace datamodel {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(trigger_digitized_hit,
                                           "snemo::datamodel::trigger_digitized_hit")

    /*** trigger_digitized_hit::rtd_origin ***/

    trigger_digitized_hit::rtd_origin::rtd_origin(int32_t hit_number_,
                                                  int32_t trigger_id_)
    : _hit_number_(hit_number_)
      , _trigger_id_(trigger_id_)
    {
      return;
    }

    int32_t trigger_digitized_hit::rtd_origin::get_hit_number() const
    {
      return _hit_number_;
    }

    int32_t trigger_digitized_hit::rtd_origin::get_trigger_id() const
    {
      return _trigger_id_;
    }

    bool trigger_digitized_hit::rtd_origin::is_valid() const
    {
      if (_hit_number_ < 0) return false;
      if (_trigger_id_ < 0) return false;
      return true;
    }

    void trigger_digitized_hit::rtd_origin::invalidate()
    {
      _hit_number_ = INVALID_HIT_ID;
      _trigger_id_ = INVALID_TRIGGER_ID;
      return;
    }

    // virtual
    void trigger_digitized_hit::rtd_origin::print_tree(std::ostream & out_,
                                                       const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Hit number : " << _hit_number_ << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Trigger ID : " << _trigger_id_ << std::endl;

      return;
    }


    bool trigger_digitized_hit::is_valid() const
    {
      return true;
    }

    void trigger_digitized_hit::invalidate()
    {
      return;
    }

    // virtual
    void trigger_digitized_hit::print_tree(std::ostream & out_,
                                           const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Valid : " << std::boolalpha << is_valid() << std::endl;

      return;
    }

  } // namespace datamodel

} // namespace snemo
