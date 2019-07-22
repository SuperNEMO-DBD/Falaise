#ifndef GT_EVENT_H
#define GT_EVENT_H 1

// Standard library
#include <iostream>
#include <list>
#include <map>
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/clhep.h>

namespace gt {

class event {
 public:
  /// Internal stucture holding calorimeter hit parameters
  struct calorimeter_hit {
    std::string label;
    geomtools::vector_3d position;
    double energy;
    double sigma_energy;
    double time;
    double sigma_time;

    friend bool operator<(const calorimeter_hit& lhs_, const calorimeter_hit& rhs_);

    friend std::ostream& operator<<(std::ostream& out_, const calorimeter_hit& hit_);
  };

  /// Typedef for calorimeter collection type
  typedef std::map<size_t, calorimeter_hit> calorimeter_collection_type;

  /// Reset
  void reset();

  /// Return a non-mutable reference to calorimeters
  const calorimeter_collection_type& get_calorimeters() const;

  /// Return a mutable reference to calorimeters
  calorimeter_collection_type& grab_calorimeters();

  friend std::ostream& operator<<(std::ostream& out_, const event& event_);

 private:
  calorimeter_collection_type _calos_;
};
}  // namespace gt

#endif  // GT_EVENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
