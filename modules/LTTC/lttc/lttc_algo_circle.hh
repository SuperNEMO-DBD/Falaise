#ifndef FALAISE_LTTC_PLUGIN_LTTC_ALGO_CIRCLE_HH
#define FALAISE_LTTC_PLUGIN_LTTC_ALGO_CIRCLE_HH

// Standard library:
#include <iostream>
#include <vector>
#include <limits>

// Third party:
#include <datatools/utils.h>
#include <mygsl/histogram.h>

// This project:
#include <falaise/geometry/point.hh>
#include <falaise/geometry/triplet2.hh>
#include <lttc/uhistogram.hh>

namespace lttc {
  
  struct lttc_algo;

  /// \brief Tracker clustering algorithm for circular tracks
  struct lttc_algo_circle
  {
  public:

    struct circle_data_type
    {
      falaise::geometry::point2 center;
      double radius = datatools::invalid_real();
    };
      
    struct hit_triplet_type
    {
      int i = -1;
      int j = -1;
      int k = -1;
    };
     
    struct hit_triplet_circle_type
    {
      int triplet_id = -1;
      falaise::geometry::triplet2 contacts;
      circle_data_type circle_data;

      void print(std::ostream & out_) const;
    };
 
    typedef std::vector<hit_triplet_type> hit_triplet_col;
    typedef std::vector<hit_triplet_circle_type> hit_triplet_circle_col;
    
    lttc_algo_circle() = default;

    void prepare();

    void do_steps();

    void do_step_1();
 
    void do_step_2();
 
    void do_step_3();
    
    void terminate();
   
    // Work:
    const lttc_algo * lttc = nullptr; ///< Parent algo
    hit_triplet_col hit_triplets; ///< Collection of triplets of input hits
    hit_triplet_circle_col hit_triplet_circles;
    hit_triplet_circle_col hit_triplet_blured_circles;
    uhistogram hr;
    uhistogram hx;
    uhistogram hy;
    
  }; // struct lttc_algo_circle

} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LTTC_ALGO_CIRCLE_HH
