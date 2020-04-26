/// \file mctools/g4/sensitive_hit_collection.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-07-02
 *
 * License:
 *
 * Description:
 *
 *   GEANT4 sensitive hit collection
 *
 * History:
 *
 */

#ifndef SNSIM_SENSITIVE_HIT_COLLECTION_H
#define SNSIM_SENSITIVE_HIT_COLLECTION_H 1

// Standard library:
#include <vector>

// Third party:
// - Geant4
#include <G4VHitsCollection.hh>

namespace snsim {

class sensitive_hit;

class sensitive_hit_collection : public G4VHitsCollection {
 public:
  typedef std::vector<sensitive_hit*> hit_collection_type;

  sensitive_hit_collection() = default;

  sensitive_hit_collection(const G4String& a_detector_name, const G4String& a_collection_name);

  virtual ~sensitive_hit_collection() = default;

  sensitive_hit_collection(const sensitive_hit_collection&) = default;

  sensitive_hit_collection& operator=(const sensitive_hit_collection&) = default;

  sensitive_hit_collection(sensitive_hit_collection&&) = default;

  sensitive_hit_collection& operator=(sensitive_hit_collection&&) = default;

  G4int operator==(const sensitive_hit_collection& right) const;

  const hit_collection_type& get_hits() const;

  hit_collection_type& grab_hits();

  // G4VHitsCollection Interface :

  virtual G4VHit* GetHit(size_t) const;

  virtual size_t GetSize() const;

 protected:
  hit_collection_type _hits;
};

}  // namespace snsim

#endif  // SNSIM_SENSITIVE_HIT_COLLECTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
