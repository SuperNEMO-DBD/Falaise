// fecom/calo_utils.cpp

// Ourselves:
#include <fecom/calo_utils.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

// static
const int calo_channel_id::INVALID_ID;

calo_channel_id::calo_channel_id() : board_id(INVALID_ID), slot_id(INVALID_ID) {}

calo_channel_id::calo_channel_id(const int bid_, const int sid_)
    : board_id(bid_ < 0 ? INVALID_ID : bid_), slot_id(sid_ < 0 ? INVALID_ID : sid_) {}

bool calo_channel_id::is_valid() const {
  if (board_id < 0) return false;
  if (slot_id < 0) return false;
  if (slot_id > 19) return false;
  return true;
}

// static
int calo_channel_id::compare(const calo_channel_id& id1_, const calo_channel_id& id2_) {
  DT_THROW_IF(!id1_.is_valid(), std::logic_error, "Invalid calorimeter channel ID!");
  DT_THROW_IF(!id2_.is_valid(), std::logic_error, "Invalid calorimeter channel ID!");
  if (id1_.board_id < id2_.board_id) return -1;
  if (id1_.board_id > id2_.board_id) return +1;
  if (id1_.slot_id < id2_.slot_id) return -1;
  if (id1_.slot_id > id2_.slot_id) return +1;
  return 0;
}

bool calo_channel_id::operator<(const calo_channel_id& id_) const {
  return compare(*this, id_) == -1;
}

bool calo_channel_id::operator==(const calo_channel_id& id_) const {
  return compare(*this, id_) == 0;
}

// friend
std::ostream& operator<<(std::ostream& out_, const calo_channel_id& id_) {
  out_ << '[' << id_.board_id << "." << id_.slot_id << ']';
  return out_;
}

}  // namespace fecom
