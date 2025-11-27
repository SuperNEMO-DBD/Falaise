#ifndef FALAISE_TKRECONSTRUCT_TKEVENT_H
#define FALAISE_TKRECONSTRUCT_TKEVENT_H

// Standard headers
#include <vector>
#include <iostream>

// ROOT headers
// #include "TObject.h"

// TK headers
#include "tkrec/TKOMhit.h"
#include "tkrec/TKtrack.h"
#include "tkrec/TKtrhit.h"
#include "tkrec/TKcluster.h"
#include "tkrec/TKpoint.h"
#include "tkrec/TKtrajectory.h"

// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>

namespace tkrec {

  /// Model of a tracker reconstructed event
  class TKEvent
  // : public TObject
  {
  private:
	
    int run_number   = -1; ///< Run number
    int event_number = -1; ///< Event number

    std::vector<TKOMhitHdl> OM_hits; ///< List of calo hits		
    std::vector<TKtrhitHdl> tr_hits; ///< List of tracker hits
    std::vector<TKtrackHdl> tracks; ///< List of tracks
    std::vector<TKclusterHdl> clusters; ///< List of clusters
    std::vector<TKtrajectoryHdl> trajectories; ///< List of trajectories
		
  public:
	
    // basic functionality section:
	
    TKEvent() = default;
    TKEvent(int _run_number, int _event_number);
    virtual ~TKEvent() = default;

    void set_event_ids(int _run_number, int _event_number);
    /// Reset the event internal data
    void reset();

    // OM hits:
    std::vector<TKOMhitHdl> & get_OM_hits();
    std::vector<ConstTKOMhitHdl> get_OM_hits() const;
    TKOMhitHdl & get_OM_hit(int _i);
    ConstTKOMhitHdl get_OM_hit(int _i) const;
    void add_OM_hit(int _OM_num,  bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell);
    void add_OM_hit(int _SWCR[4], bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell);
    void add_OM_hit(const TKOMhitHdl & hit_);

    // Tracker hits:
    std::vector<TKtrhitHdl> & get_tr_hits();
    std::vector<ConstTKtrhitHdl> get_tr_hits() const;
    TKtrhitHdl & get_tr_hit(int _i);
    ConstTKtrhitHdl get_tr_hit(int _i) const;
    void add_tracker_hit(int _cell_num, int64_t _tsp[7]);
    void add_tracker_hit(int _SRL[3],   int64_t _tsp[7]);
    void add_tracker_hit(const TKtrhitHdl & hit_);

    // Tracks:
    std::vector<TKtrackHdl> & get_tracks();
    std::vector<ConstTKtrackHdl> get_tracks() const;
    std::vector<TKtrackHdl> get_all_tracks();
    std::vector<ConstTKtrackHdl> get_all_tracks() const;
    
    // Clusters:
    std::vector<TKclusterHdl> & get_clusters();		
    TKclusterHdl & get_cluster(int _i);
    ConstTKclusterHdl get_cluster(int _i) const;

    // Trajectories:
    std::vector<TKtrajectoryHdl> & get_trajectories();
    std::vector<ConstTKtrajectoryHdl> get_trajectories() const;
    TKtrajectoryHdl & get_trajectory(int _i);
    ConstTKtrajectoryHdl get_trajectory(int _i) const;
	
    int get_run_number() const;
    int get_event_number() const;
    size_t get_no_tracks() const;
    size_t get_no_trajectories() const;
		
    void print(std::ostream & out_ = std::clog) const;
    void print_tracks(std::ostream & out_ = std::clog) const;
    void print_trajectories(std::ostream & out_ = std::clog) const;

    // drift model and plasma propagation section
	
    // associates tracker hits to OM hits and calculates hit radii
    // drift model: "Manchester" or "Betsy"
    // association_mode: "time" or "distance"
    // 	"distance": minimizes distance between OM and tracker hit
    // 	"time": minimizes time difference between OM and tracker hit
    void set_r(std::string drift_model = "Manchester",
	       std::string association_mode = "distance");
		
    // calls sigma_R, sigma_Z, h set functions for all tracker hits
    void set_sigma_R(); // currently unnecessary
    void set_h();
    void set_sigma_Z(); // currently unnecessary
	
    // tracker hit collection filtering
	
    static std::vector<TKtrhitHdl> filter_side(const std::vector<TKtrhitHdl>& _hits, int side);
    static std::vector<TKtrhitHdl> filter_usable(const std::vector<TKtrhitHdl>& _hits);
    static std::vector<TKtrhitHdl> filter_unassociated(const std::vector<TKtrhitHdl>& _hits);
    static std::vector<TKtrhitHdl> filter_unclustered(const std::vector<TKtrhitHdl>& _hits, const TKEvent & event_);
    static std::vector<TKtrhitHdl> filter_distant(const std::vector<TKtrhitHdl>& _hits);
    static std::vector<TKtrhitHdl> filter_close_hits(const std::vector<TKtrhitHdl>& _hits,
						     double phi,
						     double r,
						     double distance_limit);
    
    // ClassDef(TKEvent,1);

    friend class TKvisu; ///< Private access from visualization engine
    
  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKEVENT_H
