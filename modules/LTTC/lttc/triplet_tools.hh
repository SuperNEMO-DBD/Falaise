#ifndef LTTCC_TRIPLET_TOOLS_HPP
#define LTTCC_TRIPLET_TOOLS_HPP

// Standard library:
#include <iostream>
#include <tuple>
#include <cmath>
#include <limits>
#include <vector>

// This project:
#include <lttcc/triplet.hpp>
#include <lttcc/tracker.hpp>
#include <lttcc/hit.hpp>
#include <lttcc/circle.hpp>

namespace lttcc {
  
  struct hits_ijk
  {
    hits_ijk() = default;
    hits_ijk(int i_, int j_, int k_);
    void clear();
    bool is_valid() const;
    bool operator==(const hits_ijk &) const;
    bool operator<(const hits_ijk &) const;
    friend std::ostream & operator<<(std::ostream & out_, const hits_ijk & ijk_);
    int i = -1;
    int j = -1;
    int k = -1;
  };
  
  struct triplet_data
  {
    void clear();
    std::vector<triplet>  triplets;
    std::vector<hits_ijk> owner_hits;
    std::vector<circle>   circles;
  };
 
  struct triplet_tools
  {
    triplet_tools(const tracker & sntracker_);

    void make_from_hits(const hit_collection & hits_,
                        const std::vector<int> & removed_hits_,
                        triplet_data & tdata_) const;
    
    const tracker & sntracker;
    double min_radius = 100.0;
    double nrsig = 3.0;
    
  };

 
} // namespace lttcc 

#endif // LTTCC_TRIPLET_TOOLS_HPP
