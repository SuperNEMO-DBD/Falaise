#ifndef FALAISE_LTTC_PLUGIN_DBSCAN_CLUSTERIZER_HH
#define FALAISE_LTTC_PLUGIN_DBSCAN_CLUSTERIZER_HH

// Standard library:
#include <cmath>
#include <limits>
#include <vector>
#include <set>
#include <map>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>

// This project:
// #include <lttc/legendre_transform_tools.hh>
#include <lttc/lttc_algo.hh>
  
namespace lttc {
  
  /// \brief DBSCAN clusterizer in Legendre Transform space
  class dbscan_clusterizer
  {
  public:

    /// \brief Input data
    struct input_data
    {
      const lttc_algo::map_type * trmap = nullptr; ///< Step-1 2D-histogram in theta-r space
    };
 
    /// \brief Configuration parameters
    struct config
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL; 
      double epsilon = 2.5;
      size_t min_pts = 3;
    };

    /// \brief Cluster
    struct cluster
    {
      int id = -1;
      int max_sbin = -1;
      double max_height = 0.0;
      std::set<int> sbins; ///< List of (sorted) bins
    };
       
    /// \brief Output data
    struct output_data
    {     
      void print(std::ostream & out_, const input_data & input_) const;
      void draw(std::ostream & out_, const input_data & input_) const;
      // Attributes:
      std::vector<cluster> clusters;    ///< List of clusters
      std::set<int>        unclustered; ///< List of unclustered (sorted) bins
    };
    
    dbscan_clusterizer(const input_data & input_,
                       const config & cfg_);

    bool is_visited(int sbin_index_) const;
 
    bool is_unvisited(int sbin_index_) const;

    void epsilon_neighbourhood(int sbin_index_,
                               std::set<int> & pts_) const;

    void expand_cluster(int sbin_index_,
                        const std::set<int> & pts_);

    void draw_step1(std::ostream & out_,
                    int sbin_index_,
                    const std::set<int> & pts_) const;

    void run(output_data & output_);
    
  public:
    
    const input_data * input = nullptr; ///< Handle to input data
    config cfg; ///< Configuration

    // Working data:
    size_t             nclusters = 0; ///< Current number of clusters
    std::vector<bool>  noisy; ///< Array of noisy flags for clusters
    std::vector<int>   sbins2clusters; ///< Sorted bins to clusters map
    std::map<int, int> bins2sbins; ///< Bins to sorted bins map
    
  };
    
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_DBSCAN_CLUSTERIZER_HH
