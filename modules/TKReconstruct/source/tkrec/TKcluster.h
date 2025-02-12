#ifndef FALAISE_TKRECONSTRUCT_TKCLUSTER_H
#define FALAISE_TKRECONSTRUCT_TKCLUSTER_H

// Standard headers
#include <iostream>
#include <cmath>
#include <memory>

// ROOT headers
#include "TCanvas.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TObject.h"

#include "tkrec/TKtrhit.h"
#include "tkrec/TKtrack.h"

#include <datatools/logger.h>

namespace tkrec {

  class TKcluster
  //  : public TObject
  {
  private:
		
    int    side    = -1;   ///< Side of the tracking chamber
    double phi_min = 0.0;  ///< Min angle
    double phi_max = M_PI; ///< Max angle
		
    // 0 == no ambiguity
    // 1 == mirror image along line x = x0 
    // 2 == mirror image along line y = y0 
    // 3 == mirror image along line y = x + (y0-x0) 
    // 4 == mirror image along line y = -x + (y0-x0) 
    int ambiguity_type = 0;
		
    std::vector<TKtrhitHdl> cluster_tr_hits;
    TKtrackHdl track;

  public:
    
    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    
  public:
    
    TKcluster() = default;		
    TKcluster(const std::vector<TKtrhitHdl> & tr_hits,
	      double _phi_min,
	      double _phi_max);
    virtual ~TKcluster() = default;
		
    void add_tr_hit(TKtrhitHdl tracker_hit);
    std::vector<TKtrhitHdl> & get_tr_hits();
    std::vector<ConstTKtrhitHdl> get_tr_hits() const;

    bool has_track() const;
    void set_track(const TKtrackHdl & _track);
    TKtrackHdl & get_track();
    ConstTKtrackHdl get_track() const;
		
    void set_side(double _side);
    void set_phi_min(double _phi_min);
    void set_phi_max(double _phi_max);
		
    int    get_side() const;
    double get_phi_min() const;
    double get_phi_max() const;

    void detect_ambiguity_type();
    int get_ambiguity_type() const;
		
    void reconstruct_ambiguity();
    void reconstruct_MLM(bool save_sinograms, int run_number, int event_number);
    void reconstruct_MLM_3D(bool save_sinograms, int run_number, int event_number);
    void draw_ML_vertical(int run_number, int event_number);

    void print(std::ostream & out_ = std::cout) const;
		
    // ClassDef(TKcluster,1);
  };

  typedef std::shared_ptr<TKcluster> TKclusterHdl;
  typedef std::shared_ptr<const TKcluster> ConstTKclusterHdl;

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKCLUSTER_H
