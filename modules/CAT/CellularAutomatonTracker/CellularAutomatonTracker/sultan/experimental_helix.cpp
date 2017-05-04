
#include <sultan/experimental_helix.h>
#include <algorithm>

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


    void experimental_helix::add_ids(std::vector<size_t> ids){
      for(std::vector<size_t>::const_iterator fid = ids.begin(); fid!=ids.end(); ++fid)
        add_id(*fid);
    }


    void experimental_helix::distance_from_cell_measurement(topology::cell c, experimental_double *DR, experimental_double *DH)const{
       //////////////////////////////////////////////////////////////////////////
      //   center_of_helix              radius_of_helix    center_of_cell
      //          O                            )(                ep
      //          ------------- R -------------><-------- r ------
      //          -------------- delta -------------------------->
      //          ---------------- dr1 -------------------------->
      //          --- dr2 --><-------- r ------
      //
      //   center_of_helix              center_of_cell    radius_of_helix
      //          O                            ep                ))
      //          ----------- delta ----------->--------- r ----->
      //          ------------------- R ------------------------->
      //          ---------------- dr1 ----------------------------------- r ----->
      //          ------------ dr2 ------------>
      //////////////////////////////////////////////////////////////////////////
      experimental_vector delta = c.ep() - this->center();
      experimental_double delta_hor = delta.hor().length();
      experimental_double dr1 = R() + c.r();
      experimental_double dr2 = R() - c.r();
      experimental_double dr = experimental_min(
                                                experimental_fabs(delta_hor - dr1),
                                                experimental_fabs(delta_hor - dr2)
                                                );

      experimental_double phi = delta.phi();
      experimental_double dh = z0() + H()*phi - c.ep().z();

      *DR = dr;
      *DH = experimental_fabs(dh);

      return;
    }

    void experimental_helix::distance_from_cell_center(topology::cell c, experimental_double *DR, experimental_double *DH)const{
      //////////////////////////////////////////////////////////////////////////
      //   center_of_helix              radius_of_helix    center_of_cell
      //          O                            )(                ep
      //          ------------- R ------------->
      //          -------------- delta -------------------------->
      //
      //   center_of_helix              center_of_cell    radius_of_helix
      //          O                            ep                ))
      //          ----------- delta ----------->
      //          ------------------- R ------------------------->
      //////////////////////////////////////////////////////////////////////////
      experimental_vector delta = c.ep() - this->center();
      experimental_double delta_hor = delta.hor().length();
      experimental_double phi = delta.phi();
      experimental_double dh = z0() + H()*phi - c.ep().z();

      *DR = experimental_fabs(delta_hor - this->R());
      *DH = experimental_fabs(dh);

      return;
    }

    void experimental_helix::get_phi_of_point(topology::experimental_point input_p, topology::experimental_point * p, double * angle){

      *p = this->position(input_p);
      // angle of cell center wrt circle center
      *angle = atan2(
                     input_p.y().value() - this->y0().value(),
                     input_p.x().value() - this->x0().value()
                     );
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

    bool experimental_helix::isnan()const{
      return (
              this->x0().experimental_isnan() ||
              this->y0().experimental_isnan() ||
              this->z0().experimental_isnan() ||
              this->R().experimental_isnan() ||
              this->H().experimental_isnan() );
    }

    bool experimental_helix::isinf()const{
      return (
              this->x0().experimental_isinf() ||
              this->y0().experimental_isinf() ||
              this->z0().experimental_isinf() ||
              this->R().experimental_isinf() ||
              this->H().experimental_isinf() );
    }

    experimental_vector experimental_helix::direction_at(const experimental_point & ep)const{
      
      const experimental_double phi = phi_of_point(ep, 0.);
      
      experimental_vector dir(-R() * experimental_sin(phi),
			      R() * experimental_cos(phi),
			      H());
      
      return dir.unit();
      
    }


  }

}
