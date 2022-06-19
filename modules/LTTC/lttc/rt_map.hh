#ifndef FALAISE_LTTC_PLUGIN_RT_MAP_HH
#define FALAISE_LTTC_PLUGIN_RT_MAP_HH

// Standard library:
#include <cmath>
#include <limits>
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// This project:
  
namespace lttc {

    /// Legendre transform working 2D-histogram : radius-theta ("rt") map
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
    struct rt_map
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
      
    }; 

} // namespace lttc


#endif // FALAISE_LTTC_PLUGIN_RT_MAP_HH
