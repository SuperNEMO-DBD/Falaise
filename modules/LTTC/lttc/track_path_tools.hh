#ifndef FALAISE_LTTC_PLUGIN_TRACK_PATH_TOOLS_HH
#define FALAISE_LTTC_PLUGIN_TRACK_PATH_TOOLS_HH

// Standard library:
#include <iostream>
#include <set>
#include <map>
#include <tuple>

// Third party:
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <falaise/geometry/i_curve2.hh>
#include <falaise/geometry/point.hh>
#include <falaise/geometry/line2.hh>
#include <falaise/geometry/triplet2.hh>
#include <falaise/geometry/fitted_point2.hh>

namespace lttc {

  /// \brief Track path vertex
  struct track_path_vertex
  {
    int hit_index = -1;      ///< Hit index
    falaise::geometry::fitted_point2 node; ///< Contact point of the track with the hit cell (XY-plane)
    double sxy   = datatools::invalid_real(); ///< Curvilinear abscissa on the horizontal  XY plane
    double z     = datatools::invalid_real(); ///< Vertical position
    double z_err = datatools::invalid_real(); ///< Error on vertical position
  };

  enum track_path_z_quality
    {
     TPZQ_UNDEFINED = -1,
     TPZQ_GOOD      =  0, ///< Good Z-slope 
     TPZQ_OUTLIER   =  1, ///< Outlier Z-slope (but acceptable)
     TPZQ_BAD       =  2  ///< Bad Z-slope (fake Z-association)
    };

  struct track_path_z_residual
  {
    track_path_z_quality quality = TPZQ_UNDEFINED;
    double distance = datatools::invalid_real();
  };

  enum track_path_z_pattern
    {
     TPZP_UNDEFINED = -1,
     TPZP_NORMAL    =  0, ///< Normal slope pattern
     TPZP_KINK      =  1, ///< Kinked slope pattern
     TPZP_BREAK     =  2, ///< Break (two-tracks) slope pattern
     TPZP_STRANGE   =  3  ///< Strange abnormal slope pattern
    };

  /// \brief Track path associated to a cluster
  struct track_path_data
  {
    /// \brief Number of point along the track
    size_t size() const;
    /// \brief First point in the XY-plane along the track
    const falaise::geometry::fitted_point2 & first() const;
    /// \brief Last point in the XY-plane along the track
    const falaise::geometry::fitted_point2 & last() const;
    /// \brief Vertex by index in the 3D-space along the track
    std::tuple<falaise::geometry::point3, double> vertex3(int i_) const;
    /// \brief First point in the 3D-space along the track
    std::tuple<falaise::geometry::point3, double> first3() const;
    /// \brief Last point in the 3D-space along the track
    std::tuple<falaise::geometry::point3, double> last3() const;
    /// Compute the Z statistics given a threshold (in sigma unit, default: 2.0)
    /// for the identification of outlier track path segments
    // void compute_z_statistics(double z_bad_threshold_, double z_outlier_threshold_);
    /// Compute the Z statistics
    void compute_z_statistics();
    void draw(std::ostream & out_, int tag_ = 0) const;
    void print(std::ostream & out_, const std::string & indent_ = "") const;
    double get_z_pvalue() const;
    double get_effective_z_pvalue() const;
    void compute_z_and_error(double s_, double & z_, double & z_err_) const;
    
    // Attributes:
    int cluster_index = -1; ///< Parent cluster index
    std::vector<track_path_vertex> vertexes; ///< List of vertices (XY x Z informations) 

    // SZ-fit statistics:
    double z_c0     = datatools::invalid_real(); ///< Intercept in the (sxy-z) plane
    double z_c1     = datatools::invalid_real(); ///< Slope in the (sxy-z) plane
    double z_cov00  = datatools::invalid_real(); ///< Covariance matrix element
    double z_cov01  = datatools::invalid_real(); ///< Covariance matrix element
    double z_cov11  = datatools::invalid_real(); ///< Covariance matrix element
    size_t z_ndof   = 0; ///< Number of degrees of freedom associated to the Z-fit
    double z_chi2   = datatools::invalid_real(); ///< Chi square value associated to the Z-fit
    double z_pvalue = datatools::invalid_real(); ///< P-value associated to the Z-fit
    std::vector<double> z_residuals; ///< Residuals
    std::set<int>       z_trimmed;   ///< Set of trimmed vertices per index
  };

  /// \brief Intercept bit
  enum track_path_intercept_bit
    {
     TPIB_AF = datatools::bit_mask::bit00, ///< First track path first vertex
     TPIB_AM = datatools::bit_mask::bit01, ///< First track path middle vertex
     TPIB_AL = datatools::bit_mask::bit02, ///< First track path last vertex
     TPIB_BF = datatools::bit_mask::bit03, ///< Second track path first vertex
     TPIB_BM = datatools::bit_mask::bit04, ///< Second track path middle vertex
     TPIB_BL = datatools::bit_mask::bit05  ///< Second track path last vertex
    };

  /// \brief Intercept pattern between two track paths 
  ///
  /// Patterns to describe the intersection point along two intersecting track paths
  ///
  /// .. code::
  ///
  ///       [FF]          [FM]          : [FL]
  ///                                   b 
  ///                                   b 
  ///                       :           b 
  ///           :           b           :
  ///          -*--aaaaa-  -*--aaaaa-  -*--aaaaa- 
  ///           :           b           :
  ///           b           b
  ///           b           :
  ///           b                            
  ///           :                         
  ///
  ///       [MF]          [MM]          : [ML]
  ///                                   b 
  ///                                   b 
  ///                       :           b 
  ///           :           b           :
  ///        -aa*aa-     -aa*aa-     -aa*aa- 
  ///           :           b           :
  ///           b           b
  ///           b           :
  ///           b
  ///           :
  ///                                 
  ///       [LF]          [LM]          : [LL]
  ///                                   b 
  ///                                   b 
  ///                       :           b 
  ///           :           b           :
  ///     -aaaa-*-    -aaaa-*-    -aaaa-*- 
  ///           :           b           :
  ///           b           b
  ///           b           :
  ///           b
  ///           :
  /// ..
  ///
  enum track_path_intercept_mask
    {
     TPIP_NONE = 0,
     TPIP_FL   = TPIB_AF | TPIB_BL,
     TPIP_FM   = TPIB_AF | TPIB_BM,
     TPIP_FF   = TPIB_AF | TPIB_BF,
     TPIP_ML   = TPIB_AM | TPIB_BL,
     TPIP_MM   = TPIB_AM | TPIB_BM,
     TPIP_MF   = TPIB_AM | TPIB_BF,
     TPIP_LL   = TPIB_AL | TPIB_BL,
     TPIP_LM   = TPIB_AL | TPIB_BM,
     TPIP_LF   = TPIB_AL | TPIB_BF
    };

  /// \brief Result of the intercept of 2 track paths in the XY plane
  struct track_path_intercept
  {
    uint16_t pattern;
    falaise::geometry::point2   intercept;
    double   s_first  = datatools::invalid_real(); ///< Curvilinear abscissa of the intercept with respect to the track path 1
    double   s_second = datatools::invalid_real(); ///< Curvilinear abscissa of the intercept with respect to the track path 2
    // bool     before_first = false;
    // bool     before_second = false;
  };

  /// \brief Algorithm which searches for an intercept between 2 candidate track paths in the XY plane 
  class track_path_interceptor
  {
  public:
    
    enum mode_type
      {
       MODE_LINE  = 0,
       MODE_HELIX = 1
      };

    track_path_interceptor(mode_type mode_,
                           datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
    
    bool find(const track_path_data & tp_a_,
              const track_path_data & tp_b_,
              track_path_intercept & tpi_);

  private:

    bool _find_linear_(const track_path_data & tp_a_,
                       const track_path_data & tp_b_,
                       track_path_intercept & tpi_);

    bool _find_helix_(const track_path_data & tp_a_,
                      const track_path_data & tp_b_,
                      track_path_intercept & tpi_);
    
  private:
    
    mode_type _mode_ = MODE_LINE;
    datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_TRACK_PATH_TOOLS_HH
