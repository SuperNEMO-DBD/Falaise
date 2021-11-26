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
    
    static const size_t DEFAULT_TNBINS = 1000;
    static const size_t DEFAULT_RNBINS = 1000;

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
      bool draw                   = false;
      std::string draw_prefix     = "lttc_algo-";
      int  loop_draw_nmin         = 0;
      int  loop_draw_nmax         = -1;
      size_t step1_ntbins         = DEFAULT_TNBINS;
      size_t step1_nrbins         = DEFAULT_RNBINS;
      double step1_track_threshold = 2.9;    ///< Rejection threshold on minimal number of tracks in a step1 map's bin
      double step2_max_nlines      = 6;      ///< Process only the best 6 ranked candidate clusters
      double step2_delta_theta     = 1.5e-3 * CLHEP::radian; ///< (unit: radian)
      double step2_delta_r         = 0.2 * CLHEP::mm; 
      double step2_gauss_threshold = 0.05;   ///< Probability threshold for the Gauss kernel
      double step2_track_threshold = 3.0;    ///< Rejection threshold on number of tracks
      double step3_nsigma          = 3.0;    ///< 
      double step3_nsigma_outliers = 2.0;    ///< 
    };

    lttc_algo(const tracker & sntracker_, const config & cfg_);
    
    ~lttc_algo();

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

      static constexpr double no_gauss_kernel{-1.0};

      /// \brief Fill a given bin in the theta-r histogram, optionnaly applying a Gauss kernel
      ///
      /// @param it0_ theta index
      /// @param ir0_ r index
      /// @param gauss_threshold_ gauss kernel threshold (<= 0.0 : no gauss kernel is applied)
      void fill_bin(int it0_, int ir0_, double gauss_threshold_ = no_gauss_kernel, double weight_ = 1.0);

      /// \brief Zeroes all bin with contents under a given threshold
      void apply_threshold(double t_ = 2.9);

      void clear();

      void build_sorted_bins();
 
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      void draw_bins(std::ostream & out_) const;

      void draw_sorted_bins(std::ostream & out_,
                            double threshold_ = 0.0) const;
      
      // Histogram:
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      std::string id;
      double tmin = 0.0 * CLHEP::radian;  ///< Theta min value
      double tmax = M_PI * CLHEP::radian; ///< Theta max value
      double rmin =  std::numeric_limits<double>::infinity(); ///< r min value
      double rmax = -std::numeric_limits<double>::infinity(); ///< r max value
      size_t nt = 0;    ///< Number of theta bins
      size_t nr = 0;    ///< Number of r bins
      double dt = 0.01 * CLHEP::radian; ///< Theta step
      double dr = 1.0 * CLHEP::mm   ;  ///< r step
      std::vector<double> bins;        ///< (2D-)array of bins with height
      std::vector<int>    sorted_bins; ///< Sorted bins referenced by their index in the 'bins' 2D-array 
      
    }; // struct map_type

      
    /// \brief Cluster of bins in Legendre t-r space
    struct cluster
    {
      void add_bin(int ibin_, int tbin_, int rbin_, double hbin_);
      size_t size() const;
      bool empty() const;
      void compute_ratios();
      bool has_bin(int ibin_) const;
      void draw(std::ostream & out_, const map_type & trmap_) const;
      void print(std::ostream & out_, const std::string & indent_) const;
      /// Check if a cluster significantly overlaps another one (comparing fiducial t-r rectangles)
      bool overlap(const cluster & other_) const; 
      void clear();
      // Attributes:
      int    id = -1; ///< Cluster ID
      int    peak_bin_index = -1; ///< Bin index of the max peak bin
      double peak_height = std::numeric_limits<double>::quiet_NaN(); ///< Peak height
      int    peak_tbin = -1; ///< Theta index of max peak bin
      int    peak_rbin = -1; ///< R index of max peak bin
      double threshold_height = std::numeric_limits<double>::quiet_NaN();
      int    itmin  = -1; ///< Min theta index
      int    itmax  = -1; ///< Max theta index
      int    irmin  = -1; ///< Min r index
      int    irmax  = -1; ///< Max r index
      double t_step = std::numeric_limits<double>::quiet_NaN(); ///< Theta step width 
      double t_min  = std::numeric_limits<double>::quiet_NaN(); ///< Min theta
      double t_max  = std::numeric_limits<double>::quiet_NaN(); ///< Max theta
      double r_step = std::numeric_limits<double>::quiet_NaN(); ///< r step width
      double r_min  = std::numeric_limits<double>::quiet_NaN(); ///< Min r
      double r_max  = std::numeric_limits<double>::quiet_NaN(); ///< max r
      size_t nbins_over_threshold = 0;
      double hits_per_bin_ratio   = std::numeric_limits<double>::quiet_NaN(); 
      double t_mean = std::numeric_limits<double>::quiet_NaN(); 
      double r_mean = std::numeric_limits<double>::quiet_NaN(); 
      double t_err  = std::numeric_limits<double>::quiet_NaN(); 
      double r_err  = std::numeric_limits<double>::quiet_NaN(); 
      std::map<int, double> bins; ///< Bins in this cluster with their height
      line line_data; ///< Parametrization of the 'best' line
    };

    /// Compare two clusters with respect to their 'hits per bin' ratios
    static bool compare_by_ratio(const cluster & c1_, const cluster & c2_); 

    /// \brief Legendre t-r space clustering result data
    struct clustering_data
    {
      void clear();
      void draw(std::ostream & out_, const map_type & trmap_) const;
      void print(std::ostream & out_,
                 const std::string & indent_,
                 const map_type & trmap_) const;
      // Attributes:
      std::vector<cluster> clusters;    ///< List of clusters
      std::set<int>        unclustered; ///< List of unclustered hits    
    };

    /// \brief Clusterizer in Legendre t-r space
    struct cluster_finder
    {
      cluster_finder(const map_type & trmap_,
                     datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
      void find(clustering_data & clustering_);
      // Attributes:
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL; 
      const map_type * trmap = nullptr; ///< Reference to the t-r 2D-histogram    
    };

    /// \brief t-r space clustering data
    struct trc_data
    {
      /// \brief Clear the theta-r clustering data
      void clear();
      /// \brief Print the theta-r clustering data
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes:
      map_type        trmap;      ///< 2D-histogram in t-r space
      clustering_data clustering; ///< Clustering result in t-r space
    };

    /**********************
     *       Step 1       *
     **********************/

    /// \brief Step-1 working data
    struct step1_data : public trc_data
    {
    };

    /// Step 1 run
    void step1_run();

    /**********************
     *       Step 2       *
     **********************/

    /// \brief Theta-r clustering reference data 
    struct trc_ref 
    {
      /// Compare by hits_per_bin_ratio
      bool operator<(const trc_ref & tr_) const;
      // Attributes:
      const map_type * ptrmap   = nullptr; ///< Reference to a 2D-histogram in t-r space
      const cluster  * pcluster = nullptr; ///< Reference cluster in t-r space
    };

    /// \brief Step-2 working data
    struct step2_data
    {
      void clear();
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      
      // Attributes:
      std::vector<trc_data> trmaps;   ///< Step-2 2D-histograms in t-r space
      std::vector<trc_ref>  clusters; ///< All clusters in t-r space    
    };

    /// Check cluster overlap
    static bool cluster_overlap(const cluster & cl1_, const map_type & trmap1_,
                                const cluster & cl2_, const map_type & trmap2_);

    /// Step 2 run
    void step2_run();

    /**********************
     *       Step 3       *
     **********************/

    enum hit_cluster_association_status_type
      {
       HCAS_NOT_VALID       = -2,
       HCAS_NOT_ASSOCIABLE  = -1,
       HCAS_NOT_ASSOCIATED  =  0,
       HCAS_ASSOCIATED      =  1
      };

    static std::string to_string(hit_cluster_association_status_type hcas_);
    
    struct hit_cluster_association_data
    {
      hit_cluster_association_status_type association_status = HCAS_NOT_ASSOCIATED; ///< Association status
      double distance = std::numeric_limits<double>::quiet_NaN(); ///< Distance to the cluster's line (XY-plane)
      double residual = std::numeric_limits<double>::quiet_NaN(); ///< Normalized residual (for heteroscedasticity check later)
      double chi2 = std::numeric_limits<double>::quiet_NaN();     ///< Pearson chi2-term
      fitted_point node; ///< Contact spot of the hit with the line of the cluster
      friend std::ostream & operator<<(std::ostream & out_, const hit_cluster_association_data & hca_);
    };
    typedef boost::multi_array<hit_cluster_association_data, 2> hit_cluster_association_array_type;

    enum missing_cell_reason_type
      {
       MCR_UNDEFINED   = 0,  ///< Undefined reason
       MCR_NO_TRIGGER  = 1,  ///< No trigger as expected
       MCR_DEAD_CELL   = 2,  ///< Dead cell
       MCR_BUSY_CELL   = 3,  ///< Busy cell (dead time)
       MCR_OTHER_TRACK = 4   ///< Cell hit by another track
      };

    /// \brief Information about a missing hit 
    struct cluster_missing_hit_data
    {
      cell_id cid;
      missing_cell_reason_type why;
      friend std::ostream & operator<<(std::ostream & out_, const cluster_missing_hit_data & cmh_);
    };

    static std::string to_string(missing_cell_reason_type why_);
    
    /// \brief Information about cluster quality 
    struct cluster_quality_data
    {
      double get_effective_pvalue() const;
      unsigned int get_effective_number_of_missing_hits() const;
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes:
      int    nhits   = 0;
      double sumChi2 = std::numeric_limits<double>::quiet_NaN();
      int    ndof    = 0;
      double pvalue  = std::numeric_limits<double>::quiet_NaN();
      double noOutliersSumChi2 = std::numeric_limits<double>::quiet_NaN();
      int    noOutliersNdof    = 0;
      double noOutliersPvalue  = std::numeric_limits<double>::quiet_NaN();
      std::set<int> hits;
      std::set<int> outliers;
      std::vector<cluster_missing_hit_data> missing_hits;
    };
    typedef std::vector<cluster_quality_data> cluster_quality_array_type;
 
    static void print_hc(const hit_cluster_association_array_type & hc_,
                         std::ostream & out_,
                         const std::string & indent_ = "");

    static void print_cq(const cluster_quality_array_type & cq_,
                         std::ostream & out_,
                         const std::string & indent_ = "");
    
    /// \brief Step-3 working data
    struct step3_data
    { 
      void clear();
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes
      hit_cluster_association_array_type HC; ///< Hit-to-cluster association array
      cluster_quality_array_type         CQ; ///< Cluster quality
    };

    /// Step 3
    void step3_run();

    /**********************
     * Final data product *
     **********************/

    typedef std::map<int, hit_cluster_association_data> hit_cluster_association_map_type;

    struct track_path_vertex
    {
      int hit; ///< Hit ID
      fitted_point node; ///< Contact point of the track with the hit cell
    };

    struct track_path_data
    {
      void draw(std::ostream & out_, int tag_ = 0) const; 
      std::vector<track_path_vertex> vertexes;
    };
       
    /// \brief Hit cluster working data
    struct hit_cluster_data
    {
      enum flag_type
        {
         FLAG_DISCARDED   = datatools::bit_mask::bit00,
         FLAG_DEGENERATED = datatools::bit_mask::bit01
        };

      /// Build a candidate track path using the LT cluster geometrical information
      /// (hit-cluster nodes)
      void build_track_path(track_path_data & track_path_) const;
      void add_hit(int ihit_, const lttc_algo & algo_);
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes:
      int           id      = -1;
      int           side    = -1;
      bool          delayed = false;
      uint32_t      flags   = 0;
      std::set<int> hits; ///< Hits addressed through their hit index
      cluster_quality_data quality;
      hit_cluster_association_map_type hit_associations;
      int           end_hit_1 = -1;
      int           end_hit_2 = -1;
      fitted_line   line_data;
      std::set<int> twins; ///< Set of other clusters with the same liste of hits but different line params (degenerated)
      std::vector<int> track_ordered_hits; ///< Array of hit indexes ordered along the track from the end hit #1 to end hit #2
      
    };
    
    /// \brief Hit clustering working data
    struct hit_clustering_data
    {
      enum cluster_quality_flags
        {
         CQ_LOW_STAT  = 0x1,
         CQ_CRIT_STAT = 0x2      
        };
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      void draw(std::ostream & out_, const lttc_algo & algo_) const;
      void clear();
      // Attributes:
      std::vector<hit_cluster_data> clusters; ///< Array of clusters
      std::set<int> degenerated_clusters;     ///< List of degenerated clusters
      std::set<int> unclustered_hits;         ///< List of unclustered hit by index     
    };
    
    void compute_cluster_missing_hits(hit_cluster_data & hit_cluster_);

    void detect_degenerated_clusters();

    void build_cluster_ordered_hits();
    
    void update_hit_clustering();

    void enrich_clusters();

    void associate_unclustered_hits();

    struct loop_data
    {
      void clear();
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes:
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
      hit_clustering_data hit_clustering; ///< Clustering
    };

    void process(const input_data & indata_, output_data & outdata_);
    
  private:
    
    void _clear_processing_data_();

    void _set_input_data_(const input_data & indata_);
    
    void _build_output_data_();

  public:
    
    // Configuration
    const tracker * sntracker = nullptr;   ///< Tracker description
    config          cfg;                   ///< Configuration

    // Work:
    const input_data *  indata = nullptr;  ///< Current input data
    std::map<cell_id, int> hit_cells;      ///< Map of hit cells
    std::vector<int>    removed_hits;      ///< Status of the hits (-1 : to be processed, >= 0 : already processed and clusterized)
    output_data      *  outdata = nullptr; ///< Current output data
    
    size_t               loop_counter = 0;
    loop_data *          current_loop = nullptr;
    std::list<loop_data> loops;
    
  }; // struct lttc_algo

} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LTTC_ALGO_HH
  
