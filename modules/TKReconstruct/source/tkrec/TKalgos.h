#ifndef FALAISE_TKRECONSTRUCT_TKALGOS_H
#define FALAISE_TKRECONSTRUCT_TKALGOS_H

#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>

// TK headers
#include "tkrec/TKEvent.h"
#include "tkrec/TKgeom.h"
#include "tkrec/TKvisu.h"

namespace tkrec {

  /// Event tracking reconstruction mode
  enum class EventRecMode
    {
      undefined,
      full,   /// Full reconstruction
      simple, /// Simple reconstruction
      basic,  /// Basic reconstruction
      single, /// Single track reconstruction
      multi,  /// Multi-track reconstruction
      ML,     /// ML reconstruction
      ML3D    /// ML(3D) reconstruction
    };

  EventRecMode event_recmode_from_label(const std::string & label_);

  /// Configuration parameters for event tracking reconstruction
  struct TKEventRecConfig
  {
    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    EventRecMode mode = EventRecMode::undefined;
    bool save_sinograms = false; //< Extremely CPU/storage expensive (only for short runs and tests)
    std::string save_dir = "./Events_visu";
    bool visualization = false;

    // For full & simple mode:
    bool force_default_sigma_r = false; ///< Flag to force the default sigma r value for tracker hits
    double default_sigma_r = 2.0; ///< implicit unit: mm
    double chi_square_threshold = 5.0; ///< dimensionless
    
    // For basic mode:
    uint32_t basic_resolution = 250u;
    uint32_t basic_iterations = 2u;
     
    // For single mode:
    uint32_t single_resolution = 250u;
    uint32_t single_iterations = 2u;
    uint32_t single_threshold = 5.0;
  
    // For multi mode:
    uint32_t multi_resolution = 50u;
    uint32_t multi_iterations = 3u;
    double   multi_threshold = 0.75;
    uint32_t multi_start_no_segments = 5u;

    // For legendre  mode:
    double   legendre_distance_limit = 6.0; ///< mm
    double   legendre_limit_angle = 5.0 * M_PI / 180.0; ///< radian
    uint32_t legendre_resolution = 250u;
    uint32_t legendre_iterations = 2u;

    void parse(const datatools::properties & config_);
    
  };

  /// Main cluster/track reconstruction class.
  /// This class implements several algorithms.
  class TKalgos
  {
  public:

    TKalgos(const TKgeom & geom_);
    ~TKalgos();

    // Public interface:
    void set_event(TKEvent & event_);
    bool has_event() const;
    
    bool is_initialized() const;
    void initialize(const TKEventRecConfig & evrecconf_);
    void reset();
    void process(TKEvent & event_);

  private:
  
    // clustering :
	
    // Hough transform based clutering - finds a largest subgroup of given hits
    // such that is geometrically possible to have a single common line
    TKclusterHdl find_cluster(const std::vector<TKtrhitHdl>& tr_hits);
    // Legendre based clustering
    TKclusterHdl find_cluster_legendre(const std::vector<TKtrhitHdl>& hits);

    // note (9.4.): currently the main functions to use are the following:
    // 	reconstruct(); full reconstruction including trajectory 
    //					   builder and verteces extrapolation
    //	reconstruct_ML(); for quick track finding
    //	make_top_projection(int hits_option, int tracking_option);
    //	build_event(int tracking_option);

    // full reconstruction functions:
	
    // full reconstruction algorithm:
    //	1. different clusterings to safe failed events 
    //	2. maximum likelihood to obtain line tracks 
    //	3. ambiguity checker and solver
    //	4. trajectory builder from found segments
    //	5. trajectory extrapolator
		 
    void reconstruct_full(); // full reconstruction
    
    void reconstruct_simple(); // simpler quick algo for one track per side
		
    // line track reconstruction section	
	
    // basic reconstruction - no uncertainties, one candidate
    void reconstruct_track();
    void _reconstruct_track_from_hits_(const std::vector<TKtrhitHdl>& hits_);
		
    // with uncertainties, one candidate - recommended function
    void reconstruct_single();
    void _reconstruct_single_from_hits_(const std::vector<TKtrhitHdl>& hits_);
				
    // with uncertainties, multiple candidates
    void reconstruct_multi();

    // reconstruction based on maximum likelihood - currently best algorithm
    // a combination of basic clustering and maximum likelihood method
    // currently finds only solution per detector side
    void reconstruct_ML();
    void reconstruct_ML_3D();
	
    // trajectory builder
	
    void calculate_tr_hit_points();
    void build_trajectories();
    void extrapolate_trajectories();
	
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
	    
  private:

    void _process_full_();
    void _process_simple_();
    void _process_basic_();
    void _process_single_();
    void _process_multi_();
    void _process_ML_();
    void _process_ML3D_();

    const TKgeom & _geom_; ///< Geometry informations
    TKEventRecConfig _config_; ///< Configuration
    TKEvent * _event_ = nullptr; ///< Working event to be reconstructed
    std::unique_ptr<TKvisu> _visu_; ///< Visualisation engine
    
  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKALGOS_H
