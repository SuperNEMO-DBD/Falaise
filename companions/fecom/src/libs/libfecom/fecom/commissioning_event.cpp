// fecom/commissioning_event.cpp

// Ourselves:
#include <fecom/commissioning_event.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(commissioning_event,
						    "fecom::commissioning_event")

  commissioning_event::commissioning_event()
  {
    reset();
  }

  commissioning_event::~commissioning_event()
  {
  }

  bool commissioning_event::is_valid() const
  {
    return _event_id_.is_valid();
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

  const datatools::event_id & commissioning_event::get_event_id() const
  {
    return _event_id_;
  }

  datatools::event_id & commissioning_event::grab_event_id()
  {
    return _event_id_;
  }

  void commissioning_event::set_event_id(const datatools::event_id & event_id_)
  {
    _event_id_ = event_id_;
    return;
  }

  double commissioning_event::get_time_start_ns() const
  {
    return _time_start_ns_;
  }

  void commissioning_event::set_time_start_ns(const double time_start_ns_)
  {
    _time_start_ns_ = time_start_ns_;
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
	  geomtools::geom_id electronic_channel_id = ichan -> electronic_id;
	  const uint16_t channel_type = electronic_channel_id.get_type();
	  // const uint16_t board_id = electronic_channel_id.get(tracker_constants::SLOT_INDEX);
	  // const uint16_t feast_id = electronic_channel_id.get(tracker_constants::FEAST_INDEX);
	  // const uint16_t channel  = electronic_channel_id.get(tracker_constants::CHANNEL_INDEX);
	  geomtools::geom_id associated_geometric_id;

	  _my_channel_mapping_-> get_geometric_id_from_electronic_id(electronic_channel_id,
								     associated_geometric_id);

	  geomtools::geom_id cell_geometric_id(tracker_constants::GEOMETRIC_CELL_TYPE,
					       associated_geometric_id.get(tracker_constants::LAYER_INDEX),
					       associated_geometric_id.get(tracker_constants::ROW_INDEX));
	  a_tracker_hit.cell_geometric_id = cell_geometric_id;
	  geomtools::geom_id anodic_id;
	  geomtools::geom_id bot_cathodic_id;
	  geomtools::geom_id top_cathodic_id;

	  _my_channel_mapping_-> get_associated_electronics_id(electronic_channel_id,
							       anodic_id,
							       bot_cathodic_id,
							       top_cathodic_id);

	  const std::string timestamp = ichan -> timestamp_type;
	  DT_THROW_IF(!(channel_type == tracker_constants::ANODIC_CHANNEL_TYPE || channel_type == tracker_constants::CATHODIC_CHANNEL_TYPE),
		      std::logic_error,
		      "The input tracker channel : " << electronic_channel_id << "' is not coherent with the mapping ! Check if the mapping (in the csv file) is correct !");

	  // Search all channels in the tracker channel hit collection
	  uint16_t associated_anodic_feast = anodic_id.get(tracker_constants::FEAST_INDEX);
	  uint16_t associated_anodic_channel = anodic_id.get(tracker_constants::CHANNEL_INDEX);
	  uint16_t associated_bottom_cathodic_feast = bot_cathodic_id.get(tracker_constants::FEAST_INDEX);
	  uint16_t associated_bottom_cathodic_channel = bot_cathodic_id.get(tracker_constants::CHANNEL_INDEX);
	  uint16_t associated_top_cathodic_feast = top_cathodic_id.get(tracker_constants::FEAST_INDEX);
	  uint16_t associated_top_cathodic_channel = top_cathodic_id.get(tracker_constants::CHANNEL_INDEX);

	  // For the anodic channel, search the 5 timestamps :
	  for (std::size_t itime = 0; itime < 5; itime++)
	    {
	      std::string search_timestamp = "R"+ std::to_string(itime);
	      auto it_set = std::find_if(_tracker_channel_hit_collection_.begin(),
	  				 _tracker_channel_hit_collection_.end(),
					 fecom::tracker_channel_hit::find_by_timestamp(associated_anodic_feast,
	  									       associated_anodic_channel,
	  									       search_timestamp));
	      if (it_set != _tracker_channel_hit_collection_.end() && !it_set -> associated) {
	  	it_set -> associated = true;
	  	a_tracker_hit.add_anodic_tracker_channel(*it_set);
	      }
	    }

	  // For the bottom cathodic channel, search the only timestamp :
	  auto it_set_bot_cat = std::find_if(_tracker_channel_hit_collection_.begin(),
	  				     _tracker_channel_hit_collection_.end(),
	  				     fecom::tracker_channel_hit::find_by_channel(associated_bottom_cathodic_feast,
	  										 associated_bottom_cathodic_channel));

	  if (it_set_bot_cat != _tracker_channel_hit_collection_.end() && !it_set_bot_cat -> associated) {
	    it_set_bot_cat -> associated = true;
	    a_tracker_hit.add_bot_cathodic_tracker_channel(*it_set_bot_cat);
	  }

	  // For the top cathodic channel, search the only timestamp :
	  auto it_set_top_cat = std::find_if(_tracker_channel_hit_collection_.begin(),
	  				     _tracker_channel_hit_collection_.end(),
	  				     fecom::tracker_channel_hit::find_by_channel(associated_top_cathodic_feast,
	  										 associated_top_cathodic_channel));

	  if (it_set_top_cat != _tracker_channel_hit_collection_.end() && !it_set_top_cat -> associated) {
	    it_set_top_cat -> associated = true;
	    a_tracker_hit.add_top_cathodic_tracker_channel(*it_set_top_cat);
	  }

	  // Add the tracker hit to the collection :
	  if (a_tracker_hit.is_valid())	  _tracker_hit_collection_.push_back(a_tracker_hit);

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
    _event_id_.reset();
    datatools::invalidate(_time_start_ns_);
    _traits_.reset();
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

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Event_id : " << _event_id_.to_string() << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Time start (ns) : " << _time_start_ns_ << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Traits : " << _traits_ << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Calo hit collection size : " << _calo_hit_collection_.size() << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Tracker channel hit collection size : " << _tracker_channel_hit_collection_.size() << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::inherit_tag(inherit_)
	 << "Tracker hit collection size : " << _tracker_hit_collection_.size() << std::endl;

    return;
  }

  bool commissioning_event::compare::operator()(const commissioning_event & a,
						const commissioning_event & b)
  {
    return a.get_event_id() < b.get_event_id();
  }

} // namespace fecom
