/* -*- mode: c++ -*- */
#ifndef __sultan___clusterizer___
#define __sultan___clusterizer___

#include <stdexcept>
#include <mybhep/messenger.h>

#include <iostream>
#include <vector>

#include <sultan/Clock.h>
#include <sultan/cell.h>
#include <sultan/experimental_double.h>
#include <sultan/cluster.h>
#include <sultan/sequence.h>
#include <sultan/tracked_data.h>
#include <CATAlgorithm/calorimeter_hit.h>


namespace SULTAN{

  /// The clusterizer algorithm
  class clusterizer{
    
  public:
    
    clusterizer(void);

    virtual ~clusterizer();

  public:
    bool initialize( void );
    bool finalize();
    void read_properties();
    bool prepare_event(topology::tracked_data & __tracked_data);
    void print_cells(void)const;
    void clusterize(topology::tracked_data & __tracked_data);
    void print_clusters(void) const;
    void set_module_nr(const std::string &mID);
    std::string get_module_nr(void);
    void set_max_time(double v);
    void set_probmin(double v);
    void set_nofflayers(size_t v);
    void set_first_event(int v);
    void set_level(std::string v);
    void set_cell_distance(double v);
    void set_SuperNemoChannel(bool v);
    void set_foil_radius(double v);
    void set_xsize(double v);
    void set_ysize(double v);
    void set_zsize(double v);

    //! get cells
    const std::vector<topology::cell>& get_cells()const;

    //! set cells
    void set_cells(const std::vector<topology::cell> & cells);

    //! get clusters
    const std::vector<topology::cluster>& get_clusters()const;

    //! set clusters
    void set_clusters(const std::vector<topology::cluster> & clusters);

    //! get calorimeter_hits
    const std::vector<topology::calorimeter_hit>& get_calorimeter_hits()const;

    //! set calorimeter_hits
    void set_calorimeter_hits(const std::vector<topology::calorimeter_hit> & calorimeter_hits);

  protected:

    int cell_side( const topology::cell & c);
    std::vector<topology::cell> get_near_cells(const topology::cell & c);
    void setup_cells();
    void setup_clusters();
    void _set_defaults ();


    Clock clock;

    mybhep::prlevel level;
    mybhep::messenger m;
    int nevent;
    int event_number;
    int initial_events;
    int skipped_events;

    //geom param
    double cell_distance;
    double xsize,ysize,zsize;
    double inner_radius, outer_radius, foil_radius; //only for plotting
    double bfield;

    double probmin;
    size_t nofflayers;
    int first_event_number;

    // Support numbers
    double execution_time;
    bool SuperNemoChannel; /** New initialization modeof the algorithm
                            *  for SuperNEMO and usage from Channel by
                            *  Falaise and Hereward.
                            */
    double max_time;
    double run_time;
    bool first_event;


  private:
    std::string  _moduleNR;

    std::vector<topology::cell> cells_;
    std::vector<topology::cluster> clusters_;
    std::vector<topology::calorimeter_hit> calorimeter_hits_;

  };

}

#endif
