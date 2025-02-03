// TK headers
#include "tkrec/TKtrajectory.h"

// Bayeux:
#include <datatools/exception.h>

// ClassImp(tkrec::TKtrajectory);

namespace tkrec {

  using namespace std;

  TKtrajectory::TKtrajectory(const TKtrackHdl & segment)
  {
    side = segment->get_side();
    composite_trajectory = false;
	
    add_segment(segment);
    vector<ConstTKpointHdl> tr_hit_points = segment->get_associated_tr_hit_points();
    // TODO: Run 974 | Event 1376: only 1 associated hit
    if(tr_hit_points.size() < 2)
      {
	cout << "WARNING: only 1 associated tracker hit... Cannot create a trajectory" << endl;
      }
    double y_min =  2500.0; // in mm
    double y_max = -2500.0; // in mm
	
    auto index_max = 0u;
    auto index_min = 0u;
    // TODO: Run 974 | Event 1158: a = 0 -> does not work
    for(auto i = 0u; i < tr_hit_points.size(); i++)
      {
	double y = tr_hit_points[i]->get_y();
	if(y >= y_max)
	  {
	    y_max = y;
	    index_max = i; 
	  }
	if(y < y_min)
	  {
	    y_min = y; 
	    index_min = i;
	  }
      }
    add_track_point(tr_hit_points[index_max]);
    add_track_point(tr_hit_points[index_min]);
    return;
  }

  TKtrajectory::TKtrajectory(const std::vector<TKtrackHdl> & _segments)
  {
    DT_THROW_IF(_segments.size() == 0, std::logic_error, "No track segments");
    segments = _segments;
    side = segments[0]->get_side();
    if(segments.size() > 1)
      {
	composite_trajectory = true;
      }
    else
      {
	composite_trajectory = false;
      }
	
    for(auto i = 0u; i < segments.size()-1; i++)
      {
	double a1 = segments[i]->get_a();
	double a2 = segments[i+1]->get_a();
	double b1 = segments[i]->get_b();
	double b2 = segments[i+1]->get_b();
		
	double x = (b2-b1)/(a1-a2);
	double y = a1*x + b1;			
	double z1 = segments[i]->get_c()*x + segments[i]->get_d();
	double z2 = segments[i+1]->get_c()*x + segments[i+1]->get_d();
	double z = (z1+z2)/2.0;
	add_track_point(std::make_shared<TKpoint>(x,y,z));
      }

    vector<ConstTKpointHdl> tr_hit_points = segments[0]->get_associated_tr_hit_points();
    double y_kink = track_points[0]->get_y();
    // double x_kink = track_points[0]->get_x();
    double y_min = 2500.0;
    double y_max = -2500.0;
	
    int side_positive_counter = 0;
    int side_negative_counter = 0;
    auto index_max = 0u; 
    auto index_min = 0u;
    for(auto i = 0u; i < tr_hit_points.size(); i++)
      {
	double y = tr_hit_points[i]->get_y();
	if( y > y_kink ) side_positive_counter++;
	if( y < y_kink ) side_negative_counter++;
		
	if(y > y_max)
	  {
	    y_max = y;
	    index_max = i; 
	  }
	if(y < y_min)
	  {
	    y_min = y; 
	    index_min = i;
	  }
      }
    if( side_positive_counter > side_negative_counter )
      {
	track_points.insert(track_points.begin(), tr_hit_points[index_max]);
      } 
    else
      {
	track_points.insert(track_points.begin(), tr_hit_points[index_min]);
      }
	
    tr_hit_points = segments.back()->get_associated_tr_hit_points();
    y_kink = track_points.back()->get_y();
    // x_kink = track_points.back()->get_x();
    y_min = 2500.0;
    y_max = -2500.0;
	
    side_positive_counter = 0;
    side_negative_counter = 0;
    index_max = 0u; 
    index_min = 0u;
    for(auto i = 0u; i < tr_hit_points.size(); i++)
      {
	double y = tr_hit_points[i]->get_y();
	if( y > y_kink ) side_positive_counter++;
	if( y < y_kink ) side_negative_counter++;				
	if(y > y_max)
	  {
	    y_max = y;
	    index_max = i; 
	  }
	if(y < y_min)
	  {
	    y_min = y; 
	    index_min = i;
	  }
      }
    if( side_positive_counter > side_negative_counter )
      {
	add_track_point(tr_hit_points[index_max]);
      } 
    else
      {
	add_track_point(tr_hit_points[index_min]);
      }
    return;
  }

  void TKtrajectory::set_side(double _side)
  {
    side = _side;
    return;
  }

  int TKtrajectory::get_side() const
  {
    return side;
  } 

  void TKtrajectory::add_segment(const TKtrackHdl & segment_)
  {
    segments.push_back(segment_);
    return;
  }

  std::vector<TKtrackHdl> & TKtrajectory::get_segments()
  {
    return segments;
  }

  std::vector<ConstTKtrackHdl> TKtrajectory::get_segments() const
  {
    std::vector<ConstTKtrackHdl> segs;
    for (const auto & trk : segments) {
      segs.push_back(trk);
    }
    return segs;
  }

  void TKtrajectory::add_track_point(const ConstTKpointHdl & track_point)
  {
    track_points.push_back(track_point);
    return;
  }

  // std::vector<TKpointHdl> & TKtrajectory::get_track_points()
  // {
  //   return track_points;
  // }

  const std::vector<ConstTKpointHdl> & TKtrajectory::get_track_points() const
  {
    return track_points;
    // std::vector<ConstTKpointHdl> trkpts;
    // for (const auto & tp : track_points) {
    //   trkpts.push_back(tp);
    // }
    // return trkpts;
  }

  void TKtrajectory::extrapolate()
  {
    const double max_allowed_extrapolation = 150.0;

    const double mainwall_x_position = 435.0;
    const double Xwall_y_position = 2505.5;
    const double Gveto_z_position = 1550.0;

    if(track_points.size() < 2) return;
	
    double x0(0.0), y0(0.0), z0(0.0);
    double x1 = track_points[0]->get_x();
    double y1 = track_points[0]->get_y();
    double z1 = track_points[0]->get_z();
    double x2 = track_points[1]->get_x();
    double y2 = track_points[1]->get_y();
    double z2 = track_points[1]->get_z();

    bool found = false;
    if(x1 != x2)
      {
	x1 < x2 ? x0 = mainwall_x_position * (side-1) : x0 = side*mainwall_x_position; 
	double Cx = (x1-x0)/(x2-x1);
	y0 = y1 - Cx*(y2-y1);
	z0 = z1 - Cx*(z2-z1);
	if( Xwall_y_position > abs(y0) && Gveto_z_position > abs(z0) )
	  {
	    found = true;
	  }
      }
    if(!found && y1 != y2)
      {
	y1 < y2 ? y0 = -Xwall_y_position : y0 = Xwall_y_position; 
	double Cy = (y1-y0)/(y2-y1);
	x0 = x1 - Cy*(x2-x1);
	z0 = z1 - Cy*(z2-z1);
	if( mainwall_x_position/2.0 > abs(x0 - (double(side)-0.5)*mainwall_x_position) && Gveto_z_position > abs(z0) )
	  {
	    found = true;
	  }
      }
    if(!found && z1 != z2)
      {
	z1 < z2 ? z0 = -Gveto_z_position : z0 = Gveto_z_position; 
	double Cz = (z1-z0)/(z2-z1);
	x0 = x1 - Cz*(x2-x1);
	y0 = y1 - Cz*(y2-y1);
	if( mainwall_x_position/2.0 > abs(x0 - (double(side)-0.5)*mainwall_x_position) && Xwall_y_position > abs(y0) )
	  {
	    found = true;
	  }
      }

    {
      TKpointHdl start_point = std::make_shared<TKpoint>(x0, y0, z0);
      if( distance_2D( *start_point, *track_points[0]) < max_allowed_extrapolation )
	{
	  track_points.insert(track_points.begin(), start_point);
	}
    }
    x0 = 0.0;
    y0 = 0.0; 
    z0 = 0.0;
    x1 = track_points.end()[-1]->get_x();
    y1 = track_points.end()[-1]->get_y();
    z1 = track_points.end()[-1]->get_z();
    x2 = track_points.end()[-2]->get_x();
    y2 = track_points.end()[-2]->get_y();
    z2 = track_points.end()[-2]->get_z();

    found = false;
    if(x1 != x2)
      {
	x1 < x2 ? x0 = mainwall_x_position * (side-1) : x0 = side*mainwall_x_position; 
	double Cx = (x1-x0)/(x2-x1);
	y0 = y1 - Cx*(y2-y1);
	z0 = z1 - Cx*(z2-z1);
	if( Xwall_y_position > abs(y0) && Gveto_z_position > abs(z0) )
	  {
	    found = true;
	  }
      }
    if(!found && y1 != y2)
      {
	y1 < y2 ? y0 = -Xwall_y_position : y0 = Xwall_y_position; 
	double Cy = (y1-y0)/(y2-y1);
	x0 = x1 - Cy*(x2-x1);
	z0 = z1 - Cy*(z2-z1);
	if( mainwall_x_position/2.0 > abs(x0 - (double(side)-0.5)*mainwall_x_position) && Gveto_z_position > abs(z0) )
	  {
	    found = true;
	  }
      }
    if(!found && z1 != z2)
      {
	z1 < z2 ? z0 = -Gveto_z_position : z0 = Gveto_z_position; 
	double Cz = (z1-z0)/(z2-z1);
	x0 = x1 - Cz*(x2-x1);
	y0 = y1 - Cz*(y2-y1);
	if( mainwall_x_position/2.0 > abs(x0 - (double(side)-0.5)*mainwall_x_position) && Xwall_y_position > abs(y0) )
	  {
	    found = true;
	  }
      }

    {
      TKpointHdl end_point = std::make_shared<TKpoint>(x0, y0, z0);
      if( distance_2D( *end_point, *track_points.end()[-1]) < max_allowed_extrapolation )
	{
	  track_points.push_back(end_point);
	}
    }
    return;		
  }

  void TKtrajectory::print(std::ostream & out_) const
  {
    out_ <<"Trajectory: " << endl;
    out_ << "	side: " << side << endl;
    out_ << "	" << segments.size() << " segments: " << endl;
    for(auto i = 0u; i < segments.size(); i++)
      {
	out_ << "	";
	segments[i]->print();
      }
    out_ << "	" << track_points.size() << " track points: " << endl;
    for(auto i = 0u; i < track_points.size(); i++)
      {
	out_ << "	" << i+1 << ". ";
	track_points[i]->print(); 
      }
    return;
  }

} //  end of namespace tkrec
