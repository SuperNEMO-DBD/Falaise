// fecom/calo_pedestal_calib.cpp

// Ourselves:
#include <fecom/calo_pedestal_calib.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

calo_pedestal_calib::calo_pedestal_calib() {
  _reset_();
  return;
}

bool calo_pedestal_calib::is_valid() const {
  if (slot_index >= 16) return false;
  if (channel >= calo_constants::INVALID_BOARD_CHANNEL) return false;
  return (offset_size > 0);
}

void calo_pedestal_calib::reset() {
  _reset_();
  return;
}

void calo_pedestal_calib::_reset_() {
  slot_index = 0xFFFF;
  channel = calo_constants::INVALID_BOARD_CHANNEL;
  offset_size = 0;
  for (size_t i = 0; i < calo_constants::MAX_NUMBER_OF_SAMPLES; i++) {
    offset[i] = 0xFFFFFFFF;
  }
  return;
}

int32_t calo_pedestal_calib::get_offset(const uint16_t cell_) const {
  DT_THROW_IF(cell_ >= offset_size, std::range_error,
              "Invalid cell index [" << cell_ << " >= " << offset_size << "]!");
  return offset[cell_];
}

void calo_pedestal_calib::set_offset(const uint16_t cell_, const int32_t adc_) {
  DT_THROW_IF(cell_ >= offset_size, std::range_error,
              "Invalid cell index [" << cell_ << " >= " << offset_size << "]!");
  offset[cell_] = adc_;
  return;
}

double calo_pedestal_calib::foffset(const uint16_t cell_) const {
  return get_offset(cell_) * calo_constants::adc_offset_unit();
}

void calo_pedestal_calib::print_offsets(std::ostream& out_, const uint32_t /* flags_*/) const {
  for (std::size_t i = 0; i < offset_size; i++) {
    out_ << i << ' ' << offset[i] << '\n';
  }
  out_ << std::endl << std::endl;
  return;
}

void calo_pedestal_calib::tree_dump(std::ostream& out_, const std::string& title_,
                                    const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << io::tag() << "Slot index  : " << slot_index << std::endl;

  out_ << indent_ << io::tag() << "Channel     : " << (int)channel << std::endl;

  out_ << indent_ << io::tag() << "Offset size : " << offset_size << std::endl;

  out_ << indent_ << io::inherit_last_tag(inherit_) << "Validity    : " << std::boolalpha
       << is_valid() << std::endl;

  return;
}

}  // namespace fecom
