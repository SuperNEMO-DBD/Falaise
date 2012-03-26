/* -*- mode: c++ -*- */
// CAT_clusterizer_interface.h

#ifndef _CAT_clusterizer_interface_h_
#define _CAT_clusterizer_interface_h_ 1

#include <vector>
#include <iostream>
#include <string>

#include <CATAlgorithm/CAT_config.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/tracked_data.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/clusterizer.h>

namespace CAT {

  /// Setup data of the CAT clusterizer 
  /// This class contains the minimal set of parameters
  /// needed to run the CAT clusterizer algorithm
  struct clusterizer_setup_data
  {
  public:

    clusterizer_setup_data ();
    bool check () const;
    void reset ();
    const string & get_error_message () const;

  protected:

    void _set_defaults ();
    bool _check_snemo ();
    void _set_error_message (const std::string & message_);

  protected :

    std::string _error_message;

  public:

    /// Let all attributes be public :
    std::string level; /// verbosity level: "mute", "normal", "verbose", "vverbose"

    bool   SuperNemo;
    double MaxTime; /// Maximum computing time 

    double SmallRadius; // [length] -> mm
    double TangentPhi;
    double TangentTheta;
    double SmallNumber; // [length] - mm
    double QuadrantAngle;
    double Ratio;
    double CompatibilityDistance;
    double MaxChi2;
    double nsigma;
    int    nofflayers;

    // SuperNEMO geometry :
    int num_blocks;
    std::vector<double> planes_per_block;
    std::vector<double> gaps_Z;
    int    num_cells_per_plane;
    double cell_size; 
 
    // SuperNEMO :
    double sigma0; // Longitudinal sigma z
    double k0, k1, k2, k3; 
    double th0, th1, th2, th3;
    double l0, l1; 

  };

  /// Configure the clusterizer from a setup data object
  void clusterizer_configure (clusterizer & czer_, const clusterizer_setup_data & setup_);

  /// Input data model 
  struct clusterizer_input_data
  {
  public:
    topology::cell & add_cell ();
    clusterizer_input_data ();

  public:
    std::vector<topology::cell> cells;  
  };

  /// Output data model
  struct clusterizer_output_data
  {
  public:
    clusterizer_output_data ();
  public:
    topology::tracked_data tracked_data;  
  };

}

#endif // _CAT_clusterizer_interface_h_ 

// end of CAT_clusterizer_interface.h
