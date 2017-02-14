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
    bool verbose = true;

    if (verbose) std::clog << "Number of tracker channel in the event : " << _tracker_channel_hit_collection_.size() << std::endl;

    for (std::set<fecom::tracker_channel_hit>::iterator ichan = _tracker_channel_hit_collection_.begin();
	 ichan != _tracker_channel_hit_collection_.end();
	 ichan++)
      {
	if (ichan -> associated) {
	  // Channel already associated to a tracker hit.
	  // Skip it and go to the next channel hit
	  if (verbose)  std::clog << "DEBUG : COMMISSIONING_EVENT.CPP : CHANNEL ALREADY ASSOCIATED" << std::endl;
	} else {
	  // Create a new tracker hit and search his associated channels
	  fecom::tracker_hit a_tracker_hit;

	  const uint16_t feast_id = ichan -> feast_id;
	  const uint16_t channel  = ichan -> channel;
	  const std::string timestamp = ichan -> timestamp_type;
	  // Add channel into the tracker hit
	  a_tracker_hit.add_tracker_channel(*ichan);
	  ichan -> associated = true;

	  // Search for other channels which build a 'full' tracker hit

	  ichan->tree_dump(std::clog, "INPUT CHANNEL");

	  // if channel is anodic, check if other timestamp of this channel already exist and search associated cathodic :
	  if (_my_channel_mapping_->is_anodic_channel(feast_id, channel))
	    {
	      std::clog <<  "DEBUG : COMMISSIONING_EVENT.CPP : IS ANODIC CHANNEL" << std::endl;
	      for (std::size_t itime = 0; itime < 5; itime++)
		{
		  std::string search_timestamp = "t"+ std::to_string(itime);
		  auto it_set = std::find_if(_tracker_channel_hit_collection_.begin(),
					     _tracker_channel_hit_collection_.end(),
					     fecom::tracker_channel_hit::find_by_timestamp(feast_id,
											   channel,
											   search_timestamp));
		  if (it_set != _tracker_channel_hit_collection_.end() && !it_set -> associated) {
		    it_set -> tree_dump(std::clog, "IS ANODIC AND FINDED TIMESTAMP IT SET :");
		    a_tracker_hit.add_tracker_channel(*it_set);
		    it_set -> associated = true;
		  }
		}

	      // Search the associated cathodic channel :
	      fecom::tracker_channel_hit associated_tchan_1;
	      fecom::tracker_channel_hit associated_tchan_2;

	      // MAPPING TO BE DONE : FEAST IN = FEAST OUT
	      // uint16_t associated_feast_id_1 = -1;
	      uint16_t associated_feast_id_1 = feast_id;
	      // uint16_t associated_feast_id_2 = -1;
	      uint16_t associated_feast_id_2 = feast_id;
	      uint16_t associated_channel_1 = -1;
	      uint16_t associated_channel_2 = -1;

	      _my_channel_mapping_->get_associated_channels(feast_id,
							    channel,
							    associated_feast_id_1,
							    associated_channel_1,
							    associated_feast_id_2,
							    associated_channel_2);

	      std::clog << "Is anodic : Feast in : " << feast_id
			<< " Channel in : " << channel
			<< " Feast out 1 : " << associated_feast_id_1
			<< " Channel out 1 : " << associated_channel_1
			<< " Feast out 2 : " << associated_feast_id_2
			<< " Channel out 2 : " << associated_channel_2 << std::endl;


	      auto it_set_1 = std::find_if(_tracker_channel_hit_collection_.begin(),
					   _tracker_channel_hit_collection_.end(),
					   fecom::tracker_channel_hit::find_by_channel(associated_feast_id_1, associated_channel_1));


	      auto it_set_2 = std::find_if(_tracker_channel_hit_collection_.begin(),
					   _tracker_channel_hit_collection_.end(),
					   fecom::tracker_channel_hit::find_by_channel(associated_feast_id_2, associated_channel_2));

	      if (it_set_1 != _tracker_channel_hit_collection_.end() && !it_set_1 -> associated) {
		a_tracker_hit.add_tracker_channel(*it_set_1);
		it_set_1 -> associated = true;
	      }
	      if (it_set_2 != _tracker_channel_hit_collection_.end() && !it_set_2 -> associated) {
		a_tracker_hit.add_tracker_channel(*it_set_2);
		it_set_2 -> associated = true;
	      }
	    } // end of if anodic

	  else if (_my_channel_mapping_->is_bottom_cathodic_channel(feast_id, channel)
		   || _my_channel_mapping_->is_top_cathodic_channel(feast_id, channel))
	    {
	      std::clog <<  "DEBUG : COMMISSIONING_EVENT.CPP : IS CATHODIC CHANNEL" << std::endl;
	      // If input = cathodic, search the other cathodic and all anodic associated channels :

	      // Search the associated cathodic channel :
	      fecom::tracker_channel_hit associated_tchan_1;
	      fecom::tracker_channel_hit associated_tchan_2;

	      // MAPPING TO BE DONE : FEAST IN = FEAST OUT
	      // uint16_t associated_feast_id_1 = -1;
	      uint16_t associated_feast_id_1 = feast_id;
	      // uint16_t associated_feast_id_2 = -1;
	      uint16_t associated_feast_id_2 = feast_id;
	      uint16_t associated_channel_1 = -1;
	      uint16_t associated_channel_2 = -1;

	      _my_channel_mapping_->get_associated_channels(feast_id,
							    channel,
							    associated_feast_id_1,
							    associated_channel_1,
							    associated_feast_id_2,
							    associated_channel_2);

	      std::clog << "Is cathodic : Feast in : " << feast_id
			<< " Channel in : " << channel
			<< " Feast out 1 : " << associated_feast_id_1
			<< " Channel out 1 : " << associated_channel_1
			<< " Feast out 2 : " << associated_feast_id_2
			<< " Channel out 2 : " << associated_channel_2 << std::endl;

	      	      auto it_set_1 = std::find_if(_tracker_channel_hit_collection_.begin(),
					   _tracker_channel_hit_collection_.end(),
					   fecom::tracker_channel_hit::find_by_channel(associated_feast_id_1, associated_channel_1));

	      // Channel 1 will be the anodic, channel 2 will be the other cathodic :
	      if (it_set_1 != _tracker_channel_hit_collection_.end() && !it_set_1 -> associated) {
		const std::string associated_timestamp = it_set_1 -> timestamp_type;
		for (std::size_t itime = 0; itime < 5; itime++)
		  {
		    std::string search_timestamp = "t"+ std::to_string(itime);
		    auto it_set_anodic = std::find_if(_tracker_channel_hit_collection_.begin(),
						      _tracker_channel_hit_collection_.end(),
						      fecom::tracker_channel_hit::find_by_timestamp(associated_feast_id_1,
												    associated_channel_1,
												    search_timestamp));
		    if (it_set_anodic != _tracker_channel_hit_collection_.end() && !it_set_anodic -> associated) {
		      a_tracker_hit.add_tracker_channel(*it_set_anodic);
		      it_set_anodic -> associated = true;
		    }
		  }

		a_tracker_hit.add_tracker_channel(*it_set_1);
		it_set_1 -> associated = true;
	      }
	      // Channel 2 : cathodic

	      auto it_set_2 = std::find_if(_tracker_channel_hit_collection_.begin(),
					   _tracker_channel_hit_collection_.end(),
					   fecom::tracker_channel_hit::find_by_channel(associated_feast_id_2, associated_channel_2));

	      if (it_set_2 != _tracker_channel_hit_collection_.end() && !it_set_2 -> associated) {
		a_tracker_hit.add_tracker_channel(*it_set_2);
		it_set_2 -> associated = true;
	      }

	    } // end of if cathodic

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
