// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/tracker_digitized_hit.cc
 */

// This project:
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {
  namespace datamodel {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(tracker_digitized_hit,
                                           "snemo::datamodel::tracker_digitized_hit")

    /*** tracker_digitized_hit::rtd_origin ***/

    tracker_digitized_hit::rtd_origin::rtd_origin(int32_t hit_number_,
                                                  int32_t trigger_id_)
    : _hit_number_(hit_number_)
      , _trigger_id_(trigger_id_)
    {
      return;
    }

    int32_t tracker_digitized_hit::rtd_origin::get_hit_number() const
    {
      return _hit_number_;
    }

    int32_t tracker_digitized_hit::rtd_origin::get_trigger_id() const
    {
      return _trigger_id_;
    }

    bool tracker_digitized_hit::rtd_origin::is_valid() const
    {
      if (_hit_number_ < 0) return false;
      if (_trigger_id_ < 0) return false;
      return true;
    }

    void tracker_digitized_hit::rtd_origin::invalidate()
    {
      _hit_number_ = INVALID_HIT_ID;
      _trigger_id_ = INVALID_TRIGGER_ID;
      return;
    }

    // virtual
    void tracker_digitized_hit::rtd_origin::print_tree(std::ostream & out_,
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

    /*** tracker_digitized_hit::gg_times ***/

    void tracker_digitized_hit::gg_times::invalidate()
    {
      _anode_times_[ANODE_R0] = INVALID_TIME_TICKS();
      _anode_times_[ANODE_R1] = INVALID_TIME_TICKS();
      _anode_times_[ANODE_R2] = INVALID_TIME_TICKS();
      _anode_times_[ANODE_R3] = INVALID_TIME_TICKS();
      _anode_times_[ANODE_R4] = INVALID_TIME_TICKS();
      _bottom_cathode_time_ = INVALID_TIME_TICKS();
      _top_cathode_time_ = INVALID_TIME_TICKS();

      _anode_origins_[ANODE_R0].invalidate();
      _anode_origins_[ANODE_R1].invalidate();
      _anode_origins_[ANODE_R2].invalidate();
      _anode_origins_[ANODE_R3].invalidate();
      _anode_origins_[ANODE_R4].invalidate();
      _bottom_cathode_origin_.invalidate();
      _top_cathode_origin_.invalidate();
    }

    bool tracker_digitized_hit::gg_times::has_anode_origin(uint16_t rank_) const
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      return _anode_origins_[rank_].is_valid();
    }

    const tracker_digitized_hit::rtd_origin &
    tracker_digitized_hit::gg_times::get_anode_origin(uint16_t rank_) const
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      return _anode_origins_[rank_];
    }

    void tracker_digitized_hit::gg_times::set_anode_origin(uint16_t rank_, const rtd_origin & anode_orig_)
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      _anode_origins_[rank_] = anode_orig_;
      return;
    }

    bool tracker_digitized_hit::gg_times::has_anode_time(uint16_t rank_) const
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      return _anode_times_[rank_].is_valid();
    }

    void tracker_digitized_hit::gg_times::set_anode_time(uint16_t rank_, const int64_t & ts_)
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      _anode_times_[rank_] = ts_;
      return;
    }

    const int64_t & tracker_digitized_hit::gg_times::get_anode_time(uint16_t rank_) const
    {
      DT_THROW_IF(rank_ >= 5, std::range_error, "Invalid anode time rank!");
      return _anode_times_[rank_];
    }

    bool tracker_digitized_hit::gg_times::has_bottom_cathode_origin() const
    {
      return _bottom_cathode_origin_.is_valid();
    }

    const tracker_digitized_hit::rtd_origin &
    tracker_digitized_hit::gg_times::get_bottom_cathode_origin() const
    {
      return _bottom_cathode_origin_;
    }

    void tracker_digitized_hit::gg_times::set_bottom_cathode_origin(const rtd_origin & bottom_cathode_orig_)
    {
      _bottom_cathode_origin_ = bottom_cathode_orig_;
      return;
    }

    bool tracker_digitized_hit::gg_times::has_bottom_cathode_time() const
    {
      return _bottom_cathode_time_.is_valid();
    }

    void tracker_digitized_hit::gg_times::set_bottom_cathode_time(const int64_t & ts_)
    {
      _bottom_cathode_time_ = ts_;
      return;
    }

    const int64_t & tracker_digitized_hit::gg_times::get_bottom_cathode_time() const
    {
      return _bottom_cathode_time_;
    }

    bool tracker_digitized_hit::gg_times::has_top_cathode_origin() const
    {
      return _top_cathode_origin_.is_valid();
    }

    const tracker_digitized_hit::rtd_origin &
    tracker_digitized_hit::gg_times::get_top_cathode_origin() const
    {
      return _top_cathode_origin_;
    }

    void tracker_digitized_hit::gg_times::set_top_cathode_origin(const rtd_origin & top_cathode_orig_)
    {
      _top_cathode_origin_ = top_cathode_orig_;
      return;
    }

    bool tracker_digitized_hit::gg_times::has_top_cathode_time() const
    {
      return _top_cathode_time_.is_valid();
    }

    void tracker_digitized_hit::gg_times::set_top_cathode_time(const int64_t & ts_)
    {
      _top_cathode_time_ = ts_;
      return;
    }

    const int64_t & tracker_digitized_hit::gg_times::get_top_cathode_time() const
    {
      return _top_cathode_time_;
    }

    void tracker_digitized_hit::gg_times::print_tree(std::ostream & out_,
                                                     const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "RTD anode origins : " << std::endl;
      for (int i = 0; i < 5; i++) {
        const rtd_origin & orig = _anode_origins_[i];
        out_ << popts.indent << skip_tag;
        if (i == 4) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "R" << i << " : " << (orig.is_valid() ? "" : "undefined") << std::endl;
        if (orig.is_valid()) {
          boost::property_tree::ptree opts2;
          std::ostringstream indent2;
          indent2 << popts.indent << skip_tag;
          if (i == 4) {
            indent2 << last_skip_tag;
          } else {
            indent2 << skip_tag;
          }
          opts2.put("indent", indent2.str());
          orig.print_tree(out_, opts2);
        }
      }

      out_ << popts.indent << tag
           << "Anode times : " << std::endl;
      for (int i = 0; i < 5; i++) {
        const int64_t & ts = _anode_times_[i];
        out_ << popts.indent << skip_tag;
        if (i == 4) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "R" << i << " : ";
        if (ts.is_valid()) {
          out_ << ts;
        } else  {
          out_ << "undefined";
        }
        out_ << std::endl;
      }

      out_ << popts.indent << tag
           << "RTD bottom cathode origin : " << (_bottom_cathode_origin_.is_valid() ? "" : "undefined") << std::endl;
      if (_bottom_cathode_origin_.is_valid()) {
        boost::property_tree::ptree opts2;
        opts2.put("indent", popts.indent + "|   ");
        _bottom_cathode_origin_.print_tree(out_, opts2);
      }

      out_ << popts.indent << tag
           << "Bottom cathode time : ";
      if (_bottom_cathode_time_.is_valid()) {
        out_ << _bottom_cathode_time_;
      } else  {
        out_ << "undefined";
      }
      out_ << std::endl;

      out_ << popts.indent << tag
           << "RTD top cathode origin : " << (_top_cathode_origin_.is_valid() ? "" : "undefined") << std::endl;
      if (_top_cathode_origin_.is_valid()) {
        boost::property_tree::ptree opts2;
        opts2.put("indent", popts.indent + "|   ");
        _top_cathode_origin_.print_tree(out_, opts2);
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Top cathode time : ";
      if (_top_cathode_time_.is_valid()) {
        out_ << _top_cathode_time_;
      } else  {
        out_ << "undefined";
      }
      out_ << std::endl;

      return;
    }

    /*** tracker_digitized_hit ***/
    //
    // bool tracker_digitized_hit::has_cell_id() const
    // {
    //   return _cell_id_.is_valid();
    // }
    //
    // void tracker_digitized_hit::set_cell_id(const sncabling::gg_cell_id & id_)
    // {
    //   _cell_id_ = id_;
    //   return;
    // }
    //
    // void tracker_digitized_hit::reset_cell_id()
    // {
    //   _cell_id_.invalidate();
    //   return;
    // }
    //
    // const sncabling::gg_cell_id & tracker_digitized_hit::get_cell_id() const
    // {
    //   return _cell_id_;
    // }

    const std::vector<tracker_digitized_hit::gg_times> & tracker_digitized_hit::get_times() const
    {
      return _times_;
    }

    std::vector<tracker_digitized_hit::gg_times> & tracker_digitized_hit::grab_times()
    {
      return _times_;
    }

    tracker_digitized_hit::gg_times & tracker_digitized_hit::add_times()
    {
      gg_times new_gg_times;
      new_gg_times.invalidate();
      _times_.push_back(new_gg_times);
      return _times_.back();
    }

    bool tracker_digitized_hit::is_valid() const
    {
      if (not this->geomtools::base_hit::is_valid()) return true;
      if (_times_.size() == 0) return false;
      return true;
    }

    void tracker_digitized_hit::invalidate()
    {
      this->geomtools::base_hit::invalidate();
      _cell_id_.invalidate();
      _times_.clear();
      return;
    }

    // virtual
    void tracker_digitized_hit::print_tree(std::ostream & out_,
                                           const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      this->geomtools::base_hit::print_tree(out_,
                                            base_print_options::force_inheritance(options_));

      out_ << popts.indent << tag
           << "Times : " << _times_.size() << std::endl;
      {
        for (int i = 0; i < (int) _times_.size(); i++) {
          out_ << popts.indent << skip_tag;
          std::ostringstream sindent2;
          sindent2 << popts.indent << skip_tag;
          if ((i + 1) == (int) _times_.size()) {
            out_ << last_tag;
            sindent2 << last_skip_tag;
          } else {
            out_ << tag;
            sindent2 << skip_tag;
          }
          out_ << "Times #" << i << " : ";
          out_ << '\n';
          boost::property_tree::ptree opts2;
          opts2.put("indent", sindent2.str());
          _times_[i].print_tree(out_, opts2);
        }
      }

      out_ << popts.indent << tag
           << "Cell ID : [" << _cell_id_.to_label().to_string() << ']' << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Valid : " << std::boolalpha << is_valid() << std::endl;

      return;
    }

  } // namespace datamodel

} // namespace snemo
