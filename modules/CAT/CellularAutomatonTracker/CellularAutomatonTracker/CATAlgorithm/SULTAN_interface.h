/* -*- mode: c++ -*- */
// SULTAN_interface.h

#ifndef _CAT_SULTAN_interface_h_
#define _CAT_SULTAN_interface_h_ 1

#include <vector>
#include <iostream>
#include <string>

#include <CATAlgorithm/SULTAN_config.h>
#include <CATAlgorithm/Cell.h>
#include <CATAlgorithm/tracked_data.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/Sultan.h>

namespace CAT {

  /// Setup data of the SULTAN algorithms
  /// This class contains the minimal set of parameters
  /// needed to run the SULTAN algorithm
  struct setup_data_sultan
  {
  public:

    setup_data_sultan ();
    bool check() const;
    void reset();
    const std::string & get_error_message () const;

  protected:

    void _set_defaults ();
    bool _check_snemo ();
    void _set_error_message (const std::string & message_);

  protected :

    std::string _error_message;

  public:

    /// Let all attributes be public :

    /// Verbosity level: "mute", "normal", "verbose", "vverbose"
    std::string level;

    /// Used to flag SuperNEMO of NEMO3 experiment
    bool   SuperNemo;

    /// Maximum computing time in ms
    double MaxTime;

    /// minimum p-value
    double probmin;

    /// Number of cells which can be skipped (because the cell did not
    /// work) and still the cluster is continuous
    int    nofflayers;

    /// first event to be processed
    /// (default = -1 to process all events)
    int    first_event;

    /// 0. for SuperNEMO, 1.5 m for NEMO3
    double FoilRadius;

    double vel;  // plasma velocity in cell, not needed anymore because vertical position
                 // is reconstructed outside of SULTAN
    double len;  // length of each drift wire, should be read from geometry instead of free parameter
    double rad;  // radius of each cell, should be read from geometry instead of free parameter
    double CellDistance;  // same as above

    double bfield; // value of magnetic field
    double xsize, ysize, zsize; // chamber size

    int    num_cells_per_plane;
    double cell_size;

    // SuperNEMO :
    double sigma0; // Longitudinal sigma z
    double k0, k1, k2, k3;
    double th0, th1, th2, th3;
    double l0, l1;

  };

  /// Configure the sultan from a setup data object
  void sultan_configure (Sultan & sultan_, const setup_data_sultan & setup_);

  /// Input data model
  struct input_data_sultan
  {
  public:
    topology::Cell & add_cell ();
    input_data_sultan ();
    bool check () const;

  public:
    std::vector<topology::Cell> cells;
  };

  /// Output data model
  struct output_data_sultan
  {
  public:
    output_data_sultan ();
  public:
    topology::Tracked_data tracked_data;
  };

}

#endif // _CAT_SULTAN_interface_h_

// end of SULTAN_interface.h
