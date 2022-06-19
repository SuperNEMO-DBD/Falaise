#ifndef FALAISE_LTTC_PLUGIN_TRACK_HH
#define FALAISE_LTTC_PLUGIN_TRACK_HH

// Standard library:
#include <vector>
#include <set>
#include <iostream>

// Third party:
// - Bayeux/datatools :
#include <datatools/utils.h>

// This project:
#include <falaise/geometry/point.hh>

namespace lttc {

  /// \brief Track in XY-plane
  struct track2
  {
    enum flags_type
      {
       LINE   = 0x1,
       CIRCLE = 0x2,
       KINK   = 0x4
      };
    
    track2() = default;
 
    void draw(std::ostream & out_) const;
 
    void print(std::ostream & out_, const std::string & indent_ = "", bool inherit_ = false) const;
   
    int       id = -1;
    uint32_t  flags = 0;
    falaise::geometry::polyline2 pl;
    std::set<int> cuts;
    double    s_start = datatools::invalid_real();
    double    s_stop = datatools::invalid_real();
    falaise::geometry::point2    kink{datatools::invalid_real(), datatools::invalid_real()};
    double    s_kink = datatools::invalid_real();
   
  };

  typedef track2 track;
  typedef std::vector<track2> track2_collection;

  /// \brief Track in 3D
  struct track3
    : public track2
  {
    track3() = default;

    static void make_from_xy_track(const track2 & trk_, track3 & trk3_,
                                   double zstart_, double alpha_);

    void draw(std::ostream & out_) const;

    void make(falaise::geometry::polyline3 & p3_) const;

    void print(std::ostream & out_, const std::string & indent_ = "", bool inherit_ = false) const;
    
    // Atributes:
    std::vector<double> z;
    double z_kink = datatools::invalid_real();
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRACK_HH
