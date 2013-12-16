/* -*- mode: c++ -*- */
// SULTAN_interface.h

#ifndef _SULTAN_interface_h_
#define _SULTAN_interface_h_ 1

#include <vector>
#include <iostream>
#include <string>

#include <sultan/SULTAN_config.h>
#include <sultan/cell.h>
#include <sultan/tracked_data.h>
#include <sultan/experimental_point.h>
#include <sultan/cell.h>
#include <sultan/clusterizer.h>
#include <sultan/sultan.h>

namespace SULTAN {
  
  /// Setup data of the SULTAN algorithms
  /// This class contains the minimal set of parameters
  /// needed to run the SULTAN algorithm
  struct setup_data
  {
  public:
    
    setup_data ();
    bool check () const;
    void reset ();
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
    std::string clusterizer_level;
    std::string sequentiator_level;

    /// Used to flag SuperNEMO of NEMO3 experiment
    bool   SuperNemo;

    /// Maximum computing time in ms
    double max_time;

    /// minimum p-value
    double probmin;

    /// number of sigmas
    double nsigma_r;
    double nsigma_z;

    /// Number of cells which can be skipped (because the cell did not
    /// work) and still the cluster is continuous
    int    nofflayers;

    /// first event to be processed
    /// (default = -1 to process all events)
    int    first_event;

    // minimum n of cells in a cluster
    size_t min_ncells_in_cluster;

    // minimum distance between cells in a triplet (units of cell size)
    size_t ncells_between_triplet_min;

    // range distance between cells in a triplet (units of cell size)
    size_t ncells_between_triplet_range;

    /// 0. for SuperNEMO, 1.5 m for NEMO3
    double foil_radius;

    double cell_distance;  // distance between GG cells

    double bfield; // value of magnetic field

    double nsigmas; // n of sigmas for clusterization in helix space

    double xsize, ysize, zsize; // chamber size

    double Emin; // minimum energy of detected electrons

    double Emax; // maximum energy of detected electrons

    // number of blocks of GG wires
    int num_blocks;

    // number of planes of wires in each block
    std::vector<double> planes_per_block;

    // thickness of gap between blocks
    std::vector<double> gaps_Z;

    // number of cells per plane
    int    num_cells_per_plane;

  };

  /// Configure the clusterizer from a setup data object
  void clusterizer_configure (clusterizer & czer_, const setup_data & setup_);

  /// Configure the sultan from a setup data object
  void sultan_configure (sultan & stor_, const setup_data & setup_);

  /// Input data model
  struct input_data
  {
  public:
    topology::cell & add_cell ();
    input_data ();
    bool check () const;

  public:
    std::vector<topology::cell> cells;
  };

  /// Output data model
  struct output_data
  {
  public:
    output_data ();
  public:
    topology::tracked_data tracked_data;
  };

}

#endif // _SULTAN_interface_h_

// end of SULTAN_interface.h
