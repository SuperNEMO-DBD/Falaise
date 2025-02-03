#ifndef FALAISE_TKRECONSTRUCT_TKTRAJECTORY_H
#define FALAISE_TKRECONSTRUCT_TKTRAJECTORY_H

// Standard headers
#include <iostream>
#include <vector>
#include <cmath>

// ROOT headers
// #include "TObject.h"

#include "tkrec/TKOMhit.h"
#include "tkrec/TKtrhit.h"
#include "tkrec/TKtrack.h"
#include "tkrec/TKpoint.h"

namespace tkrec {

  class TKtrajectory
  //  : public TObject
  {
  private:
		
    int side = -1;
    bool composite_trajectory = false;
    std::vector<ConstTKpointHdl> track_points;
    std::vector<TKtrackHdl> segments;
		
  public:
		
    TKtrajectory() = default;
    TKtrajectory(const TKtrackHdl & _segment);
    TKtrajectory(const std::vector<TKtrackHdl> & _segments);
    virtual ~TKtrajectory() = default;
		
    void set_side(double _side);		
    int get_side() const;
    
    void add_segment(const TKtrackHdl & segment_);
    std::vector<TKtrackHdl> & get_segments();
    std::vector<ConstTKtrackHdl> get_segments() const;

    void add_track_point(const ConstTKpointHdl & track_point);
    const std::vector<ConstTKpointHdl> & get_track_points() const;
		
    void extrapolate();

    void print(std::ostream & out_ = std::clog) const;
		
    // ClassDef(TKtrajectory,1);
    
  };
  
  typedef std::shared_ptr<TKtrajectory> TKtrajectoryHdl;
  typedef std::shared_ptr<const TKtrajectory> ConstTKtrajectoryHdl;

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKTRAJECTORY_H
