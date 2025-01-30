#ifndef FALAISE_TKRECONSTRUCT_TKEVENT_H
#define FALAISE_TKRECONSTRUCT_TKEVENT_H

// Standard headers
#include <vector>
#include <iostream>

// ROOT headers
#include "TObject.h"

// TK headers
#include "tkrec/TKOMhit.h"
#include "tkrec/TKtrack.h"
#include "tkrec/TKtrhit.h"
#include "tkrec/TKcluster.h"
#include "tkrec/TKpoint.h"
#include "tkrec/TKtrajectory.h"

// - Bayeux:
#include <bayeux/datatools/logger.h>

// note (9.4.): currently the main functions to use are the following:

// 	reconstruct(bool save_sinograms); full reconstruction including trajectory 
//					   builder and verteces extrapolation
//	reconstruct_ML(bool save_sinograms); for quick track finding
//	make_top_projection(int hits_option, int tracking_option);
//	build_event(int tracking_option);

// In case you are looking for the implementation - all functions concerning 
// the tracking are implemented in a file "tracking_tools.cpp"

namespace tkrec {

  struct TKEventRecConfig
  {
    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    bool debug_mode = false;
    bool save_sinograms = false;
    double chi_square_threshold = 5.0;

    uint32_t track_from_hits_resolution = 250u;
    uint32_t track_from_hits_iterations = 2u;
   
    uint32_t multi_resolution = 50u;
    uint32_t multi_iterations = 3u;
    double   multi_threshold = 0.75;
    uint32_t multi_start_no_segments = 5u;
    
    uint32_t single_resolution = 250u;
    uint32_t single_iterations = 2u;
    uint32_t single_threshold = 5.0;

    double   legendre_distance_limit = 6.0;
    double   legendre_limit_angle = 5.0 * M_PI / 180.0;
    uint32_t legendre_resolution = 250u;
    uint32_t legendre_iterations = 2u;
  };

  class TKEvent
    : public TObject
  {
  private:
		
    int run_number = -1; ///< Run number
    int event_number = -1; ///< Event number

    std::vector<TKOMhitHdl> OM_hits;		
    std::vector<TKtrhitHdl> tr_hits;
    std::vector<TKtrackHdl> tracks;
    std::vector<TKclusterHdl> clusters;
    std::vector<TKtrajectoryHdl> trajectories;
		
  public:
	
    // basic functionality section:
	
    TKEvent() = default;
    TKEvent(int _run_number, int _event_number);
    virtual ~TKEvent() = default;

    void reset();

    // OM hits:
    std::vector<TKOMhitHdl> & get_OM_hits();
    std::vector<ConstTKOMhitHdl> get_OM_hits() const;
    TKOMhitHdl & get_OM_hit(int _i);
    ConstTKOMhitHdl get_OM_hit(int _i) const;
    void add_OM_hit(int _OM_num,  bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell);
    void add_OM_hit(int _SWCR[4], bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell);
    void add_OM_hit(const TKOMhitHdl & hit_);

    // Tracker hits:
    std::vector<TKtrhitHdl> & get_tr_hits();
    std::vector<ConstTKtrhitHdl> get_tr_hits() const;
    TKtrhitHdl & get_tr_hit(int _i);
    ConstTKtrhitHdl get_tr_hit(int _i) const;
    void add_tracker_hit(int _cell_num, int64_t _tsp[7]);
    void add_tracker_hit(int _SRL[3],   int64_t _tsp[7]);
    void add_tracker_hit(const TKtrhitHdl & hit_);

    // Tracks:
    std::vector<TKtrackHdl> & get_tracks();
    std::vector<ConstTKtrackHdl> get_tracks() const;
    std::vector<TKtrackHdl> get_all_tracks();
    std::vector<ConstTKtrackHdl> get_all_tracks() const;
    
    std::vector<TKclusterHdl> & get_clusters();		
    TKclusterHdl & get_cluster(int _i);
    ConstTKclusterHdl get_cluster(int _i) const;

    std::vector<TKtrajectoryHdl> & get_trajectories();
    std::vector<ConstTKtrajectoryHdl> get_trajectories() const;
    TKtrajectoryHdl & get_trajectory(int _i);
    ConstTKtrajectoryHdl get_trajectory(int _i) const;
	
    int get_run_number();
    int get_event_number();
    size_t get_no_tracks();
    size_t get_no_trajectories();
		
    void print(std::ostream & out_ = std::clog) const;
    void print_tracks(std::ostream & out_ = std::clog) const;
    void print_trajectories(std::ostream & out_ = std::clog) const;

		
	
    // drift model and plasma propagation section
	
    // associates tracker hits to OM hits and calculates hit radii
    // drift model: "Manchester" or "Betsy"
    // association_mode: "time" or "distance"
    // 	"distance": minimazes distance between OM and tracker hit
    // 	"time": minimazes time difference between OM and tracker hit
    void set_r(std::string drift_model = "Manchester",
	       std::string association_mode = "distance");
		
    // calls sigma_R, sigma_Z, h set functions for all tracker hits
    void set_sigma_R(); // currently unnecessary
    void set_h();
    void set_sigma_Z(); // currently unnecessary
	
    // tracker hit collection filtering
	
    std::vector<TKtrhitHdl> filter_side(const std::vector<TKtrhitHdl>& _hits, int side);
    std::vector<TKtrhitHdl> filter_usable(const std::vector<TKtrhitHdl>& _hits);
    std::vector<TKtrhitHdl> filter_unassociated(const std::vector<TKtrhitHdl>& _hits);
    std::vector<TKtrhitHdl> filter_unclustered(const std::vector<TKtrhitHdl>& _hits);
    std::vector<TKtrhitHdl> filter_distant(const std::vector<TKtrhitHdl>& _hits);
    std::vector<TKtrhitHdl> filter_close_hits(const std::vector<TKtrhitHdl>& _hits,
					      double phi,
					      double r,
					      double distance_limit);
		
    // clustering 
	
    // Hough transform based clutering - finds a largest subgroup of given hits
    // such that is geometrically possible to have a single common line
    TKclusterHdl find_cluster(std::vector<TKtrhitHdl>& tr_hits);
    // Legendre based clustering
    TKclusterHdl find_cluster_legendre(const std::vector<TKtrhitHdl>& hits,
				       const TKEventRecConfig & config_);
	
    // full reconstruction functions
	
    // full reconstruction algorithm:
    //	1. different clusterings to safe failed events 
    //	2. maximum likelihood to obtain line tracks 
    //	3. ambiguity checker and solver
    //	4. trajectory builder from found segments
    //	5. trajectory extrapolator
		 
    void reconstruct(const TKEventRecConfig & config_); // full reconstruction
    void reconstruct_simple(const TKEventRecConfig & config_); // simpler quick algo for one track per side
		
    // line track reconstruction section	
	
    // basic reconstruction - no uncertainties, one candidate
    void reconstruct_track(const TKEventRecConfig & config_);
    void reconstruct_track_from_hits(const std::vector<TKtrhitHdl>& hits,
				     const TKEventRecConfig & config_);
		
    // with uncertainties, one candidate - recommended function
    void reconstruct_single(const TKEventRecConfig & config_);
    void reconstruct_single_from_hits(const std::vector<TKtrhitHdl>& hits,
				      const TKEventRecConfig & config_);
				
    // with uncertainties, multiple candidates
    void reconstruct_multi(const TKEventRecConfig & config_);

    // reconstruction based on maximum likelihood - currently best algorithm
    // a combination of basic clustering and maximum likelihood method
    // currently finds only solution per detector side
    void reconstruct_ML(const TKEventRecConfig & config_);
    void reconstruct_ML_3D(const TKEventRecConfig & config_);
	
    // trajectory builder
	
    void calculate_tr_hit_points();
    void build_trajectories();
    void extrapolate_trajectories();
		
    // vizualization section
		
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
    //	0 - only tracks
    //	1 - tracks
    //	    reconstructed tracker hit avalanche origin points
    //	2 - trajectories
    //	    avalanche origin points
    //	3 - tracks
    //	    trajectories
    //	    avalanche origin points
				 
				 
    void make_top_projection(int hits_option = 3,
			     int tracking_option = 3) const;
    
    void build_event(int tracking_option = 3) const;	

    // tools for drawing certain mathematical functions 
    void hough_transform(const std::vector<TKtrhitHdl>& hits,
			 double phi_min,
			 double phi_max,
			 double R_min,
			 double R_max,
			 int ID);
    void draw_likelihood();
    void draw_likelihood_centred();
    void draw_sinusoids();

    ClassDef(TKEvent,1);
    
  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKEVENT_H
