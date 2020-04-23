// sensitive_hit_collection.cc

// Ourselves:
#include <mctools/g4/sensitive_hit_collection.h>

// This project:
#include <mctools/g4/sensitive_hit.h>

namespace mctools {

  namespace g4 {

    sensitive_hit_collection::sensitive_hit_collection (G4String a_detector_name,
                                                        G4String a_collection_name)
      : G4VHitsCollection (a_detector_name, a_collection_name)
    {
      return;
    }

    G4int sensitive_hit_collection::operator==(const sensitive_hit_collection & right) const
    {
      return collectionName == right.collectionName;
    }

    const sensitive_hit_collection::hit_collection_type &
    sensitive_hit_collection::get_hits () const
    {
      return _hits;
    }

    sensitive_hit_collection::hit_collection_type &
    sensitive_hit_collection::grab_hits ()
    {
      return _hits;
    }

    G4VHit * sensitive_hit_collection::GetHit(size_t a_index) const
    {
      return _hits[a_index];
    }

    size_t sensitive_hit_collection::GetSize() const
    {
      return _hits.size ();
    }

  } // end of namespace g4

} // end of namespace mctools
