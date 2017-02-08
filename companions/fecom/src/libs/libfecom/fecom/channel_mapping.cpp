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

  void channel_mapping::get_associated_channels(const uint16_t channel_id_,
						uint16_t & associated_channel_1_,
						uint16_t & associated_channel_2_)
  {
    DT_THROW_IF(channel_id_ >= tracker_constants::NUMBER_OF_CHANNEL_PER_FEAST,
		std::logic_error,
		"Channel ID '" + std::to_string(channel_id_) + "' is not valid ! ");

    uint16_t result_modulo = channel_id_ % 6;

    if (result_modulo == 0 || result_modulo == 3)
      {
	associated_channel_1_ = channel_id_ + 1;
	associated_channel_2_ = channel_id_ + 2;
      }

    else if (result_modulo == 1 || result_modulo == 4)
      {
	associated_channel_1_ = channel_id_ - 1;
	associated_channel_2_ = channel_id_ + 1;
      }

    else if (result_modulo == 2)
      {
       	associated_channel_1_ = channel_id_ - 2;
	associated_channel_2_ = channel_id_ - 1;
      }

    else if (result_modulo == 5)
      {
       	associated_channel_1_ = channel_id_ - 1;
	associated_channel_2_ = channel_id_ - 2;
      }

    return;
  }

  bool channel_mapping::is_anodic_channel(const uint16_t channel_id_)
  {
    if (channel_id_ % 6 == 0 || channel_id_ % 6 == 4) return true;
    else return false;
  }

  bool channel_mapping::is_bottom_cathodic_channel(const uint16_t channel_id_)
  {
    if (channel_id_ % 6 == 1 || channel_id_ % 6 == 3) return true;
    else return false;
  }

  bool channel_mapping::is_top_cathodic_channel(const uint16_t channel_id_)
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
