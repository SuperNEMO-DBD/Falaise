/* -*- mode: c++ -*- */
// CAT_interface.h

#ifndef _CAT_interface_h_
#define _CAT_interface_h_ 1

#include <vector>
#include <iostream>
#include <string>

#include <CATAlgorithm/CAT_config.h>
#include <CATAlgorithm/tracked_data_base.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/cell_base.h>
#include <CATAlgorithm/clusterizer.h>
#include <CATAlgorithm/sequentiator.h>

namespace CAT {

/// Setup data of the CAT algorithms
/// This class contains the minimal set of parameters
/// needed to run the CAT algorithm
struct setup_data {
 public:
  setup_data();
  bool check() const;
  void reset();
  const std::string& get_error_message() const;

 protected:
  void _set_defaults();
  bool _check_snemo();
  void _set_error_message(const std::string& message_);

 protected:
  std::string _error_message;

 public:
  /// Let all attributes be public :

  /// Verbosity level: "mute", "normal", "verbose", "vverbose"
  std::string level;

  /// Used to flag SuperNEMO of NEMO3 experiment
  bool SuperNemo;

  /// Maximum computing time in ms
  double MaxTime;

  /// Ratio of 2nd best to best probability which is acceptable as 2nd solution
  double Ratio;

  /// minimum p-value to be a straight line
  double probmin;

  /// Number of cells which can be skipped (because the cell did not
  /// work) and still the cluster is continuous
  int nofflayers;

  /// first event to be processed
  /// (default = -1 to process all events)
  int first_event;

  /// 0. for SuperNEMO, 1.5 m for NEMO3
  double FoilRadius;

  // Obsolete parameters, just there for backwards compatibility,
  double SmallRadius;  // [length] -> mm
  double TangentPhi;
  double TangentTheta;
  double SmallNumber;  // [length] - mm
  double QuadrantAngle;
  double CompatibilityDistance;
  double MaxChi2;
  double vel;  // plasma velocity in cell, not needed anymore because vertical position
               // is reconstructed outside of CAT
  double len;  // length of each drift wire, should be read from geometry instead of free parameter
  double rad;  // radius of each cell, should be read from geometry instead of free parameter
  double CellDistance;  // same as above

  double bfield;               // value of magnetic field
  double xsize, ysize, zsize;  // chamber size

  // SuperNEMO geometry :
  int num_blocks;
  std::vector<double> planes_per_block;
  std::vector<double> gaps_Z;
  int num_cells_per_plane;
  double cell_size;

  // SuperNEMO :
  double sigma0;  // Longitudinal sigma z
  double k0, k1, k2, k3;
  double th0, th1, th2, th3;
  double l0, l1;
};

/// Configure the clusterizer from a setup data object
void clusterizer_configure(clusterizer& czer_, const setup_data& setup_);

/// Configure the sequentiator from a setup data object
void sequentiator_configure(sequentiator& stor_, const setup_data& setup_);

/// Input data model
struct input_data {
 public:
  topology::cell& add_cell();
  topology::calorimeter_hit& add_calo_cell();
  input_data();
  bool check() const;
  bool gg_check() const;
  bool calo_check() const;

 public:
  std::vector<topology::cell> cells;
  std::vector<topology::calorimeter_hit> calo_cells;
};

/// Output data model
struct output_data {
 public:
  output_data();

 public:
  topology::tracked_data tracked_data;
};

}  // namespace CAT

#endif  // _CAT_interface_h_

// end of CAT_interface.h
