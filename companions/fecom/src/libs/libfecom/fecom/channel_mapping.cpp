// fecom/channel_mapping.cpp
#include <fstream>

// Ourselves:
#include <fecom/channel_mapping.hpp>

// This project:
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/tracker_constants.hpp>
#include <fecom/calo_constants.hpp>

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
    tracker_bimap.clear();
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
    initialized = true;
    return;
  }

  void channel_mapping::build_calo_mapping_from_file(const std::string & filename_)
  {
    std::ifstream file_stream(filename_.c_str(), std::ifstream::in);
    if (file_stream)
      {
	std::string line = "";
	std::size_t line_counter = 0;
	while (std::getline(file_stream, line))
	  {
	    if (line_counter != 0) // ignore the header
	      {	std::stringstream ss(line);

		uint16_t slot_number = -1;
		uint16_t channel_number = -1;
		uint16_t column_number = -1;
		uint16_t row_number = -1;

		ss >> slot_number >> channel_number >> column_number >> row_number;

		// Electronic ID
		geomtools::geom_id electronic_id(calo_constants::CALO_CHANNEL_TYPE,
						 slot_number,
						 channel_number);

		// Geometric ID
		geomtools::geom_id geometric_id(calo_constants::GEOMETRIC_CALO_TYPE,
						column_number,
						row_number);

		calo_bimap.insert(ID_doublet(geometric_id, electronic_id));
	      }
	    line_counter++;
	  }

	file_stream.close();
      }
    else  DT_THROW(std::runtime_error, "Cannot open input file '" << filename_ << "'");

    return;
  }

  void channel_mapping::build_tracker_mapping_from_file(const std::string & filename_)
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

		uint16_t slot_number = -1;
		uint16_t layer_number = -1;
		uint16_t row_number = -1;
		uint16_t anode_number = -1;
		uint16_t bot_cathode_number = -1;
		uint16_t top_cathode_number = -1;

		ss >> slot_number >> layer_number >> row_number >> anode_number >> bot_cathode_number >> top_cathode_number;

		uint16_t feast_anode = -1;
		uint16_t feast_bot_cathode = -1;
		uint16_t feast_top_cathode = -1;

		uint16_t channel_anode = -1;
		uint16_t channel_bot_cathode = -1;
		uint16_t channel_top_cathode = -1;

		if (anode_number - 54 >= 0) {
		  feast_anode = 1;
		  channel_anode = anode_number - 54;
		} else {
		  feast_anode = 0;
		  channel_anode = anode_number;
		}
		if (bot_cathode_number - 54 >= 0) {
		  feast_bot_cathode = 1;
		  channel_bot_cathode = bot_cathode_number - 54;
		} else {
		  feast_bot_cathode = 0;
		  channel_bot_cathode = bot_cathode_number;
		}
		if (top_cathode_number - 54 >= 0) {
		  feast_top_cathode = 1;
		  channel_top_cathode = top_cathode_number - 54;
		} else {
		  feast_top_cathode = 0;
		  channel_top_cathode = top_cathode_number;
		}

		// Geometric ID
		geomtools::geom_id cell_id_anodic(tracker_constants::GEOMETRIC_CELL_TYPE,
						  layer_number,
						  row_number,
						  tracker_constants::ANODIC_PORT);

		geomtools::geom_id cell_id_bot_cathode(tracker_constants::GEOMETRIC_CELL_TYPE,
						       layer_number,
						       row_number,
						       tracker_constants::BOT_CATHODE_PORT);

		geomtools::geom_id cell_id_top_cathode(tracker_constants::GEOMETRIC_CELL_TYPE,
						       layer_number,
						       row_number,
						       tracker_constants::TOP_CATHODE_PORT);

		// Electronic ID
		geomtools::geom_id anode_channel_id(tracker_constants::ANODIC_CHANNEL_TYPE,
						    slot_number,
						    feast_anode,
						    channel_anode);

		geomtools::geom_id bottom_cathode_channel_id(tracker_constants::CATHODIC_CHANNEL_TYPE,
							     slot_number,
							     feast_bot_cathode,
							     channel_bot_cathode);

		geomtools::geom_id top_cathode_channel_id(tracker_constants::CATHODIC_CHANNEL_TYPE,
							  slot_number,
							  feast_top_cathode,
							  channel_top_cathode);

		tracker_bimap.insert(ID_doublet(cell_id_anodic, anode_channel_id));
		tracker_bimap.insert(ID_doublet(cell_id_bot_cathode, bottom_cathode_channel_id));
		tracker_bimap.insert(ID_doublet(cell_id_top_cathode, top_cathode_channel_id));

	      }
	    line_counter++;
	  }

	file_stream.close();
      }
    else  DT_THROW(std::runtime_error, "Cannot open input file '" << filename_ << "'");

    return;
  }

  void channel_mapping::get_cell_layer_row_for_a_channel(const geomtools::geom_id & electronic_id_,
							 uint16_t & layer_number_,
							 uint16_t & row_number_) const
  {
    auto it_map = tracker_bimap.right.find(electronic_id_);
    if (it_map != tracker_bimap.right.end()) {
      geomtools::geom_id associated_geom_id = it_map -> second;
      std::clog << associated_geom_id << std::endl;
      layer_number_ = associated_geom_id.get(tracker_constants::LAYER_INDEX);
      row_number_ = associated_geom_id.get(tracker_constants::ROW_INDEX);
    }
    else DT_THROW(std::logic_error, "The input channel with EID : " << electronic_id_ << " is not in the bimap GID <-> EID !");
    return;
  }

  void channel_mapping::get_electronics_id_for_a_cell_layer_row(const uint16_t & layer_number_,
								const uint16_t & row_number_,
								geomtools::geom_id & electronic_anodic_id_,
								geomtools::geom_id & electronic_bot_cathodic_id_,
								geomtools::geom_id & electronic_top_cathodic_id_) const
  {
    geomtools::geom_id geometric_anodic_cell_id(tracker_constants::GEOMETRIC_CELL_TYPE,
						layer_number_,
						row_number_,
						tracker_constants::ANODIC_PORT);

    get_electronic_id_from_geometric_id(geometric_anodic_cell_id,
					electronic_anodic_id_);

    geomtools::geom_id geometric_bot_cathodic_cell_id(tracker_constants::GEOMETRIC_CELL_TYPE,
						      layer_number_,
						      row_number_,
						      tracker_constants::BOT_CATHODE_PORT);

    get_electronic_id_from_geometric_id(geometric_bot_cathodic_cell_id,
					electronic_bot_cathodic_id_);

    geomtools::geom_id geometric_top_cathodic_cell_id(tracker_constants::GEOMETRIC_CELL_TYPE,
						      layer_number_,
						      row_number_,
						      tracker_constants::TOP_CATHODE_PORT);

    get_electronic_id_from_geometric_id(geometric_top_cathodic_cell_id,
					electronic_top_cathodic_id_);

    return;
  }

  void channel_mapping::get_electronic_id_from_geometric_id(const geomtools::geom_id & geometric_id_,
							    geomtools::geom_id & electronic_id_) const
  {
    if (geometric_id_.get_type() == tracker_constants::GEOMETRIC_CELL_TYPE)
      {
    	auto it_map = tracker_bimap.left.find(geometric_id_);
	if (it_map != tracker_bimap.left.end()) {
	  electronic_id_ = it_map -> second;
	}
	else DT_THROW(std::logic_error, "Input geometric tracker id is not find in the bimap " << geometric_id_ << " !");
      }
    else if (geometric_id_.get_type() == calo_constants::GEOMETRIC_CALO_TYPE)
      {
	auto it_map = calo_bimap.left.find(geometric_id_);
	if (it_map != calo_bimap.left.end()) {
	  electronic_id_ = it_map -> second;
	}
	else DT_THROW(std::logic_error, "Input geometric calo id is not find in the bimap " << geometric_id_ << " !");
      }
    else DT_THROW(std::logic_error, "Input geometric id type is not valid '" << geometric_id_.get_type() << " !");

    return;
  }

  void channel_mapping::get_geometric_id_from_electronic_id(const geomtools::geom_id & electronic_id_,
							    geomtools::geom_id & geometric_id_) const
  {
    if (electronic_id_.get_type() == tracker_constants::ANODIC_CHANNEL_TYPE
	|| electronic_id_.get_type() == tracker_constants::CATHODIC_CHANNEL_TYPE)
      {
	auto it_map = tracker_bimap.right.find(electronic_id_);

	if (it_map != tracker_bimap.right.end()) {
	  geometric_id_ = it_map -> second;
	}

	else DT_THROW(std::logic_error, "Input electronic tracker id is not find in the bimap " << electronic_id_ << " !");
      }
    else if (electronic_id_.get_type() == calo_constants::CALO_CHANNEL_TYPE)
      {
	auto it_map = calo_bimap.right.find(electronic_id_);

	if (it_map != calo_bimap.right.end()) {
	  geometric_id_ = it_map -> second;
	}

	else DT_THROW(std::logic_error, "Input electronic calo id is not find in the bimap " << electronic_id_ << " !");
      }
    else DT_THROW(std::logic_error, "Input electronic id type is not valid '" << electronic_id_.get_type() << " !");

    return;
  }

  void channel_mapping::get_associated_electronics_id(const geomtools::geom_id & input_electronic_id_,
						      geomtools::geom_id & electronic_anodic_id_,
						      geomtools::geom_id & electronic_bot_cathodic_id_,
						      geomtools::geom_id & electronic_top_cathodic_id_) const
  {
    geomtools::geom_id geometric_cell_id;
    get_geometric_id_from_electronic_id(input_electronic_id_,
					geometric_cell_id);
    uint32_t layer = geometric_cell_id.get(tracker_constants::LAYER_INDEX);
    uint32_t row = geometric_cell_id.get(tracker_constants::ROW_INDEX);

    get_electronics_id_for_a_cell_layer_row(layer,
					    row,
					    electronic_anodic_id_,
					    electronic_bot_cathodic_id_,
					    electronic_top_cathodic_id_);

    return;
  }

  bool channel_mapping::is_anodic_channel(const geomtools::geom_id & input_electronic_id_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");

    return input_electronic_id_.get_type() == tracker_constants::ANODIC_CHANNEL_TYPE;

  }

  bool channel_mapping::is_bottom_cathodic_channel(const geomtools::geom_id & input_electronic_id_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    geomtools::geom_id associated_geometric_id;

    get_geometric_id_from_electronic_id(input_electronic_id_,
					associated_geometric_id);
    bool is_bot_cathodic = false;
    if (associated_geometric_id.get(tracker_constants::PORT_INDEX) == tracker_constants::BOT_CATHODE_PORT) is_bot_cathodic = true;
    return is_bot_cathodic;
  }

  bool channel_mapping::is_top_cathodic_channel(const geomtools::geom_id & input_electronic_id_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Channel mapping is not initizalied !");
    geomtools::geom_id associated_geometric_id;

    get_geometric_id_from_electronic_id(input_electronic_id_,
					associated_geometric_id);
    bool is_top_cathodic = false;
    if (associated_geometric_id.get(tracker_constants::PORT_INDEX) == tracker_constants::TOP_CATHODE_PORT) is_top_cathodic = true;
    return is_top_cathodic;
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

    return;
  }

} // namespace fecom
