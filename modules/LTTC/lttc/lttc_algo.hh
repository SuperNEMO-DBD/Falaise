#ifndef FALAISE_LTTC_PLUGIN_LTTC_ALGO_HH
#define FALAISE_LTTC_PLUGIN_LTTC_ALGO_HH

// Standard library:
#include <cmath>
#include <limits>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <list>

// Third party:
// - Boost:
#include <boost/multi_array.hpp>
// - Bayeux :
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// This project
#include <lttc/tracker_hit.hh>
#include <lttc/legendre_transform_tools.hh>
#include <lttc/sngeometry.hh>
#include <lttc/line.hh>

namespace lttc {

  /// \brief Legendre transform tracker clustering algorithm
  struct lttc_algo
  {
    
    const size_t TNBINS = 1000;
    const size_t RNBINS = 1000;
    const double TMIN   = 0.0;
    const double TMAX   = M_PI;

    enum mode_type
      {
       MODE_LINE   = 0,
       MODE_CIRCLE = 1
      };
    
    /// \brief Configuration
    struct config
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      mode_type mode              = MODE_LINE;
      size_t step1_ntbins         = 1000;
      size_t step1_nrbins         = 1000;
      double step1_track_threshold = 2.9;    ///< Rejection threshold on number of tracks
      double step2_max_nlines      = 6;      ///< Process only the best 6 ranked candidate clusters
      double step2_delta_theta     = 1.5e-3; ///< (unit: radian)
      double step2_delta_r         = 0.2 * CLHEP::mm; 
      double step2_gauss_threshold = 0.05;   ///< Probability threshold for the Gauss kernel
      double step2_track_threshold = 3.0;    ///< Rejection threshold on number of tracks
      double step3_nsigma          = 5.0;    ///< 
    };

    lttc_algo(const tracker & sntracker_,
              const config & cfg_);
    
    ~lttc_algo();
    
    void step1_run();

    int compute_neighbour_distance(int ifrom_, int jfrom_, int i_, int j_);

    /// Legendre transform working 2D-histogram
    ///
    /// nbins = 6 * 5 = 30
    ///
    ///         |  0  1  2  3  4  5 < [t_index, nt=6]
    ///       --+-------------------+
    ///       0 |  0  1  2  3  4  5 |
    ///       1 |  6  7  8  9 10 11 |
    ///       2 | 12 13 14 15 16 17 |
    ///       3 | 18 19 20 21 22 23 |
    ///       4 | 24 25 26 27 28 29 |
    ///       ^ +-------------------+
    /// [r_index, nr=5]
    ///
    struct map_type
    {
      ~map_type();
      
      int r_to_index(double r_) const;

      int t_to_index(double t_) const;

      double index_to_t(int it_) const;

      double index_to_r(int ir_) const;

      int t_r_indexes_to_tr_bin_index(int t_index_, int r_index_) const;

      bool tr_bin_index_to_t_r_indexes(int bin_index_,
                                       int & t_index_, int & r_index_) const;

      struct fill_bin_ctrl
      {
        static datatools::logger::priority logging;
      };

      /// @param it0_   theta index
      /// @param ir0_   r index
      /// @param gauss_threshold_ gauss kernel threshold
      void fill_bin(int it0_, int ir0_, double gauss_threshold_ = -1.0);

      void apply_threshold(double t_ = 3.5);

      void build_sorted_bins();
 
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      void draw_bins(std::ostream & out_) const;

      void draw_sorted_bins(std::ostream & out_,
                            double threshold_ = 0.0) const;

      void clear();
      
      // Histogram:
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      std::string id;
      double tmin = 0.0;  ///< Theta min value
      double tmax = M_PI; ///< Theta max value
      double rmin =  std::numeric_limits<double>::infinity(); ///< r min value
      double rmax = -std::numeric_limits<double>::infinity(); ///< r max value
      size_t nt = 0;    ///< Number of theta bins
      size_t nr = 0;    ///< Number of r bins
      double dt = 0.01; ///< Theta step
      double dr = 1.0;  ///< r step
      std::vector<double> bins;        ///< (2D-)array of bins with height
      std::vector<int>    sorted_bins; ///< Sorted bins referenced by their index in the 'bins' 2D-array 
      
    }; // struct map_type

      
    /// \brief Cluster of bins in Legendre t-r space
    struct cluster
    {
      ~cluster();
      void add_bin(int ibin_, int tbin_, int rbin_, double hbin_);
      size_t size() const;
      bool empty() const;
      void compute_ratios();
      bool has_bin(int ibin_) const;
      void draw(std::ostream & out_, const map_type & trmap_) const;
      void print(std::ostream & out_,
                 const std::string & indent_) const;

      /// Check if a cluster significantly overlaps another one (comparing fiducial t-r rectangles)
      bool overlap(const cluster & other_) const;
      
      void clear();

      int    id = -1; ///< Cluster ID
      int    peak_bin_index = -1; ///< Bin index of the max peak bin
      double peak_height = std::numeric_limits<double>::quiet_NaN();
      int    peak_tbin = -1; ///< Theta index of max peak bin
      int    peak_rbin = -1; ///< R index of max peak bin
      double threshold_height = std::numeric_limits<double>::quiet_NaN();
      int    itmin = -1; ///< Min theta index
      int    itmax = -1; ///< Max theta index
      int    irmin = -1; ///< Min r index
      int    irmax = -1; ///< Max r index
      double t_step = std::numeric_limits<double>::quiet_NaN();  
      double t_min = std::numeric_limits<double>::quiet_NaN(); 
      double t_max = std::numeric_limits<double>::quiet_NaN(); 
      double r_step = std::numeric_limits<double>::quiet_NaN();  
      double r_min = std::numeric_limits<double>::quiet_NaN(); 
      double r_max = std::numeric_limits<double>::quiet_NaN(); 
      size_t nbins_over_threshold = 0;
      double hits_per_bin_ratio = std::numeric_limits<double>::quiet_NaN(); 
      double t_mean = std::numeric_limits<double>::quiet_NaN(); 
      double r_mean = std::numeric_limits<double>::quiet_NaN(); 
      double t_err  = std::numeric_limits<double>::quiet_NaN(); 
      double r_err  = std::numeric_limits<double>::quiet_NaN(); 
      std::map<int, double> bins; ///< Bins in this cluster with their height
      line   line_data;
    };

    /// Compare two clusters with respect to their 'hits per bin' ratios
    static bool compare_by_ratio(const cluster & c1_, const cluster & c2_); 

    /// \brief Legendre t-r space clustering result data
    struct clustering_data
    {
      ~clustering_data();
      
      void clear();

      void draw(std::ostream & out_, const map_type & trmap_) const;

      void print(std::ostream & out_,
                 const std::string & indent_,
                 const map_type & trmap_) const;

      std::vector<cluster> clusters;    ///< List of clusters
      std::set<int>        unclustered; ///< List of unclustered hits
      
    };

    /// \brief Clusterizer in Legendre t-r space
    struct cluster_finder
    {
      cluster_finder(const map_type & trmap_);
      
      ~cluster_finder();
      
      void find(clustering_data & clustering_);

      bool             debug = false;  
      const map_type * trmap = nullptr; ///< Reference to the t-r 2D-histogram
      
    }; // struct cluster_finder

    /// \brief t-r space clustering data
    struct trc_data
    {   
      void clear();

      void print(std::ostream & out_, const std::string & indent_ = "") const;

      map_type        trmap;      ///< 2D-histogram in t-r space
      clustering_data clustering; ///< Clustering result in t-r space
      
    };

    /// \brief Step-1 working data
    struct step1_data : public trc_data
    {
    };
    
    struct trc_ref
    {
      bool operator<(const trc_ref & tr_) const;

      const map_type * ptrmap = nullptr;   ///< Ref to a 2D-histogram in t-r space
      const cluster  * pcluster = nullptr; ///< Ref cluster in t-r space
      
    };

    /// \brief Step-2 working data
    struct step2_data
    {
      void clear();
      
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      std::vector<trc_data> trmaps;  ///< Step-2 2D-histograms in t-r space
      std::vector<trc_ref> clusters; ///< All clusters in t-r space
      
    };

    /// Check cluster overlap
    static bool cluster_overlap(const cluster & cl1_, const map_type & trmap1_,
                                const cluster & cl2_, const map_type & trmap2_);
   
    void step2_run();

    void step3_run();

    typedef boost::multi_array<int, 2> int_array2_type;

    static void print_hc(const int_array2_type & hc_,
                         std::ostream & out_,
                         const std::string & indent_ = "");
    
    /// \brief Step-3 working data
    struct step3_data
    { 
      void clear();

      void print(std::ostream & out_, const std::string & indent_ = "") const;

      int_array2_type HC; ///< Hit-to-cluster association array
       
    };

    /// \brief Final cluster data
    struct cluster_data
    {
      trc_ref       ref;
      std::set<int> hits;
    };

    /// \brief Hit cluster working data
    struct hit_cluster_data
    {
     
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      void add_hit(int ihit_, const lttc_algo & algo_);
      
      int           id = -1;
      uint32_t      flags = 0;
      std::set<int> hits;
      int           far_hit_1 = -1;
      int           far_hit_2 = -1;
      line          line_data;
      
    };

    /// \brief Hit clustering working data
    struct hit_clustering_data
    {
      enum cluster_quality_flags
        {
         CQ_LOW_STAT  = 0x1,
         CQ_CRIT_STAT = 0x2      
        };

      std::vector<hit_cluster_data> clusters; ///< Array of clusters
      std::set<int> unclustered_hits;         ///< List of unclustered hit by index
     
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      
      void draw(std::ostream & out_, const lttc_algo & algo_) const;

    };
    
    void update_hit_clustering();

    void enrich_clusters();

    void associate_unclustered_hits();

    struct loop_data
    {
      void clear();
      
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      int           loop_id = -1; ///< Loop ID (counter)
      step1_data    step1;        ///< Step-1 data
      step2_data    step2;        ///< Step-2 data
      step3_data    step3;        ///< Step-3 data
    };

    bool can_process_hit(int ihit_) const;

    void init_loops();

    void run_loop();

    void terminate_loops();
     
    void print(std::ostream & out_, const std::string & indent_ = "") const;
    
    /// \brief input data
    struct input_data
    {
      tracker_hit_collection hits; ///< Collection of tracker hits to be clusterized
    };
    
    /// \brief output data
    struct output_data
    {
      hit_clustering_data hit_clustering;
    };

    void process(const input_data & indata_, output_data & outdata_);
    
  private:
    
    void _clear_processing_data_();

    void _set_input_data_(const input_data & indata_);
    
    void _build_output_data_();

  public:
    
    // Configuration
    const tracker * sntracker = nullptr;    ///< Tracker description
    config cfg;          ///< Configuration

    // Work:
    const input_data *  indata = nullptr; ///< Current input data
    std::vector<int>    removed_hits; ///< Status of the hits (-1 : to be processed, >= 0 : already processed and clusterized)
    output_data      *  outdata = nullptr;     ///< Current output data
    
    size_t               loop_counter = 0;
    loop_data *          current_loop = nullptr;
    std::list<loop_data> loops;
    // hit_clustering_data  hit_clustering;
    
  }; // struct lttc_algo

} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LTTC_ALGO_HH
  
