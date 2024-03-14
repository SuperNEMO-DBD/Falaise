// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/unified_digitized_data.cc

// This project:
#include <falaise/snemo/datamodels/unified_digitized_data.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(unified_digitized_data,
                                           "snemo::datamodel::unified_digitized_data")

    bool unified_digitized_data::is_valid() const
    {
      if (!has_run_id()) {
        return false;
      }
      if (!has_event_id()) {
        return false;
      }
      if (!has_reference_timestamp()) {
        return false;
      }
      if (_calorimeter_digitized_hits_.size() == 0 and _tracker_digitized_hits_.size() == 0) {
        return false;
      }
      return true;
    }

    void unified_digitized_data::invalidate()
    {
      _run_id_ = INVALID_EVENT_ID;
      _event_id_ = INVALID_EVENT_ID;
      _reference_timestamp_  = INVALID_TIME_TICKS;
      _calorimeter_digitized_hits_.clear();
      _tracker_digitized_hits_.clear();
      _origin_trigger_ids_.clear();
      return;
    }

    bool unified_digitized_data::has_run_id() const
    {
      return _run_id_ != INVALID_RUN_ID;
    }

    int32_t unified_digitized_data::get_run_id() const
    {
      return _run_id_;
    }

    void unified_digitized_data::set_run_id(const int32_t rid_)
    {
      _run_id_ = rid_;
      return;
    }

    bool unified_digitized_data::has_event_id() const
    {
      return _event_id_ != INVALID_EVENT_ID;
    }

    int32_t unified_digitized_data::get_event_id() const
    {
      return _event_id_;
    }

    void unified_digitized_data::set_event_id(const int32_t tid_)
    {
      _event_id_ = tid_;
      return;
    }

    bool unified_digitized_data::has_reference_timestamp() const
    {
      if(_reference_timestamp_ == INVALID_TIME_TICKS) return false;
      return true;
    }

    int64_t unified_digitized_data::get_reference_timestamp() const
    {
      return _reference_timestamp_;
    }

    void unified_digitized_data::set_reference_timestamp(int64_t ref_timestamp_)
    {
      _reference_timestamp_ = ref_timestamp_;
      return;
    }

    const std::set<int32_t> & unified_digitized_data::get_origin_trigger_ids() const
    {
      return _origin_trigger_ids_;
    }

    void unified_digitized_data::set_origin_trigger_ids(const std::set<int32_t> & tids_)
    {
      _origin_trigger_ids_ = tids_;
      return;
    }

    void unified_digitized_data::add_origin_trigger_id(int32_t tid_)
    {
      _origin_trigger_ids_.insert(tid_);
      return;
    }

    void unified_digitized_data::reset_origin_trigger_id()
    {
      _origin_trigger_ids_.clear();
      return;
    }

    const CalorimeterDigiHitHdlCollection & unified_digitized_data::get_calorimeter_hits() const
    {
      return _calorimeter_digitized_hits_;
    }

    CalorimeterDigiHitHdlCollection & unified_digitized_data::grab_calorimeter_hits()
    {
      return _calorimeter_digitized_hits_;
    }

    calorimeter_digitized_hit & unified_digitized_data::add_calorimeter_hit()
    {
      int32_t newHitId = 0;
      if (_calorimeter_digitized_hits_.size()) {
        newHitId = _calorimeter_digitized_hits_.back().get().get_hit_id() + 1;
      }
      auto newHit = datatools::make_handle<snemo::datamodel::calorimeter_digitized_hit>();
      newHit->set_hit_id(newHitId);
      _calorimeter_digitized_hits_.push_back(newHit);

      return _calorimeter_digitized_hits_.back().grab();
    }

    const TrackerDigiHitHdlCollection & unified_digitized_data::get_tracker_hits() const
    {
      return _tracker_digitized_hits_;
    }

    TrackerDigiHitHdlCollection & unified_digitized_data::grab_tracker_hits()
    {
      return _tracker_digitized_hits_;
    }

    tracker_digitized_hit & unified_digitized_data::add_tracker_hit()
    {
      int32_t newHitId = 0;
      if (_tracker_digitized_hits_.size()) {
        newHitId = _tracker_digitized_hits_.back().get().get_hit_id() + 1;
      }
      auto newHit = datatools::make_handle<snemo::datamodel::tracker_digitized_hit>();
      newHit->set_hit_id(newHitId);
      _tracker_digitized_hits_.push_back(newHit);

      return _tracker_digitized_hits_.back().grab();
    }

    const datatools::properties & unified_digitized_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & unified_digitized_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    void unified_digitized_data::set_auxiliaries(const datatools::properties & aux_)
    {
      _auxiliaries_ = aux_;
      return;
    }

    // override
    void unified_digitized_data::clear()
    {
      _run_id_ = INVALID_RUN_ID;
      _event_id_ = INVALID_EVENT_ID;
      _calorimeter_digitized_hits_.clear();
      _tracker_digitized_hits_.clear();
      _auxiliaries_.clear();
    }

    // override
    void unified_digitized_data::print_tree(std::ostream & out_,
                                            const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_hits_opt = options_.get("list_hits", false);
      bool hit_details_opt = options_.get("hit_details", false);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Run ID : " << _run_id_ << std::endl;

      out_ << popts.indent << tag
           << "Event ID : " << _event_id_ << std::endl;

      out_ << popts.indent << tag
           << "Reference timestamp : " << _reference_timestamp_ << std::endl;

      out_ << popts.indent << tag
           << "RTD origin trigger IDs : ";
      {
        out_ << '{';
        int countTids = 0;
        for (int32_t tid : _origin_trigger_ids_) {
          out_ << tid;
          if ((countTids + 1) < (int) _origin_trigger_ids_.size()) {
            out_ << ", ";
          }
          countTids++;
        }
        out_ << '}' << std::endl;
      }

      out_ << popts.indent << tag
           << "Valid : " << std::boolalpha << is_valid() << std::endl;

      out_ << popts.indent << tag
           << "Calo.Digi.Hits : " << _calorimeter_digitized_hits_.size() << std::endl;
      if (list_hits_opt) {
        for (int i = 0; i < (int) _calorimeter_digitized_hits_.size(); i++) {
          out_ << popts.indent << skip_tag;
          std::ostringstream sindent2;
          sindent2 << popts.indent << skip_tag;
          if ((i + 1) == (int) _calorimeter_digitized_hits_.size()) {
            out_ << last_tag;
            sindent2 << last_skip_tag;
          } else {
            out_ << tag;
            sindent2 << skip_tag;
          }
          out_ << "Calo. Hit #" << i << " : ";
 	  if (not hit_details_opt) {
	    out_ << _calorimeter_digitized_hits_[i].get().get_geom_id();
	  }
	  out_ << '\n';
	  if (hit_details_opt) {
	    boost::property_tree::ptree opts2;
	    opts2.put("indent", sindent2.str());
	    _calorimeter_digitized_hits_[i].get().print_tree(out_, opts2);
	  }
        }
      }

      out_ << popts.indent << tag
           << "Tracker.Digi.Hits : " << _tracker_digitized_hits_.size() << std::endl;
      if (list_hits_opt) {
        for (int i = 0; i < (int) _tracker_digitized_hits_.size(); i++) {
          out_ << popts.indent << skip_tag;
          std::ostringstream sindent2;
          sindent2 << popts.indent << skip_tag;
          if ((i + 1) == (int) _tracker_digitized_hits_.size()) {
            out_ << last_tag;
            sindent2 << last_skip_tag;
          } else {
            out_ << tag;
            sindent2 << skip_tag;
          }
          out_ << "Tracker. Hit #" << i << " : ";
	  if (not hit_details_opt) {
	    out_ << _tracker_digitized_hits_[i].get().get_geom_id();
	  }
          out_ << '\n';
	  if (hit_details_opt) {
	    boost::property_tree::ptree opts2;
	    opts2.put("indent", sindent2.str());
	    _tracker_digitized_hits_[i].get().print_tree(out_, opts2);
	  }
        }
      }
    
      out_ << popts.indent << inherit_tag(popts.inherit) << "Auxiliary properties : " << _auxiliaries_.size() << std::endl;
      if (list_properties_opt) {
        boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + tags::item(not popts.inherit, true));
        _auxiliaries_.print_tree(out_, auxOpts);
      }
 
      return;
    }

  } // namespace datamodel

} // namespace snemo
