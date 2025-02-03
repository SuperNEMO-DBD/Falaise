// TK headers
#include "tkrec/TKEvent.h"

// Standard headers
#include <cmath>
#include <iomanip>

// ROOT headers
#include "TCanvas.h"
#include "TColor.h"
#include "TEllipse.h"
#include "TH2D.h"
#include "TAttLine.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TFile.h"
#include "TROOT.h"
#include "TPolyLine3D.h"
#include "TPolyLine.h"
#include "TBox.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPoint.h"
#include <TStyle.h>
#include <TF1.h>

// - Bayeux:
#include <bayeux/datatools/clhep_units.h>

// ClassImp(tkrec::TKEvent);

namespace tkrec {

  // dimensions in mm
  // origin in the center of detector
  const double Bi_source_x = 1.4;
  const double Bi_source_y = 14.2;
  const double Bi_source_z = 21.1;
  const double Bi_source_dist_y = 835.0;
  const double Bi_source_dist_z = 425.0;

  // dimensions in mm
  // origin in the center of detector
  const double tc_radius = 22.0;

  // OM dimensions in mm
  // warning: dimensions are approximately taken from Falaise and might not be exactly right
  const double mw_sizex = 194.0;
  const double mw_sizey = 256.0;
  const double mw_sizez = 256.0;

  const double gv_sizex = 308.0;
  const double gv_sizey = 310.0;
  const double gv_sizez = 150.0;

  const double xw_sizex = 200.0;
  const double xw_sizey = 150.0;
  const double xw_sizez = 208.5;

  using namespace std;
    
  TKEvent::TKEvent(int _run_number ,int _event_number)
  {
    set_event_ids(_event_number, _run_number);
    return;
  }

  void TKEvent::reset()
  {
    OM_hits.clear();
    tr_hits.clear();
    tracks.clear();
    clusters.clear();
    trajectories.clear();
    event_number = -1;
    run_number = -1;
    return;
  }

  void TKEvent::set_event_ids(int _run_number, int _event_number)
  {
    event_number = _event_number;
    run_number = _run_number;
    return;
  }
 
  std::vector<TKOMhitHdl> & TKEvent::get_OM_hits()
  {
    return OM_hits;
  }

  std::vector<ConstTKOMhitHdl> TKEvent::get_OM_hits() const
  {
    std::vector<ConstTKOMhitHdl> hits;
    for (const auto & h : OM_hits) {
      hits.push_back(h);
    }
    return hits;
  }

  TKOMhitHdl & TKEvent::get_OM_hit(int _i)
  {
    return OM_hits[_i];
  }

  ConstTKOMhitHdl TKEvent::get_OM_hit(int _i) const
  {
    return OM_hits[_i];
  }

  void TKEvent::add_OM_hit(int _OM_num, bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell)
  {
    OM_hits.push_back(std::make_shared<TKOMhit>(_OM_num, _is_HT, _OM_TDC, _OM_pcell));
    return;
  }
		
  void TKEvent::add_OM_hit(int _SWCR[4], bool _is_HT, int64_t _OM_TDC, int16_t _OM_pcell)
  {
    OM_hits.push_back(std::make_shared<TKOMhit>(_SWCR, _is_HT, _OM_TDC, _OM_pcell));
    return;
  }
		
  void TKEvent::add_OM_hit(const TKOMhitHdl & om_hit_)
  {
    OM_hits.push_back(om_hit_);
    return;
  }	
 
  std::vector<TKtrhitHdl> & TKEvent::get_tr_hits()
  {
    return tr_hits;
  }

  std::vector<ConstTKtrhitHdl> TKEvent::get_tr_hits() const
  {
    std::vector<ConstTKtrhitHdl> hits;
    for (const auto & h : tr_hits) {
      hits.push_back(h);
    }
    return hits;
  }

  TKtrhitHdl & TKEvent::get_tr_hit(int _i)
  {
    return tr_hits[_i];
  }

  ConstTKtrhitHdl TKEvent::get_tr_hit(int _i) const
  {
    return tr_hits[_i];
  }
	
  void TKEvent::add_tracker_hit(int _cell_num, int64_t _tsp[7])
  {
    tr_hits.push_back(std::make_shared<TKtrhit>(_cell_num, _tsp));
    return;
  }
		
  void TKEvent::add_tracker_hit(int _SRL[3], int64_t _tsp[7])
  {
    tr_hits.push_back(std::make_shared<TKtrhit>(_SRL, _tsp));
    return;
  }

  void TKEvent::add_tracker_hit(const TKtrhitHdl & hit_)
  {
    tr_hits.push_back(hit_);
    return;
  }

  std::vector<ConstTKtrajectoryHdl> TKEvent::get_trajectories() const
  {
    std::vector<ConstTKtrajectoryHdl> trajs;
    for(const auto &  trajH : trajectories)
      {
	trajs.push_back(trajH);
      }  
    return trajs;
  }
  
  std::vector<TKtrackHdl> TKEvent::get_all_tracks()
  {
    vector<TKtrackHdl> all_tracks;
    for(auto & trackH : tracks)
      {
	all_tracks.push_back( trackH );
      }
    for(auto & cluster : clusters)
      {
	if( cluster->has_track() )
	  {
	    all_tracks.push_back( cluster->get_track() );
	    if( cluster->get_track()->has_mirror_image() )
	      {
		all_tracks.push_back( cluster->get_track()->get_mirror_image() );
	      }
	  }
      }
    return all_tracks;
  }
 
  std::vector<ConstTKtrackHdl> TKEvent::get_all_tracks() const
  {
    vector<ConstTKtrackHdl> all_tracks;
    for(const auto & trackH : tracks)
      {
	all_tracks.push_back( trackH );
      }
    for(const auto & cluster : clusters)
      {
	if( cluster->has_track() )
	  {
	    all_tracks.push_back( cluster->get_track() );
	    if( cluster->get_track()->has_mirror_image() )
	      {
		all_tracks.push_back( cluster->get_track()->get_mirror_image() );
	      }
	  }
      }
    return all_tracks;
  }

  std::vector<TKtrackHdl> & TKEvent::get_tracks()
  {
    return tracks;
  }
  
  std::vector<ConstTKtrackHdl> TKEvent::get_tracks() const
  {
    vector<ConstTKtrackHdl> constTracks;
    for(const TKtrackHdl & track : tracks)
      {
	constTracks.push_back( track );
      }
    return constTracks;
  }

  std::vector<TKclusterHdl> & TKEvent::get_clusters()
  {
    return clusters;
  }

  std::vector<TKtrajectoryHdl> & TKEvent::get_trajectories()
  {
    return trajectories;
  }

  TKclusterHdl & TKEvent::get_cluster(int _i)
  {
    return clusters[_i];
  }

  ConstTKclusterHdl TKEvent::get_cluster(int _i) const
  {
    return clusters[_i];
  }

  TKtrajectoryHdl & TKEvent::get_trajectory(int _i)
  {
    return trajectories[_i];
  }

  ConstTKtrajectoryHdl TKEvent::get_trajectory(int _i) const
  {
    return trajectories[_i];
  }

  int TKEvent::get_run_number() const
  {
    return run_number;
  }

  int TKEvent::get_event_number() const
  {	
    return event_number;
  }

  size_t TKEvent::get_no_tracks() const
  {
    return this->get_all_tracks().size();
  }

  size_t TKEvent::get_no_trajectories() const
  {
    return trajectories.size();
  }
		
  void TKEvent::print(std::ostream & out_) const
  {
    out_ << std::endl;
    out_ << "RUN " << run_number << " | EVENT " << event_number << std::endl << std::endl;
    out_ << "Collection of OM hits: " << std::endl;

    for(const auto & OM_hit : OM_hits) 
      {
	OM_hit->print(out_);
      }
	
    out_ << std::endl;
    out_ << "Collection of tracker hits: " << std::endl;
	
    for(const auto& tr_hit : tr_hits) 
      {
	tr_hit->print(out_);
      }

    out_ << std::endl;
    out_ << "Collection of tracks: " << std::endl;
	
    vector<ConstTKtrackHdl> all_tracks = this->get_all_tracks();
    for(const auto & track : all_tracks)
      {	
	track->print(out_);
      }
    out_ << std::endl;
    return;
  }

  void TKEvent::print_tracks(std::ostream & out_) const
  {
    out_ << std::endl;
    out_ << "RUN " << run_number << " | EVENT " << event_number << std::endl << std::endl;
    vector<ConstTKtrackHdl> all_tracks = this->get_all_tracks();
    for(const auto& track : all_tracks)
      {	
	track->print();
      }
    out_ << std::endl;	
    return;
  }

  void TKEvent::print_trajectories(std::ostream & out_) const
  {
    out_ << std::endl;
    out_ << "RUN " << run_number << " | EVENT " << event_number << std::endl << std::endl;
    for(const auto & trajectory : trajectories)
      {
	trajectory->print();
      }
    out_ << std::endl;
    return;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_side(const std::vector<TKtrhitHdl>& _hits, int side)
  {
    vector<TKtrhitHdl> hits;	
    for(auto& hit : _hits)
      {
	if( side == hit->get_SRL('s'))
	  {
	    hits.push_back( hit );
	  }
      }
    return hits;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_usable(const std::vector<TKtrhitHdl>& _hits)
  {
    vector<TKtrhitHdl> hits;	
    for(auto& hit : _hits)
      {
	// not using broken or too big (incorrectly associated) tracker hits
	if( hit->get_r() != -1.0 && hit->get_r() < 35.0 && hit->get_r() > 2.0 )
	  {
	    hits.push_back( hit );
	  }
      }
    return hits;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_unassociated(const std::vector<TKtrhitHdl>& _hits)
  {
    vector<TKtrhitHdl> hits;	
    for(auto& hit : _hits)
      {
	if( not hit->has_associated_track() )
	  {
	    hits.push_back( hit );
	  }
      }
    return hits;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_distant(const std::vector<TKtrhitHdl>& _hits)
  {
    double distance = 3.0; // in cells: 1 == 44mm
    vector<TKtrhitHdl> hits;
    for(auto i = 0u; i < _hits.size(); i++)
      {
	bool close = false;			
	int RL[2] = {_hits[i]->get_SRL('R'),_hits[i]->get_SRL('L')};
	for(auto j = 0u; j < _hits.size(); j++)
	  {
	    if(i == j) continue;
	    if(pow(RL[0] - _hits[i]->get_SRL('R'), 2) + pow(RL[1] - _hits[i]->get_SRL('L'), 2) <= distance*distance)
	      {
		close = true;
		// continue or break?	
	      }		
	  }
	if( close )
	  { 
	    hits.push_back(_hits[i]);
	  } 
      }
    return hits;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_unclustered(const std::vector<TKtrhitHdl>& _hits, const TKEvent & event_)
  {
    vector<TKtrhitHdl> hits;
    for(auto& hit : _hits)
      {
	int SRL[3] = {hit->get_SRL('S'), hit->get_SRL('R'), hit->get_SRL('L')};
	bool clustered = false;			
	for(auto j = 0u; j < event_.clusters.size(); j++)
	  {	
	    for(auto k = 0u; k < event_.clusters[j]->get_tr_hits().size(); k++)
	      {	
		if(event_.clusters[j]->get_tr_hits()[k]->get_SRL('S') == SRL[0] &&
		   event_.clusters[j]->get_tr_hits()[k]->get_SRL('R') == SRL[1] &&
		   event_.clusters[j]->get_tr_hits()[k]->get_SRL('L') == SRL[2])
		  {
		    clustered = true;
		  }
	      }
	  }
	if( clustered  == false )
	  {
	    hits.push_back( hit );
	  }
      }
    return hits;
  }

  std::vector<TKtrhitHdl> TKEvent::filter_close_hits(const std::vector<TKtrhitHdl>& _hits,
						     double phi,
						     double r,
						     double distance_limit)
  {
    vector<TKtrhitHdl> hits;	
    for(auto& hit : _hits)
      {
	double R = hit->get_r();
	double x = hit->get_xy('x');
	double y = hit->get_xy('y');
	double distance = std::fabs(r - x*sin(phi) + y*cos(phi)) - R;
	if( std::fabs(distance) <= distance_limit )
	  {
	    hits.push_back( hit );
	  }
      }
    return hits;
  }

  void TKEvent::set_r(std::string drift_model, std::string association_mode)
  {	
    for(auto tr_hit = 0u; tr_hit < tr_hits.size(); tr_hit++)
      {
	double r = std::numeric_limits<double>::quiet_NaN();
	if(tr_hits[tr_hit]->get_tsp('0') != -1)
	  {
	    double min_time = std::numeric_limits<double>::infinity();
	    // associates tracker hits to OM with minimal time difference
	    if( association_mode == "time" ) 
	      {
		//double calo_hit = (calo_tdc * 6.25) - 400.0 + (400.0 * peak_cell / 1024.0);
		for(auto om_hit = 0u; om_hit < OM_hits.size(); om_hit++)
		  {
		    int64_t TDC_diff = 2*tr_hits[tr_hit]->get_tsp('0') - OM_hits[om_hit]->get_OM_TDC() + 44;
		    if(       TDC_diff  < 800 && 
			      TDC_diff  > 0   &&
			      double(TDC_diff) < min_time)
		      {
			min_time = 6.25 * TDC_diff;
			tr_hits[tr_hit]->set_associated_OMhit(OM_hits[om_hit]);
		      }
		  }
	      }

	    // associates tracker hits to OM with minimal distance
	    else if( association_mode == "distance" )
	      {
		double min_distance = std::numeric_limits<double>::infinity();
		for(auto om_hit = 0u; om_hit < OM_hits.size(); om_hit++)
		  {
		    double delta_x = tr_hits[tr_hit]->get_xy('x') - OM_hits[om_hit]->get_xyz('x');
		    double delta_y = tr_hits[tr_hit]->get_xy('y') - OM_hits[om_hit]->get_xyz('y');
		    double delta_z = tr_hits[tr_hit]->get_h()     - OM_hits[om_hit]->get_xyz('z');
		    double distance = sqrt( delta_x*delta_x + delta_y*delta_y + delta_z*delta_z );
		    if( distance < min_distance ) 
		      {
			int64_t TDC_diff = 2*tr_hits[tr_hit]->get_tsp('0') - OM_hits[om_hit]->get_OM_TDC() + 44;
			if( TDC_diff <= 0 ) 
			  {
			    continue;
			  }
			min_time = 6.25 * TDC_diff;
			tr_hits[tr_hit]->set_associated_OMhit(OM_hits[om_hit]);
			min_distance = distance;
		      }
					
		  }
	      }
	    else clog << "invalid association model: choose \"distance\" or \"time\"." << endl;

	    if( drift_model == "Manchester" )
	      {
		const double A1 = 0.570947153108633;
		const double B1 = 0.580148313540993;
		const double C1 = 1.6567483468611;
		const double A2 = 1.86938462695651;
		const double B2 = 0.949912427483918;

		const double t_usec = min_time / 1000.0;
		const double ut = 10. * t_usec;
		
		r = A1 * ut / (std::pow(ut, B1) + C1);
		if (r > A2 * ut / (std::pow(ut, B2))) 
		  {
		    r = A2 * ut / (std::pow(ut, B2));
		  }	

		r *= 10.0;	
	      }
	    // modification of Betsy's model
	    else if( drift_model == "Betsy" )
	      {
		const double a1 = 0.828;
		const double b1 = -0.907;
		const double a2 = 0.402;
		const double b2 = -1.955;
		
		min_time = min_time / 1000.0;
		if(min_time < 3.0845 && min_time > 0.0)
		  {
		    r = pow(min_time/a1, 1.0/(1.0-b1)) * 10.0;
		  }
		else
		  {
		    r = pow(min_time/a2, 1.0/(1.0-b2)) * 10.0;
		  }
	      }
	    else cerr << "invalid drift time model: choose \"Betsy\" or \"Manchester\"." << endl;
	  }
	else
	  {
	    r = -1.0;
	  }
	tr_hits[tr_hit]->set_r(r);
	tr_hits[tr_hit]->set_sigma_R();
      }
  }

  void TKEvent::set_sigma_R()
  {
    for(auto tr_hit = 0u; tr_hit < tr_hits.size(); tr_hit++)
      {
	tr_hits[tr_hit]->set_sigma_R();
      }
  }

  void TKEvent::set_h()
  {
    for(auto tr_hit = 0u; tr_hit < tr_hits.size(); tr_hit++)
      {
	tr_hits[tr_hit]->set_h();
      }
  }

  void TKEvent::set_sigma_Z()
  {
    for(auto tr_hit = 0u; tr_hit < tr_hits.size(); tr_hit++)
      {
	tr_hits[tr_hit]->set_sigma_Z();
      }
  }

} // end of namespace tkrec
