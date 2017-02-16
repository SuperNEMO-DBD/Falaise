// fecom/channel_mapping.cpp
#include <fstream>

// Ourselves:
#include <fecom/channel_mapping.hpp>

// This project:
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/tracker_constants.hpp>

namespace fecom {

  channel_mapping::channel_mapping()
  {
    reset();
  }

  channel_mapping::~channel_mapping()
  {
  }

  void channel_mapping::reset()
  {
    _reset_();
    return;
  }

  void channel_mapping::_reset_()
  {
    _channel_triplet_collection_.clear();
    _cell_channel_mapping_.clear();
    initialized = false;
    return;
  }

  bool channel_mapping::is_initialized() const
  {
    return initialized;
  }

  void channel_mapping::initialize()
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Channel mapping is already initialized !");
    //  _build_channel_triplet_collection();
    _cell_to_channel_mapping();
    initialized = true;
    return;
  }

  void channel_mapping::build_mapping_from_file(const std::string & filename_)
  {
    std::ifstream file_stream(filename_.c_str(), std::ifstream::in);
    if (file_stream)
      {
	std::string line = "";
	std::size_t line_counter = 0;
	while (std::getline(file_stream, line))
	  {
	    if (line_counter != 0) // ignore the header
	      {
		std::stringstream ss(line);

		channel_triplet a_channel_triplet;
		a_channel_triplet.anodic_channel.channel_type = tracker_board_channel_id::ANODIC_CHANNEL;
		a_channel_triplet.bottom_cathode_channel.channel_type = tracker_board_channel_id::BOTTOM_CATHODIC_CHANNEL;
		a_channel_triplet.top_cathode_channel.channel_type = tracker_board_channel_id::TOP_CATHODIC_CHANNEL;

		uint16_t cell_number = -1;
		uint16_t anode_number = -1;
		uint16_t bot_cathode_number = -1;
		uint16_t top_cathode_number = -1;

		ss >> cell_number >> anode_number >> bot_cathode_number >> top_cathode_number;

		int check_feast_anode = anode_number - 54;
		int check_feast_bot_cath = bot_cathode_number - 54;
		int check_feast_top_cath = top_cathode_number - 54;

		if (check_feast_anode >= 0)
		  {
		    a_channel_triplet.anodic_channel.feast_id = 1;
		    a_channel_triplet.anodic_channel.channel_id = check_feast_anode;
		  }
		else
		  {
		    a_channel_triplet.anodic_channel.feast_id = 0;
		    a_channel_triplet.anodic_channel.channel_id = anode_number;
		  }
		if (check_feast_bot_cath >= 0)
		  {
		    a_channel_triplet.bottom_cathode_channel.feast_id = 1;
		    a_channel_triplet.bottom_cathode_channel.channel_id = check_feast_bot_cath;
		  }
		else
		  {
		    a_channel_triplet.bottom_cathode_channel.feast_id = 0;
		    a_channel_triplet.bottom_cathode_channel.channel_id = bot_cathode_number;
		  }

		if (check_feast_top_cath >= 0)
		  {
		    a_channel_triplet.top_cathode_channel.feast_id = 1;
		    a_channel_triplet.top_cathode_channel.channel_id = check_feast_top_cath;
		  }
		else
		  {
		    a_channel_triplet.top_cathode_channel.feast_id = 0;
		    a_channel_triplet.top_cathode_channel.channel_id = top_cathode_number;
		  }

		_channel_triplet_collection_.push_back(a_channel_triplet);

	      }
	    line_counter++;
	  }

	file_stream.close();
      }
    else  DT_THROW(std::runtime_error, "Cannot open input file '" << filename_ << "'");

    return;
  }

  void channel_mapping::_cell_to_channel_mapping()
  {
    DT_THROW_IF(_channel_triplet_collection_.size() == 0,
		std::logic_error,
		"Channel triplet collection is empty, check your mapping !");

    std::size_t cell_counter = 0;
    for (auto it_vector =_channel_triplet_collection_.begin();
	 it_vector != _channel_triplet_collection_.end();
	 it_vector++)
      {
	uint16_t feast_channel_anodic = it_vector->anodic_channel.feast_id * 54 + it_vector->anodic_channel.channel_id;
	uint16_t feast_channel_bottom_cathode = it_vector->bottom_cathode_channel.feast_id * 54 + it_vector->bottom_cathode_channel.channel_id;
	uint16_t feast_channel_top_cathode = it_vector->top_cathode_channel.feast_id * 54 + it_vector->top_cathode_channel.channel_id;

	_cell_channel_mapping_.insert(std::pair<uint16_t, uint16_t> (feast_channel_anodic, cell_counter));
	_cell_channel_mapping_.insert(std::pair<uint16_t, uint16_t> (feast_channel_bottom_cathode, cell_counter));
	_cell_channel_mapping_.insert(std::pair<uint16_t, uint16_t> (feast_channel_top_cathode, cell_counter));

	cell_counter++;
      }
  }

  void channel_mapping::_build_channel_triplet_collection()
  {
    // Build with a logic. For commissioning the mapping will be build from a csv file and
    // this method will not be used.

    for (uint16_t icell = 0; icell < tracker_constants::NUMBER_OF_CELLS_PER_BOARD; icell++)
      {
	channel_triplet a_channel_triplet;
	a_channel_triplet.anodic_channel.channel_type = tracker_board_channel_id::ANODIC_CHANNEL;
	a_channel_triplet.bottom_cathode_channel.channel_type = tracker_board_channel_id::BOTTOM_CATHODIC_CHANNEL;
	a_channel_triplet.top_cathode_channel.channel_type = tracker_board_channel_id::TOP_CATHODIC_CHANNEL;

	if (icell < 9)
	  {
	    a_channel_triplet.anodic_channel.feast_id = 0;
	    a_channel_triplet.bottom_cathode_channel.feast_id = 0;
	    a_channel_triplet.top_cathode_channel.feast_id = 0;

	    if (icell % 2 == 0) a_channel_triplet.anodic_channel.channel_id = icell * 3;
	    else a_channel_triplet.anodic_channel.channel_id = icell * 3 + 1;
	    a_channel_triplet.bottom_cathode_channel.channel_id = icell * 2 + 1;
	    a_channel_triplet.top_cathode_channel.channel_id = icell * 6 + 2;
	  }
	else if (icell >= 9 && icell < 18)
	  {
	    a_channel_triplet.anodic_channel.feast_id = 0;
	    a_channel_triplet.bottom_cathode_channel.feast_id = 0;
	    a_channel_triplet.top_cathode_channel.feast_id = 1;

	    if (icell % 2 == 0) a_channel_triplet.anodic_channel.channel_id = icell * 3;
	    else a_channel_triplet.anodic_channel.channel_id = icell * 3 + 1;
	    a_channel_triplet.bottom_cathode_channel.channel_id = icell * 2 + 1;
	    a_channel_triplet.top_cathode_channel.channel_id = (icell - 9) * 6 + 2;
	  }
	else if (icell >= 18 && icell < 27)
	  {
	    a_channel_triplet.anodic_channel.feast_id = 1;
	    a_channel_triplet.bottom_cathode_channel.feast_id = 1;
	    a_channel_triplet.top_cathode_channel.feast_id = 0;

	    if (icell % 2 == 0) a_channel_triplet.anodic_channel.channel_id = (icell - 18) * 3;
	    else a_channel_triplet.anodic_channel.channel_id = (icell - 18) * 3 + 1;
	    a_channel_triplet.bottom_cathode_channel.channel_id = (icell - 18) * 2 + 1;
	    a_channel_triplet.top_cathode_channel.channel_id = icell * 2 + 1;
	  }
	else if (icell >= 27 && icell < tracker_constants::NUMBER_OF_CELLS_PER_BOARD)
	  {
	    a_channel_triplet.anodic_channel.feast_id = 1;
	    a_channel_triplet.bottom_cathode_channel.feast_id = 1;
	    a_channel_triplet.top_cathode_channel.feast_id = 1;

	    if (icell % 2 == 0) a_channel_triplet.anodic_channel.channel_id = (icell - 18) * 3;
	    else a_channel_triplet.anodic_channel.channel_id = (icell - 18) * 3 + 1;
	    a_channel_triplet.bottom_cathode_channel.channel_id = (icell - 18) * 2 + 1;
	    a_channel_triplet.top_cathode_channel.channel_id = (icell - 9) * 2 + 1;
	  }

	_channel_triplet_collection_.push_back(a_channel_triplet);

      }

    return;
  }

  void channel_mapping::get_associated_channels(const uint16_t input_feast_,
						const uint16_t input_channel_,
						uint16_t & associated_feast_1_,
						uint16_t & associated_channel_1_,
						uint16_t & associated_feast_2_,
						uint16_t & associated_channel_2_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    uint16_t input_feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(input_feast_channel);
    uint16_t cell_number = 0;
    if (it_map != _cell_channel_mapping_.end()) cell_number = it_map->second;

    channel_triplet a_channel_triplet;
    a_channel_triplet = _channel_triplet_collection_.at(cell_number);

    // Input : anodic
    if (is_anodic_channel(input_feast_, input_channel_))
      {
	associated_feast_1_   = a_channel_triplet.bottom_cathode_channel.feast_id;
	associated_channel_1_ = a_channel_triplet.bottom_cathode_channel.channel_id;

	associated_feast_2_   = a_channel_triplet.top_cathode_channel.feast_id;
	associated_channel_2_ = a_channel_triplet.top_cathode_channel.channel_id;
      }

    // Input : bottom cathode
    if (is_bottom_cathodic_channel(input_feast_, input_channel_))
      {
	associated_feast_1_ = a_channel_triplet.anodic_channel.feast_id;
	associated_channel_1_ = a_channel_triplet.anodic_channel.channel_id;

	associated_feast_2_   = a_channel_triplet.top_cathode_channel.feast_id;
	associated_channel_2_ = a_channel_triplet.top_cathode_channel.channel_id;
      }

    // Input : top cathode
    if (is_top_cathodic_channel(input_feast_, input_channel_))
      {
	associated_feast_1_   = a_channel_triplet.anodic_channel.feast_id;
	associated_channel_1_ = a_channel_triplet.anodic_channel.channel_id;

	associated_feast_2_   = a_channel_triplet.bottom_cathode_channel.feast_id;
	associated_channel_2_ = a_channel_triplet.bottom_cathode_channel.channel_id;
      }
  }

  void channel_mapping::get_associated_channels_with_types(const uint16_t input_feast_,
							   const uint16_t input_channel_,
							   uint16_t & associated_anodic_feast_,
							   uint16_t & associated_anodic_channel_,
							   uint16_t & associated_bottom_cathodic_feast_,
							   uint16_t & associated_bottom_cathodic_channel_,
							   uint16_t & associated_top_cathodic_feast_,
							   uint16_t & associated_top_cathodic_channel_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    DT_THROW_IF(input_feast_ >= tracker_constants::NUMBER_OF_FEAST_PER_BOARD,
		std::logic_error,
		"Feast ID '" + std::to_string(input_feast_) + "' is not valid ! ");
    DT_THROW_IF(input_channel_ >= tracker_constants::NUMBER_OF_CHANNEL_PER_FEAST,
		std::logic_error,
		"Channel ID '" + std::to_string(input_channel_) + "' is not valid ! ");

    uint16_t input_feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(input_feast_channel);
    uint16_t cell_number = 0;
    if (it_map != _cell_channel_mapping_.end()) cell_number = it_map->second;

    channel_triplet a_channel_triplet;
    a_channel_triplet = _channel_triplet_collection_.at(cell_number);

    associated_anodic_feast_ = a_channel_triplet.anodic_channel.feast_id;
    associated_anodic_channel_ = a_channel_triplet.anodic_channel.channel_id;

    associated_bottom_cathodic_feast_ = a_channel_triplet.bottom_cathode_channel.feast_id;
    associated_bottom_cathodic_channel_ = a_channel_triplet.bottom_cathode_channel.channel_id;

    associated_top_cathodic_feast_ = a_channel_triplet.top_cathode_channel.feast_id;
    associated_top_cathodic_channel_ = a_channel_triplet.top_cathode_channel.channel_id;

    return;
  }

  void channel_mapping::get_cell_number_for_a_channel(const uint16_t input_feast_,
						      const uint16_t input_channel_,
						      uint16_t & cell_number_) const
  {
    uint16_t feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(feast_channel);
    if (it_map != _cell_channel_mapping_.end()) cell_number_ = it_map->second;
    else DT_THROW(std::logic_error, "The input channel Feast #" + std::to_string(input_feast_) + " Channel #" + std::to_string(input_channel_) + " is not valid !");
    return;
  }


  bool channel_mapping::is_anodic_channel(const uint16_t input_feast_,
					  const uint16_t input_channel_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    uint16_t input_feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(input_feast_channel);
    uint16_t cell_number = 0;
    if (it_map != _cell_channel_mapping_.end()) cell_number = it_map->second;

    channel_triplet a_channel_triplet;
    a_channel_triplet = _channel_triplet_collection_.at(cell_number);

    if (input_feast_ == a_channel_triplet.anodic_channel.feast_id
	&& input_channel_ == a_channel_triplet.anodic_channel.channel_id) return true;

    else return false;
  }

  bool channel_mapping::is_bottom_cathodic_channel(const uint16_t input_feast_,
						   const uint16_t input_channel_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    uint16_t input_feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(input_feast_channel);
    uint16_t cell_number = 0;
    if (it_map != _cell_channel_mapping_.end()) cell_number = it_map->second;

    channel_triplet a_channel_triplet;
    a_channel_triplet = _channel_triplet_collection_.at(cell_number);

    if (input_feast_ == a_channel_triplet.bottom_cathode_channel.feast_id
	&& input_channel_ == a_channel_triplet.bottom_cathode_channel.channel_id) return true;

    else return false;
  }

  bool channel_mapping::is_top_cathodic_channel(const uint16_t input_feast_,
						const uint16_t input_channel_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    uint16_t input_feast_channel = 54 * input_feast_ + input_channel_;
    auto it_map = _cell_channel_mapping_.find(input_feast_channel);
    uint16_t cell_number = 0;
    if (it_map != _cell_channel_mapping_.end()) cell_number = it_map->second;

    channel_triplet a_channel_triplet;
    a_channel_triplet = _channel_triplet_collection_.at(cell_number);

    if (input_feast_ == a_channel_triplet.top_cathode_channel.feast_id
	&& input_channel_ == a_channel_triplet.top_cathode_channel.channel_id) return true;

    else return false;
  }

  void channel_mapping::tree_dump(std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool /*inherit_*/) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
	 << "Initialized : " << is_initialized() << std::endl;

    out_ << indent_ << io::tag()
	 << "Channel triplet collection size : " << _channel_triplet_collection_.size() << std::endl;

    out_ << indent_ << io::tag()
	 << "Channel <-> cell mapping size : " << _cell_channel_mapping_.size() << std::endl;

    return;
  }

} // namespace fecom
