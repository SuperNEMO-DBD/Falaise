#ifndef FALAISE_TKRECONSTRUCT_TKVISU_H
#define FALAISE_TKRECONSTRUCT_TKVISU_H


// Standard headers
#include <string>
#include <functional>

// Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/bit_mask.h>

#include "tkrec/TKgeom.h"
#include "tkrec/TKEvent.h"

namespace tkrec {

  class TKvisu
  {
  public:
    
    TKvisu(const TKgeom & geom_, const std::string & visu_dir_ = "");

    bool has_event() const;
    
    void set_event(const TKEvent & _event_);

    // vizualization section
    enum hit_flags {
      show_hit_used = datatools::bit_mask::bit00, ///< Show used hits for reconstruction (red)
      show_hit_unused = datatools::bit_mask::bit01, ///< Show unused hits for reconstruction (yellow)
      show_hit_associated = datatools::bit_mask::bit02, ///< Show associated hits to track (green)
      show_hit_unassociated_vert = datatools::bit_mask::bit03, ///< Show failed vertical position reconstruction but good drift radius (unassociated) (magenta)
      show_hit_associated_vert = datatools::bit_mask::bit04, ///< Show associated hits to track, good vertical position (green)
      show_hit_associated_novert = datatools::bit_mask::bit04, ///< Show associated hits to track, failed vertical position (teal)
      show_hit_all = show_hit_used | show_hit_unused | show_hit_associated | show_hit_unassociated_vert | show_hit_associated_vert | show_hit_associated_novert
    };
    
    // tracker hits options:
    // 	0 - no unused hits	
    //	    red	= used hits for reconstruction    
    //
    //	1 - red	= used hits for reconstruction
    //	    yellow 	= unused hits for recontstruction
    //
    //	2 - red	= used hits for reconstruction (unassociated)
    //	    yellow 	= unused hits for recontstruction 
    //	    green 	= associated hits to track
    //
    // 	3 - red	= used hits for reconstruction (unassociated + good vertical position)
    //	    yellow 	= unused hits for recontstruction
    //	    magenta 	= failed vertical position reconstruction but good drift radius (unassociated)
    //	    green 	= associated hits to track, good vertical position
    //	    teal	= associated hits to track, failed vertical position
				 
    // tracking options:
    enum tracking_flags  {
      show_tracking_tracks = datatools::bit_mask::bit00,  ///< Show tracks
      show_tracking_hit_avalanches = datatools::bit_mask::bit01,  ///< Show hit avalanches
      show_tracking_trajectories = datatools::bit_mask::bit02, ///< Show trajectories
      show_tracking_all = show_tracking_tracks | show_tracking_hit_avalanches | show_tracking_trajectories
    };

    //	0 - only tracks
    //	1 - tracks
    //	    reconstructed tracker hit avalanche origin points
    //	2 - trajectories
    //	    avalanche origin points
    //	3 - tracks
    //	    trajectories
    //	    avalanche origin points				 
				 
    void make_top_projection(const uint32_t hits_option_ = show_hit_all,
			     const uint32_t tracking_option_ = show_tracking_all) const;
    
    void build_event(const uint32_t tracking_option_ = show_tracking_all) const;	

    // // tools for drawing certain mathematical functions 
    // void hough_transform(const std::vector<TKtrhitHdl>& hits,
    // 			 double phi_min,
    // 			 double phi_max,
    // 			 double R_min,
    // 			 double R_max,
    // 			 int ID);
    // void draw_likelihood();
    // void draw_likelihood_centred();
    // void draw_sinusoids();

    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;

  private:

    const TKgeom & _geom_;
    const TKEvent * _event_ = nullptr;
    std::string _visu_dir_;
    
  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKVISU_H
