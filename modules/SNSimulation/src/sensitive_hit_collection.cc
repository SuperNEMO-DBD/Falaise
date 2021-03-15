// sensitive_hit_collection.cc

// Ourselves:
#include <snsim/sensitive_hit_collection.h>

// This project:
#include <snsim/sensitive_hit.h>

namespace snsim {

sensitive_hit_collection::sensitive_hit_collection(const G4String& a_detector_name,
                                                   const G4String& a_collection_name)
    : G4VHitsCollection(a_detector_name, a_collection_name) {}

auto sensitive_hit_collection::operator==(const sensitive_hit_collection& right) const -> G4int {
  return static_cast<G4int>(collectionName == right.collectionName);
}

auto sensitive_hit_collection::get_hits() const
    -> const sensitive_hit_collection::hit_collection_type& {
  return _hits;
}

auto sensitive_hit_collection::grab_hits() -> sensitive_hit_collection::hit_collection_type& {
  return _hits;
}

auto sensitive_hit_collection::GetHit(size_t a_index) const -> G4VHit* { return _hits[a_index]; }

auto sensitive_hit_collection::GetSize() const -> size_t { return _hits.size(); }

}  // namespace snsim
