// This project:
#include <falaise/snemo/datamodels/raw_event_data.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(raw_event_data,
                                           "snemo::datamodel::raw_event_data")

    bool raw_event_data::is_valid() const
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

    void raw_event_data::invalidate()
    {
      _run_id_ = INVALID_EVENT_ID;
      _event_id_ = INVALID_EVENT_ID;
      _reference_timestamp_.invalidate();
      _calorimeter_digitized_hits_.clear();
      _tracker_digitized_hits_.clear();
      _origin_trigger_ids_.clear();
      return;
    }

    bool raw_event_data::has_run_id() const
    {
      return _run_id_ != INVALID_RUN_ID;
    }

    int32_t raw_event_data::get_run_id() const
    {
      return _run_id_;
    }

    void raw_event_data::set_run_id(const int32_t rid_)
    {
      _run_id_ = rid_;
      return;
    }

    bool raw_event_data::has_event_id() const
    {
      return _event_id_ != INVALID_EVENT_ID;
    }

    int32_t raw_event_data::get_event_id() const
    {
      return _event_id_;
    }

    void raw_event_data::set_event_id(const int32_t tid_)
    {
      _event_id_ = tid_;
      return;
    }

    bool raw_event_data::has_reference_timestamp() const
    {
      return _reference_timestamp_.is_valid();
    }

    const int64_t & raw_event_data::get_reference_timestamp() const
    {
      return _reference_timestamp_;
    }

    void raw_event_data::set_reference_timestamp(const int64_t & ref_timestamp_)
    {
      DT_THROW_IF(not ref_timestamp_.is_clock_40MHz(),
                  std::logic_error,
                  "Invalid event main clock!")
      _reference_timestamp_ = ref_timestamp_;
      return;
    }

    const std::set<int32_t> & raw_event_data::get_origin_trigger_ids() const
    {
      return _origin_trigger_ids_;
    }

    void raw_event_data::add_origin_trigger_id(int32_t tid_)
    {
      _origin_trigger_ids_.insert(tid_);
      return;
    }

    void raw_event_data::reset_origin_trigger_id()
    {
      _origin_trigger_ids_.clear();
      return;
    }

    const std::vector<calorimeter_digitized_hit> & raw_event_data::get_calorimeter_digitized_hits() const
    {
      return _calorimeter_digitized_hits_;
    }

    calorimeter_digitized_hit & raw_event_data::add_calorimeter_hit()
    {
      int32_t newHitId = 0;
      if (_calorimeter_digitized_hits_.size()) {
        newHitId = _calorimeter_digitized_hits_.back().get_hit_id() + 1;
      }
      calorimeter_digitized_hit newHit;
      newHit.set_hit_id(newHitId);
      _calorimeter_digitized_hits_.push_back(newHit);
      return _calorimeter_digitized_hits_.back();
    }

    std::vector<calorimeter_digitized_hit> & raw_event_data::grab_calorimeter_digitized_hits()
    {
      return _calorimeter_digitized_hits_;
    }

    const std::vector<tracker_digitized_hit> & raw_event_data::get_tracker_digitized_hits() const
    {
      return _tracker_digitized_hits_;
    }

    tracker_digitized_hit & raw_event_data::add_tracker_hit()
    {
      int32_t newHitId = 0;
      if (_tracker_digitized_hits_.size()) {
        newHitId = _tracker_digitized_hits_.back().get_hit_id() + 1;
      }
      tracker_digitized_hit newHit;
      newHit.set_hit_id(newHitId);
      _tracker_digitized_hits_.push_back(newHit);
      return _tracker_digitized_hits_.back();
    }

    std::vector<tracker_digitized_hit> & raw_event_data::grab_tracker_digitized_hits()
    {
      return _tracker_digitized_hits_;
    }

    const datatools::properties & raw_event_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & raw_event_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    // virtual
    void raw_event_data::print_tree(std::ostream & out_,
                                    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

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
           << "Calo.Digi.Hits : " << _calorimeter_digitized_hits_.size() << std::endl;
      {
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
          out_ << '\n';
          boost::property_tree::ptree opts2;
          opts2.put("indent", sindent2.str());
          _calorimeter_digitized_hits_[i].print_tree(out_, opts2);
        }
      }

      out_ << popts.indent << tag
           << "Tracker.Digi.Hits : " << _tracker_digitized_hits_.size() << std::endl;
      {
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
          out_ << '\n';
          boost::property_tree::ptree opts2;
          opts2.put("indent", sindent2.str());
          _tracker_digitized_hits_[i].print_tree(out_, opts2);
        }
      }

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
           << "Auxiliaries : " << std::endl;
      {
        boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + "|   ");
        _auxiliaries_.print_tree(out_, auxOpts);
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Valid : " << std::boolalpha << is_valid() << std::endl;

      return;
    }

  } // namespace datamodel

} // namespace snemo
