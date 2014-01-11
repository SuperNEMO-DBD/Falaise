#include <sultan/experimental_helix.h>

namespace SULTAN {
  namespace topology{


    experimental_point experimental_helix::position(const experimental_point &ep)const{
      
      experimental_vector delta = ep - this->center();
      
      experimental_double phi = delta.phi();
      
      experimental_double px = this->x0() + this->R()*experimental_cos(phi);
      experimental_double py = this->y0() + this->R()*experimental_sin(phi);
      experimental_double pz = this->z0() + this->H()*phi;
      experimental_point point(px, py, pz);
      
      return point;
      
    }

    void experimental_helix::add_id(size_t id){
      std::vector<size_t>::const_iterator fid = std::find(ids_.begin(), ids_.end(), id);
      if( fid == ids_.end() ){
	ids_.push_back(id);
      }
    }
    
    
    void experimental_helix::distance_from_cell_measurement(topology::cell c, experimental_double *DR, experimental_double *DH)const{
      experimental_vector delta = c.ep() - this->center();
      experimental_double dr_center = delta.hor().length();
      experimental_double dr1 = R() + c.r();
      experimental_double dr2 = R() - c.r();
      experimental_double dr;
      if( fabs(dr_center.value() - dr1.value()) < fabs(dr_center.value() - dr2.value()) )
	dr = experimental_fabs(dr_center - dr1);
      else
	dr = experimental_fabs(dr_center - dr2);
      
      experimental_double phi = delta.phi();
      experimental_double dh = z0() + H()*phi - c.ep().z();
      
      *DR = dr;
      *DH = dh;
      
      return;
    }
    
    void experimental_helix::distance_from_cell_center(topology::cell c, experimental_double *DR, experimental_double *DH)const{
      experimental_vector delta = c.ep() - this->center();
      experimental_double dr_center = delta.hor().length();
      experimental_double phi = delta.phi();
      experimental_double dh = z0() + H()*phi - c.ep().z();
      
      *DR = experimental_fabs(dr_center - R());
      *DH = experimental_fabs(dh);
      
      return;
    }
    
    bool experimental_helix::different_cells(topology::experimental_helix b)const{
      std::vector<size_t> bids = b.ids();
      for(std::vector<size_t>::const_iterator id = ids_.begin(); id !=ids_.end(); ++id){
	if( std::find(bids.begin(), bids.end(), *id) == bids.end() )
	  return true;
      }
      
      for(std::vector<size_t>::const_iterator id = bids.begin(); id !=bids.end(); ++id){
	if( std::find(ids_.begin(), ids_.end(), *id) == ids_.end() )
	  return true;
      }
      
      return false;
      
    }

    bool experimental_helix::is_less_than__optimist(const topology::experimental_helix a, double nsigmas=1)const{

      //
      // is this less than a?
      // each parameter must be less than a (optimistically)

      if( !this->x0().is_less_than__optimist(a.x0(), nsigmas) ) return false;
      if( !this->y0().is_less_than__optimist(a.y0(), nsigmas) ) return false;
      if( !this->z0().is_less_than__optimist(a.z0(), nsigmas) ) return false;
      if( !this->R().is_less_than__optimist(a.R(), nsigmas) ) return false;
      if( !this->H().is_less_than__optimist(a.H(), nsigmas) ) return false;
      return true;

    }
    
    bool experimental_helix::is_more_than__optimist(const topology::experimental_helix a, double nsigmas=1)const{

      //
      // is this more than a?
      // each parameter must be more than a (optimistically)

      if( !this->x0().is_more_than__optimist(a.x0(), nsigmas) ) return false;
      if( !this->y0().is_more_than__optimist(a.y0(), nsigmas) ) return false;
      if( !this->z0().is_more_than__optimist(a.z0(), nsigmas) ) return false;
      if( !this->R().is_more_than__optimist(a.R(), nsigmas) ) return false;
      if( !this->H().is_more_than__optimist(a.H(), nsigmas) ) return false;

      return true;

    }
    
  }

}

