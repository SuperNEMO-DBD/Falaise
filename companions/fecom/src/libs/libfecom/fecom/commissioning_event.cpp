// fecom/commissioning_event.cpp

// Ourselves:
#include <fecom/commissioning_event.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(commissioning_event, "fecom::commissioning_event")

  commissioning_event::commissioning_event()
  {
    reset();
  }

  commissioning_event::~commissioning_event()
  {
  }

  bool commissioning_event::is_valid() const
  {
    return _trigger_id_ != 0xFFFFFFFF;
  }

  void commissioning_event::set_channel_mapping(const fecom::channel_mapping & mapping_)
  {
    _my_channel_mapping_ = & mapping_;
    return;
  }

  const fecom::channel_mapping & commissioning_event::get_channel_mapping() const
  {
    return * _my_channel_mapping_;
  }

  void commissioning_event::add_calo_hit(fecom::calo_hit & a_calo_hit_)
  {
    _calo_hit_collection_.insert(a_calo_hit_);
    return;
  }

  void commissioning_event::add_tracker_channel_hit(fecom::tracker_channel_hit & a_tracker_channel_hit_)
  {
    _tracker_channel_hit_collection_.insert(a_tracker_channel_hit_);
    return;
  }

  uint32_t commissioning_event::get_trigger_id() const
  {
    return _trigger_id_;
  }

  void commissioning_event::set_trigger_id(const uint32_t trigger_id_)
  {
    _trigger_id_ = trigger_id_;
    return;
  }

  const commissioning_event::calo_hit_collection & commissioning_event::get_calo_hit_collection() const
  {
    return _calo_hit_collection_;
  }

  const commissioning_event::tracker_channel_hit_collection & commissioning_event::get_tracker_channel_hit_collection() const
  {
    return _tracker_channel_hit_collection_;
  }

  const commissioning_event::tracker_hit_collection & commissioning_event::get_tracker_hit_collection() const
  {
    return _tracker_hit_collection_;
  }

  void commissioning_event::build_tracker_hit_from_channels()
  {
    bool verbose = false;

    if (verbose) std::clog << "Number of tracker channel in the event : " << _tracker_channel_hit_collection_.size() << std::endl;

    for (std::set<fecom::tracker_channel_hit>::iterator ichan = _tracker_channel_hit_collection_.begin();
	 ichan != _tracker_channel_hit_collection_.end();
	 ichan++)
      {
	if (ichan -> associated) {
	  // Channel already associated to a tracker hit.
	  // Skip it and go to the next channel hit
	} else {
	  // Create a new tracker hit and search his associated channels
	  fecom::tracker_hit a_tracker_hit;
	  const uint16_t feast_id = ichan -> feast_id;
	  const uint16_t channel  = ichan -> channel;
	  uint16_t cell_id = -1;
	  _my_channel_mapping_-> get_cell_number_for_a_channel(feast_id,
							       channel,
							       cell_id);
	  a_tracker_hit.set_cell_id(cell_id);
	  const fecom::tracker_channel_hit::channelmode_type channel_type = ichan -> channel_type;
	  std::string channel_type_str = "INVALID";
	  if (channel_type == fecom::tracker_channel_hit::ANODIC_CHANNEL) channel_type_str = "Anodic";
	  else if (channel_type == fecom::tracker_channel_hit::CATHODIC_CHANNEL) channel_type_str = "Cathodic";

	  const std::string timestamp = ichan -> timestamp_type;


	  // Only seems valid because it is impossible to distinguish top cathode and bottom cathode (due to input data file of Jihanne)
	  bool seems_valid = false;
	  if (channel_type == fecom::tracker_channel_hit::ANODIC_CHANNEL
	      && _my_channel_mapping_-> is_anodic_channel(feast_id, channel)) seems_valid = true;

	  else if (channel_type == fecom::tracker_channel_hit::CATHODIC_CHANNEL
		   && (_my_channel_mapping_-> is_bottom_cathodic_channel(feast_id, channel)
		       || _my_channel_mapping_-> is_top_cathodic_channel(feast_id, channel))) seems_valid = true;

	  // Check if the input channel is coherent with the mapping, if not Fatal Error
	  DT_THROW_IF(!seems_valid,
		      std::logic_error,
		      "The input tracker channel : Feast #" + std::to_string(feast_id) + " Channel #" + std::to_string(channel) + " Channel type '" + channel_type_str + "' is not coherent with the mapping ! Check if the mapping (in the csv file) is correct !");

	  // Search all channels in the tracker channel hit collection
	  uint16_t associated_anodic_feast = -1;
	  uint16_t associated_anodic_channel = -1;
	  uint16_t associated_bottom_cathodic_feast = -1;
	  uint16_t associated_bottom_cathodic_channel = -1;
	  uint16_t associated_top_cathodic_feast = -1;
	  uint16_t associated_top_cathodic_channel = -1;

	  _my_channel_mapping_ -> get_associated_channels_with_types(feast_id,
								     channel,
								     associated_anodic_feast,
								     associated_anodic_channel,
								     associated_bottom_cathodic_feast,
								     associated_bottom_cathodic_channel,
								     associated_top_cathodic_feast,
								     associated_top_cathodic_channel);

	  // For the anodic channel, search the 5 timestamps :
	  for (std::size_t itime = 0; itime < 5; itime++)
	    {
	      std::string search_timestamp = "t"+ std::to_string(itime);
	      auto it_set = std::find_if(_tracker_channel_hit_collection_.begin(),
					 _tracker_channel_hit_collection_.end(),
					 fecom::tracker_channel_hit::find_by_timestamp(associated_anodic_feast,
										       associated_anodic_channel,
										       search_timestamp));
	      if (it_set != _tracker_channel_hit_collection_.end() && !it_set -> associated) {
		it_set -> associated = true;
		a_tracker_hit.add_tracker_channel(*it_set);
	      }
	    }

	  // For the bottom cathodic channel, search the only timestamp :
	  auto it_set_bot_cat = std::find_if(_tracker_channel_hit_collection_.begin(),
					     _tracker_channel_hit_collection_.end(),
					     fecom::tracker_channel_hit::find_by_channel(associated_bottom_cathodic_feast,
											 associated_bottom_cathodic_channel));

	  if (it_set_bot_cat != _tracker_channel_hit_collection_.end() && !it_set_bot_cat -> associated) {
	    it_set_bot_cat -> associated = true;
	    a_tracker_hit.add_tracker_channel(*it_set_bot_cat);
	  }

	  // For the top cathodic channel, search the only timestamp :
	  auto it_set_top_cat = std::find_if(_tracker_channel_hit_collection_.begin(),
					     _tracker_channel_hit_collection_.end(),
					     fecom::tracker_channel_hit::find_by_channel(associated_top_cathodic_feast,
											 associated_top_cathodic_channel));

	  if (it_set_top_cat != _tracker_channel_hit_collection_.end() && !it_set_top_cat -> associated) {
	    it_set_top_cat -> associated = true;
	    a_tracker_hit.add_tracker_channel(*it_set_top_cat);
	  }

	  // Add the tracker hit to the collection :
	  _tracker_hit_collection_.push_back(a_tracker_hit);

	} // end of not already associated

      } // end of ichan

    return;
  }

  void commissioning_event::reset()
  {
    _reset_();
    return;
  }

  void commissioning_event::_reset_()
  {
    _my_channel_mapping_ = nullptr;
    _trigger_id_ = 0xFFFFFFFF;
    _calo_hit_collection_.clear();
    _tracker_channel_hit_collection_.clear();
    _tracker_hit_collection_.clear();
    return;
  }

  void commissioning_event::tree_dump(std::ostream & out_,
				      const std::string & title_,
				      const std::string & indent_,
				      bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
	 << "Trigger ID : " << _trigger_id_ << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
	 << "Calo hit collection size : " << _calo_hit_collection_.size() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
	 << "Tracker channel hit collection size : " << _tracker_channel_hit_collection_.size() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
	 << "Tracker hit collection size : " << _tracker_hit_collection_.size() << std::endl;

    return;
  }

  bool commissioning_event::compare::operator()(const commissioning_event & a,
						const commissioning_event & b)
  {
    if (a._trigger_id_ < b._trigger_id_) {
      return true;
    }
    else if ((a._trigger_id_ == b._trigger_id_)){
      DT_THROW(std::logic_error, "Two commissioning events have the same trigger_id, trigger_id_a = '" << a._trigger_id_ << "', trigger_id_b = '" << b._trigger_id_ << "' !");
      return true;
    }

    else {
      return false;
    }
  }

} // namespace fecom
