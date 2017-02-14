// fecom/channel_mapping.cpp

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

  void channel_mapping::cell_to_channel_builder()
  {
    for (uint16_t icell = 0; icell < tracker_constants::NUMBER_OF_CELLS_PER_BOARD; icell++)
      {
	channel_triolet a_channel_triolet;
	if (icell < 9)
	  {
	    a_channel_triolet.feast_1 = 0;
	    a_channel_triolet.feast_2 = 0;
	    a_channel_triolet.feast_3 = 0;
	    if (icell % 2 == 0) a_channel_triolet.channel_1 = icell * 3;
	    else a_channel_triolet.channel_1 = icell * 3 + 1;

	    a_channel_triolet.channel_2 = icell * 2 + 1;
	    a_channel_triolet.channel_3 = icell * 6 + 2;
	  }
	else if (icell >= 9 && icell < 18)
	  {
	    a_channel_triolet.feast_1 = 0;
	    a_channel_triolet.feast_2 = 0;
	    a_channel_triolet.feast_3 = 1;
	    if (icell % 2 == 0) a_channel_triolet.channel_1 = icell * 3;
	    else a_channel_triolet.channel_1 = icell * 3 + 1;

	    a_channel_triolet.channel_2 = icell * 2 + 1;
	    a_channel_triolet.channel_3 = (icell - 9) * 6 + 2;
	  }
	else if (icell >= 18 && icell < 27)
	  {
	    a_channel_triolet.feast_1 = 1;
	    a_channel_triolet.feast_2 = 1;
	    a_channel_triolet.feast_3 = 0;
	    if (icell % 2 == 0) a_channel_triolet.channel_1 = (icell - 18) * 3;
	    else a_channel_triolet.channel_1 = (icell - 18) * 3 + 1;

	    a_channel_triolet.channel_2 = (icell - 18) * 2 + 1;
	    a_channel_triolet.channel_3 = icell * 2 + 1;
	  }
	else if (icell >= 27 && icell < tracker_constants::NUMBER_OF_CELLS_PER_BOARD)
	  {
	    a_channel_triolet.feast_1 = 1;
	    a_channel_triolet.feast_2 = 1;
	    a_channel_triolet.feast_3 = 1;
	    if (icell % 2 == 0) a_channel_triolet.channel_1 = (icell - 18) * 3;
	    else a_channel_triolet.channel_1 = (icell - 18) * 3 + 1;

	    a_channel_triolet.channel_2 = (icell - 18) * 2 + 1;
	    a_channel_triolet.channel_3 = (icell - 9) * 2 + 1;
	  }

	_cell_channel_mapping_.insert(std::pair<uint16_t, fecom::channel_mapping::channel_triolet> (icell, a_channel_triolet));
      }

    return;
  }





  void channel_mapping::get_associated_channels(const uint16_t input_feast_,
						const uint16_t input_channel_,
						uint16_t & /* associated_feast_1_*/,
						uint16_t & associated_channel_1_,
						uint16_t & /* associated_feast_2_ */,
						uint16_t & associated_channel_2_) const
  {
    DT_THROW_IF(input_feast_ >= tracker_constants::NUMBER_OF_FEAST_PER_BOARD,
		std::logic_error,
		"Feast ID '" + std::to_string(input_feast_) + "' is not valid ! ");
    DT_THROW_IF(input_channel_ >= tracker_constants::NUMBER_OF_CHANNEL_PER_FEAST,
		std::logic_error,
		"Channel ID '" + std::to_string(input_channel_) + "' is not valid ! ");


    uint16_t result_modulo = input_channel_ % 6;

    if (result_modulo == 0 || result_modulo == 3)
      {
    	associated_channel_1_ = input_channel_ + 1;
    	associated_channel_2_ = input_channel_ + 2;
      }

    // else if (result_modulo == 1 || result_modulo == 4)
    //   {
    // 	associated_channel_1_ = channel_id_ - 1;
    // 	associated_channel_2_ = channel_id_ + 1;
    //   }

    // else if (result_modulo == 2)
    //   {
    //    	associated_channel_1_ = channel_id_ - 2;
    // 	associated_channel_2_ = channel_id_ - 1;
    //   }

    // else if (result_modulo == 5)
    //   {
    //    	associated_channel_1_ = channel_id_ - 1;
    // 	associated_channel_2_ = channel_id_ - 2;
    //   }






    return;
  }

  bool channel_mapping::is_anodic_channel(const uint16_t /* feast_id */,
					  const uint16_t channel_id_) const
  {
    if (channel_id_ % 6 == 0 || channel_id_ % 6 == 4) return true;
    else return false;
  }

  bool channel_mapping::is_bottom_cathodic_channel(const uint16_t /* feast_id */,
						   const uint16_t channel_id_) const
  {
    if (channel_id_ % 6 == 1 || channel_id_ % 6 == 3) return true;
    else return false;
  }

  bool channel_mapping::is_top_cathodic_channel(const uint16_t /* feast_id */,
						const uint16_t channel_id_) const
  {
    if (channel_id_ % 6 == 2 || channel_id_ % 6 == 5) return true;
    else return false;
  }

  void channel_mapping::reset()
  {
    _reset_();
    return;
  }

  void channel_mapping::_reset_()
  {
    return;
  }
  void channel_mapping::tree_dump(std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool /*inherit_*/) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    // out_ << indent_ << io::tag()
    //      << "Trigger ID : " << _trigger_id_ << std::endl;

    return;
  }

} // namespace fecom
