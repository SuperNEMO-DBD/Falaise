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
#include <lttc/line2.hh>
#include <lttc/fitted_line2.hh>
#include <lttc/track_path_tools.hh>
#include <lttc/lttc_utils.hh>
#include <lttc/lttc_algo_circle.hh>

namespace lttc {

  /// \brief Legendre transform tracker clustering algorithm
  struct lttc_algo
  {
  public:
    static const size_t DEFAULT_TNBINS = 1000; ///< Default number of bins in theta space
    static const size_t DEFAULT_RNBINS = 1000; ///< Default number of bins in r space

    /// \brief Clustering mode
    enum mode_type
      {
       MODE_LINE   = 0, ///< Line clustering
       MODE_CIRCLE = 1  ///< Helix clustering
      };

    /// \brief Configuration specific to circular track identification
    struct circle_config
    {
      double epsilon    = 1e-5 * CLHEP::cm;
      double min_radius = 20.0 * CLHEP::cm;
      int max_cell_dist = 9;
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
      double step2_max_nlines      = 10;     ///< Process only the best 10 ranked candidate clusters
      double step2_delta_theta     = 1.5e-3 * CLHEP::radian; ///< (unit: radian)
      double step2_delta_r         = 0.2 * CLHEP::mm; 
      double step2_gauss_threshold = 0.05;   ///< Probability threshold for the Gauss kernel
      double step2_track_threshold = 3.0;    ///< Rejection threshold on number of tracks
      double step3_nsigma          = 3.0;    ///< 
      double step3_nsigma_outliers = 2.0;    ///< Distance threshold for identifying an outlier in the XY plane
      double step3_min_pvalue      = 0.05;   ///< Minimum pvalue for the cluster (not taking into account outliers)
      double step3_max_missing_hits = 2;     ///< Maximum number of missing hits along the cluster curve
      double step3_nsigma_z         = 3.0;   ///< Matching distance threshold for identifying good hits in the Z-slope analysis
      double step3_nsigma_z_outliers = 2.0;   ///< Distance threshold for identifying an outliers in the Z-slope analysis
      // double step3_node_match_tolerance = 3.0;  ///< Tolerance to detect equal clusters
      double step3_node_match_tolerance = 1.0 * CLHEP::mm; ///< Distance tolerance to detect equal clusters with corresponding nodes
      // int    step3_kink_max_cell_ortho_distance = 3;  ///< Cell grid distance tolerance for candidate two-arms clusters for a kinked track
      double step3_kink_max_cell_distance = 67.0 * CLHEP::mm;  ///< End-to-end distance tolerance for candidate two-arms clusters for a kinked track
      double step3_kink_z_min_pvalue = 0.05;  ///< Minimum pvalue for kinked tracks matching along the z axis
      double max_kink_xy_angle = 60.0 * CLHEP::degree;  ///< Maximum kink angle in the XY plane

      circle_config circ_cfg; ///< Specific configuration for circular track reconstruction
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
      double dr = 1.0 * CLHEP::mm;      ///< r step
      std::vector<double> bins;         ///< (2D-)array of bins with height
      std::vector<int>    sorted_bins;  ///< Sorted bins referenced by their index in the 'bins' 2D-array 
      
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
      line2 line_data; ///< Parametrization of the 'best' line in XY-plane
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
      fitted_point2 node; ///< Contact spot of the hit with the line of the cluster
      friend std::ostream & operator<<(std::ostream & out_, const hit_cluster_association_data & hca_);
    };
    typedef boost::multi_array<hit_cluster_association_data, 2> hit_cluster_association_array_type;
   
    static const size_t MIN_NUMBER_OF_HITS_IN_CLUSTER = 3;

    enum cluster_quality_flags
      {
       CQ_LOW_STAT  = 0x1, ///< Cluster with low number of hit cells
       CQ_CRIT_STAT = 0x2, ///< Cluster with less than 3 hit cells
       CQ_SHORT_ARM = 0x4  ///< Critical 2-hits clusters but in neigbour cells
      };
    
    static void print_hc(const hit_cluster_association_array_type & hc_,
                         std::ostream & out_,
                         const std::string & indent_ = "");
    
    /// \brief Step-3 working data
    struct step3_data
    { 
      void clear();
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      // Attributes
      hit_cluster_association_array_type HC; ///< Hit-to-cluster association array
    };

    /// Step 3
    void step3_run();

    /**********************
     * Final data product *
     **********************/

    typedef std::map<int, hit_cluster_association_data> hit_cluster_association_map_type;

    enum cluster_quality_rank
      {
       CQR_UNDEFINED = -1,
       CQR_GOOD = 0,
       CQR_POOR = 1,
       CQR_BAD  = 2
      };

    /// \brief Information about cluster quality 
    struct cluster_quality_data
    {
      double get_effective_pvalue() const;
      unsigned int get_effective_number_of_missing_hits() const;
      bool is_good() const
      {
        return rank == CQR_GOOD;
      }
      bool is_poor() const
      {
        return rank == CQR_POOR;
      }
      bool is_bad() const
      {
        return rank == CQR_BAD;
      }
      void clear();
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
      cluster_quality_rank rank = CQR_UNDEFINED;
    };

    /// \brief Hit cluster working data
    struct hit_cluster_data
    {
      /// \brief Some management flags
      enum flag_type
        {
         FLAG_DISCARDED = datatools::bit_mask::bit00
        };
      
      /// \brief This cluster is flagged as discarded because it is redundant or of bad quality
      bool is_discarded() const;
      /// \brief Check if the cluster has twin clusters which share exactly the same list of hits
      bool has_twins() const;
      /// \brief Check if the cluster is absolutely distinct from another cluster (no common hit)
      bool is_distinct_of(int icluster_) const;
      void print(std::ostream & out_, const std::string & indent_ = "") const;

      // Attributes:
      int           step3_id = -1; ///< Cluster ID from the original map
      int           id      = -1; ///< Unique cluster ID
      uint32_t      flags   = 0; ///< Special flags
      std::set<int> excluded_hits; ///< List of excluded/trimmed hits (by index) 
      
      int           side    = -1; ///< Tracker side 0/1
      bool          delayed = false; ///< Delayed cluster flag
      std::set<int> hits; ///< Hits addressed through their hit index
      cluster_quality_data quality; ///< Quality data
      hit_cluster_association_map_type hit_associations; ///< Hit association data
      std::set<int> twins; ///< Set of other clusters with the same list of hits but typically different line parameters (degenerated)
      std::set<int> superset_of; ///< Set of other clusters of which the list of hits is included in this cluster
      std::set<int> subset_of; ///< Set of other clusters of which the list of hits includes this cluster
      std::set<int> overlap_with; ///< Set of other clusters this cluster overlaps with
      std::vector<int> track_ordered_hits; ///< Array of hit indexes ordered along the track from the end hit #0 (first) to end hit #1 (last)
      int           end_hit_0 = -1; ///< First hit along the curve
      int           end_hit_1 = -1; ///< Last hit along the curve
      track_path_data track_path;
      fitted_line2   line_data; ///< Fitted line data
    };
    
    void cluster_add_hit(const int ihit_, hit_cluster_data & hc_);

    void compute_cluster_hits(hit_cluster_data & hc_);

    void compute_cluster_quality_fit(hit_cluster_data & hc_);

    void compute_cluster_quality_missing_hits(hit_cluster_data & hc_);

    void compute_cluster_quality_rank(hit_cluster_data & hc_);

    void compute_cluster_ordered_hits(hit_cluster_data & hc_);

    /// Build a candidate track path using the LT cluster's geometrical information
    /// (hit-cluster nodes, in XY plane)
    void compute_cluster_track_path(hit_cluster_data & hc_);

    // void compute_cluster_quality(const int icluster_);

    // void compute_cluster_association(const int icluster_,
    //                                  hit_cluster_data & hc_);
  
    // \brief Kinked track identified from two connected clusters
    struct kinked_track_data
    {
      enum from_type
        {
         FROM_NONE  = -1, ///< Not located kink with ecpedt to the track
         FROM_FIRST =  0, ///< Kink is on the first end side of the track
         FROM_LAST  =  1  ///< Kink is on the last end side of the track
        };
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      void draw(std::ostream & out_, int tag_ = 0) const;
      // Attributes:
      int       id = -1; ///< Unique ID
      int       first_cluster  = -1; ///< Index of the first cluster
      int       second_cluster = -1; ///< Index of the second cluster
      from_type first_cluster_from  = FROM_NONE;
      from_type second_cluster_from = FROM_NONE;
      point2    kink; ///< 2D-location of the kink (intercept)
      cell_id   kink_cell_id; ///< Cell ID where the kink is located (optional)
      point2    first_cluster_vertex; ///< 2D-location of the first cluster's end
      point2    second_cluster_vertex; ///< 2D-location of the second cluster's end
      double    z_first = datatools::invalid_real(); ///< Z-coordinate of the first cluster's end
      double    z_first_err = datatools::invalid_real(); ///< Error on Z-coordinate of the first cluster's end
      double    z_second = datatools::invalid_real(); ///< Z-coordinate of the second cluster's end
      double    z_second_err = datatools::invalid_real(); ///< Error on Z-coordinate of the second cluster's end
      double    z = datatools::invalid_real(); ///< Z-coordinate of the kink
      double    z_err = datatools::invalid_real(); ///< Error on the Z-coordinate of the kink
      double    z_pvalue = datatools::invalid_real(); ///< P-value
    };
    
    /// \brief Hit clustering working data
    struct hit_clustering_data
    {
      void print(std::ostream & out_, const std::string & indent_ = "") const;
      void draw(std::ostream & out_, const lttc_algo & algo_) const;
      void clear();
      bool is_unclustered_hit(int ihit_) const;
      // Attributes:
      std::vector<hit_cluster_data> clusters; ///< Array of clusters
      std::set<int> degenerated_clusters;     ///< List of degenerated clusters
      std::vector<kinked_track_data> kinked_tracks; ///< Array of candidate kinked tracks
      std::map<int, std::set<int>> hit_clustering_map; ///< Set of clusters associated to each hit
      std::set<int> unclustered_hits;         ///< List of unclustered hit by index
    };

    // The following methods are called in this order from the below 'create_hit_clustering' method:
 
    void clusters_build_ordered_hits();
 
    void clusters_detect_relationships();
 
    void clusters_detect_twins();

    void clusters_detect_curve_degenerated();

    void clusters_detect_contained();

    void clusters_detect_overlapping();

    void clusters_build_track_paths();

    void clusters_build_ranks();

    // Compute a list of clusters that have some spurious end hits that lie
    // too far in the XY plane or along the Z axis
    // with respect to the other hits positions:
    //
    // Example in the horizontal XY plane (Legendre transform fit in the horizontal XY-plane) :
    //
    //  Y
    // ^ 
    // : o <-- this one is too far  
    // :       from these ones
    // :             |
    // :       o     v        o
    // :        O O          O 0
    // :           o O     O o
    // :               o O 
    // :               
    // +----------------------------> X
    //
    // Example in the vertical SZ plane (curvilinar coordinate vs vertical linear fit) :
    //
    //   Z
    // ^ 
    // : o <-- this one is too far  
    // :       from these ones
    // :             |
    // :             v  
    // :        o O
    // :           o O
    // :             O o 
    // :               O o
    // +-----------------------> S
    //
    std::set<int> clusters_find_n_far_one();

    void clusters_search_short_arms();

    void clusters_search_kinked();
 
    void clusters_trim();
    
    void create_hit_clustering();

    void associate_unclustered_hits();

    // Not used
    void enrich_clusters();

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
   
    void _prepare_working_data_();
    
    void _build_output_data_();

  public:
    
    // Configuration
    const tracker * sntracker = nullptr;   ///< Tracker description
    config          cfg;                   ///< Configuration

    // Work:
    const input_data *  indata = nullptr;  ///< Current input data
    output_data      *  outdata = nullptr; ///< Current output data

    // Working data:
    lttc_algo_circle lac; ///< Specific algorithm for circular tracks
    std::map<cell_id, int> hit_cells;      ///< Map of hit cells
    // UNUSED : 
    std::vector<int>    removed_hits;      ///< Status of the hits (-1 : to be processed, >= 0 : already processed and clusterized)    
    size_t               loop_counter = 0;
    loop_data *          current_loop = nullptr;
    std::list<loop_data> loops;
    std::map<int,std::set<int>> clusters_to_be_removed_hits; ///< Hits to be removed from clusters

  }; // struct lttc_algo

} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LTTC_ALGO_HH
  
