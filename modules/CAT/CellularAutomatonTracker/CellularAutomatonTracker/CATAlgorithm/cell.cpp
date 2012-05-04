#include <CATAlgorithm/cell.h>
#include <cmath>

namespace CAT{
  namespace topology{

    cell::cell()
    {
      set_print_level(mybhep::NORMAL);
      set_nsigma(10.);
      //ep_ = experimental_point();
      //r0_= experimental_double();
      //r_= experimental_double();
      id_ = default_integer;
      layer_ = default_integer;
      block_ = default_integer;
      iid_ = default_integer;
      n3id_ = default_integer;
      fast_ = true;
      free_ = false;
      begun_ = false; 
      type_ ="SN";
    }

    cell::cell(experimental_point p, experimental_double r, size_t id, bool fast, double nsigma, prlevel level)
    {
      appname_= "cell: ";
      set_print_level(level);
      set_nsigma(nsigma);
      ep_ = p;
      r0_ = r;
      id_ = id;
      layer_ = default_integer;
      block_ = default_integer;
      iid_ = default_integer;
      n3id_ = default_integer;
      fast_ = fast;
      set_radius();
      free_ = false;
      begun_ = false;
      type_ ="SN";
    }

    //! constructor
    cell::cell(experimental_point p, double r, double er, size_t id, bool fast, prlevel level, double nsigma)
    {
      appname_= "cell: ";
      set_print_level(level);
      set_nsigma(nsigma);
      ep_ = p;
      r0_.set_value(r);
      r0_.set_error(er);
      id_ = id;
      layer_ = default_integer;
      block_ = default_integer;
      iid_ = default_integer;
      n3id_ = default_integer;
      fast_ = fast;
      set_radius();
      free_ = false;
      begun_ = false;
      type_ ="SN";
    }

    //! constructor
    cell::cell(experimental_point p, double r, size_t id, bool fast)
    {
      appname_= "cell: ";
      set_print_level(mybhep::NORMAL);
      set_nsigma(10.);
      ep_ = p;
      r0_.set_value(r);
      r0_.set_error(small_neg);
      id_ = id;
      layer_ = default_integer;
      block_ = default_integer;
      iid_ = default_integer;
      n3id_ = default_integer;
      fast_ = fast;
      set_radius();
      free_ = false;
      begun_ = false;
      type_ ="SN";
    }
 
    //! constructor from bhep hit
    cell::cell(mybhep::hit hit, size_t id, bool SuperNemo, prlevel level, double nsigma)
    {
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "cell: ";
      //ep_ = experimental_point(hit);
      r0_.set_value(small_neg);
      r0_.set_error(0.5*mybhep::mm); // radial error 

      if( hit.find_property("DIST")){
        double rr = mybhep::double_from_string(hit.fetch_property("DIST"));
        if( isnan(rr) || isinf(rr) ) rr = small_neg;
        r0_.set_value(rr);
      }

      if( hit.find_property("SLOW"))
        fast_ = false;
      else
        fast_ = true;

      id_ = id;


      if( SuperNemo )
        {
          type_ ="SN";
          string value = hit.fetch_property("CELL"); // GG_CELL_block_plane_id
          // block = 1, 2, 3 or -1, -2, -3
          // layer = 0, 1, ..., 8 or 0, -1, ..., -8
          
          
          sscanf(value.c_str(),"GG_CELL_%d_%d_%d",&block_,&layer_,&iid_);
          layer_ --;
          if( block_ < 0 ){
            layer_ *= -1;
          }
          n3id_ = 0;
          
        }
      else
        {
          type_ ="N3";
          string value = hit.fetch_property("BLK");  // BLK = sector_io_layer
          // sector = petal of the detector
          // io = 1 if hit is between foil and external calorimeter
          //     0 if hit is between foil and internal calorimeter
          // layer = 0-8
      
          size_t io;
          sscanf(value.c_str(),"%d_%d_%d",&block_,&io,&iid_);
 
          layer_ = iid_;

          //translate layer into block number
          if (layer_<4)
            block_ = 1;  // 0, 1, 2, 3
          else if(layer_ >=4 && layer_ <6)
            block_ = 2; // 4, 5
          else
            block_ = 3;  // 6, 7, 8

          if( io == 0 ){
            layer_ *= -1;
            block_ *= -1;
          }
          // block = 1, 2, 3 or -1, -2, -3
          // layer = 0, 1, ..., 8 or 0, -1, ..., -8

          string val = hit.fetch_property("CELL");  // cell number
          sscanf(val.c_str(),"%d",&n3id_);
        }

      set_radius();
      free_ = false;
      begun_ = false;
    }

    //! constructor from bhep hit
    cell::cell(mybhep::hit hit)
    {
      appname_= "cell: ";
      experimental_point ep_tmp(hit);
      ep_ = ep_tmp;
      r0_.set_value( small_neg);
      r0_.set_error( 0.5*mybhep::mm); // radial error 

      if( hit.find_property("DIST")){
        double rr = mybhep::double_from_string(hit.fetch_property("DIST"));
        if( isnan(rr) || isinf(rr) ) rr = small_neg;
        r0_.set_value(rr);
      }

      if( hit.find_property("SLOW"))
        fast_ = false;
      else
        fast_ = true;

      id_ = default_integer;
      layer_ = default_integer;
      block_ = default_integer;
      iid_ = default_integer;
      n3id_ = default_integer;
      set_radius();
      free_ = false;
      begun_ = false;
      type_ ="SN";
    }

    /*** dump ***/
    void cell::dump (ostream & a_out,
                     const string & a_title,
                     const string & a_indent,
                     bool a_inherit) const
    {
      {
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " -------------- " << endl;
        a_out << indent << "id : " << this->id() << " layer " << this->layer() << " block " << this->block() << " iid " << this->iid() << " n3id " << this->n3id() << " fast " << this->fast() << " small " << this->small() << " unknown vertical " << this->unknown_vertical() << endl;
        a_out << indent << " point " << endl;
        this->ep().dump(a_out,"", indent + "   ");
        a_out << indent << "radius : "; (r()/mybhep::mm).dump(); a_out << " [mm ] " << endl;
        if( small() && fast() ){
          a_out << indent << "original radius : "; (r0()/mybhep::mm).dump(); a_out << " [mm ] " << endl;
        }
        a_out << indent << " -------------- " << endl;

        return;
      }
    }

    //! set experimental_point, radius, error and id;
    void cell::set(const experimental_point & p, double r,double er, size_t id, bool fast)
    {
      ep_ = p;
      r0_.set_value(r); 
      r0_.set_error(er);
      id_ = id;
      fast_ = fast;
      set_radius();
    }

    //! set experimental_point
    void cell::set_p(const experimental_point & p)
    {
      ep_ = p;
    }

    //! set radius
    void cell::set_r(double r)
    {
      r0_.set_value(r);
      set_radius();
    }

    //! set radius error
    void cell::set_er(double er)
    {
      r0_.set_error(er);
      set_radius();
    }

    //! set id
    void cell::set_id(size_t id)
    {
      id_ = id;
    }

    //! set layer
    void cell::set_layer(size_t layer)
    {
      layer_ = layer;
    }

    //! set block
    void cell::set_block(size_t block)
    {
      block_ = block;
    }

    //! set iid
    void cell::set_iid(size_t iid)
    {
      iid_ = iid;
    }

    //! set n3id
    void cell::set_n3id(size_t n3id)
    {
      n3id_ = n3id;
    }

    //! set fast flag
    void cell::set_fast(bool fast)
    {
      fast_ = fast;
    }


    //! set free level
    void cell::set_free(bool free){
      free_ = free;
    }

    //! set begun level
    void cell::set_begun(bool begun){
      begun_ = begun;
    }

    //! set type
    void cell::set_type(string type){
      type_ = type;
    }


    bool cell::small() const
    {
      bool sm = false;

      if (r0_.value() <= nsigma()*r0_.error() ) sm = true;

      return sm;
    }

    bool cell::unknown_vertical() const
    {
      bool uv = false;

      if( ep().y().value() == 0. &&
          ep().y().error() > 1000. ) uv = true;

      return uv;
    }

    //! get experimental_point
    const experimental_point& cell::ep()const
    {
      return ep_;
    }      

    //!get experimental r
    const experimental_double& cell::r() const 
    {
      return r_;
    } 

    //!get original experimental r
    const experimental_double& cell::r0() const 
    {
      return r0_;
    } 

    //!get id
    const size_t& cell::id() const {return id_;} 

    //!get layer
    const int& cell::layer() const {return layer_;} 

    //!get block
    const int& cell::block() const {return block_;} 

    //!get iid
    const int& cell::iid() const {return iid_;} 

    //!get n3id
    const int& cell::n3id() const {return n3id_;} 

    //!get fast flag
    const bool& cell::fast() const {return fast_;} 

    //! get free level
    const bool cell::free()const{
      return free_;
    }

    //! get begun level
    const bool cell::begun()const{
      return begun_;
    }

    //! get type
    string cell::type(){
      return type_;
    }

    //! get cell number
    int cell::cell_number(){

      if( type() == "SN" )
        {
          return iid();
        }
      else if( type() == "N3" )
        {
          // for Nemo3, cell number repeats within each sector
          // cell numbers vary from 0 to N in each layer of each block, where:
          // block -3 ... N = 11
          // block -2 ... N = 13
          // block -1 ... N = 15
          // block 1 ... N = 17
          // block 2 ... N = 19
          // block 3 ... N = 22

          // so to distinguish cells of different sectors I add 100*sector number 

          return n3id() + 100*block();

        }
      
      if( print_level() >= mybhep::NORMAL )
        std::clog << " problem: unknown cell type " << type_ << endl;
      return 0;

    }

    experimental_double cell::distance(const cell & c) const{
    
      experimental_vector v(ep(),c.ep());

      return v.length();
    
    }

    bool cell::operator<(const topology::cell& c) const
    {

      if( id_ > default_integer ){
        if( print_level() >= mybhep::NORMAL )
          std::clog << "cell::operator<: problem: trying to compare cells with ids " << id_ << " and " << c.id() << " just returning false " << endl;
        return false;
      }

      if( this->id() == c.id() ) return false;

      // side of foil
      if( this->block() < 0 && c.block() > 0 ){
        return false;
      }

      // layer
      if(fabs(this->layer()) < fabs(c.layer())){
        return false;
      }
      if(fabs(this->layer()) > fabs(c.layer()))
        return true;

      // iid
      if(this->iid() < c.iid()){
        return false;
      }
      if(this->iid() > c.iid())
        return true;
      

      if(this->n3id() <= c.n3id()){
        return false;
      }


      return true;
    
    }

    bool cell::compare(const topology::cell& c1, const topology::cell& c) {

      // side of foil
      if( c1.block() < 0 && c.block() > 0 ){
        return false;
      }

      // layer
      if(fabs(c1.layer()) < fabs(c.layer())){
        return false;
      }
      if(fabs(c1.layer()) > fabs(c.layer()))
        return true;

      // iid
      if(c1.iid() < c.iid()){
        return false;
      }
      if(c1.iid() > c.iid())
        return true;
      

      if(c1.n3id() < c.n3id()){
        return false;
      }


      return true;
    
    }

    void cell::set_radius(){
      r_ = r0_;
      if( small() && fast() )
        r_.set_error(max(r0_.value(), r0_.error()));
    }


    experimental_point cell::build_from_cell(const experimental_vector & forward, 
                                             const experimental_vector & transverse, 
                                             const experimental_double & cos, int sign) const
    {

      experimental_double sin = experimental_sin(experimental_acos(cos))*sign;

      experimental_point p = (experimental_vector(ep()) + r()*(forward*cos + transverse*sin)).point_from_vector();

      p.set_ex(error_x_in_build_from_cell(forward, transverse, cos, sin.value()));
      p.set_ez(error_z_in_build_from_cell(forward, transverse, cos, sin.value()));

      return p;
    }


    double cell::error_x_in_build_from_cell(const experimental_vector & forward, 
                                            const experimental_vector & transverse, 
                                            const experimental_double & cos, 
                                            double sin) const
    {
      //
      // must set manually the error because factors are not independent
      //
      // p = p0 + R (F c + T s)
      //
      // so:
      // sigma_p2 = (Fc + Ts)2 sigma_R2 + R2 (F - T/t)2 sigma_c2
      //

      double fax = forward.x().value() * cos.value() + transverse.x().value() * sin ;
      double fbx = r().value()*(forward.x().value() - transverse.x().value() * cos.value() / sin );

      return sqrt(square(fax*r().error()) + square(fbx*cos.error()));
    }

    double cell::error_z_in_build_from_cell(const experimental_vector & forward, 
                                            const experimental_vector & transverse, 
                                            const experimental_double & cos, 
                                            double sin) const
    {
      double faz = forward.z().value() * cos.value() + transverse.z().value() * sin ;
      double fbz = r().value()*(forward.z().value() - transverse.z().value() * cos.value() / sin );
      return sqrt(square(faz*r().error()) + square(fbz*cos.error()));
    }



    experimental_point cell::angular_average(const experimental_point & epa, 
                                             const experimental_point & epb, 
                                             experimental_double* angle)
    {
      if( print_level() >= mybhep::VVERBOSE )
        clog << " calculating angular average for cell " << id() << endl;

      if( small() ){
        angle->set_value(0.);
        angle->set_error(0.1); // fictitious value to avoid divergences
        /*
          experimental_double avx = (epa.x() + epb.x())/2.;
          experimental_double avy = (epa.y() + epb.y())/2.;
          experimental_double avz = (epa.z() + epb.z())/2.;
          experimental_point average(avx, avy, avz);
          return average;
        */
        return ep();
      }

      experimental_double phi1 = experimental_vector(ep(), epa).phi();
      experimental_double phi2 = experimental_vector(ep(), epb).phi();

      double rephi1 = phi1.value();
      double rephi2 = phi2.value();

      fix_angles(&rephi1, &rephi2);

      phi1.set_value(rephi1);
      phi2.set_value(rephi2);

      experimental_double ave_phi = (phi1 + phi2)/2.;
      *angle = phi1 - phi2;

      if( print_level() >= mybhep::VVERBOSE ){
        clog << " avereging phi1: "; (phi1*180./M_PI).dump();
        clog << " phi2: "; (phi2*180./M_PI).dump();
        clog << " phi_ave: "; (ave_phi*180./M_PI).dump();
        clog << " " << endl;
      }

      experimental_double cos_ave_phi = experimental_cos(ave_phi);

      experimental_vector x(1.,0.,0.,0.,0.,0.);
      experimental_vector z(0.,0.,1.,0.,0.,0.);
      int sign = ( ave_phi.value() < 0. ) ? -1 : +1; 

      experimental_point p = build_from_cell(x, z, cos_ave_phi, sign);

      return p;

    }


    void cell::dump_point(const experimental_point & epp)const{

      experimental_vector v(ep(), epp);

      clog << " x: "; (v.x()).dump();
      clog << " y: "; (v.y()).dump();
      clog << " z: "; (v.z()).dump();
      clog << " phi: "; (v.phi()*180./M_PI).dump();

      return;
    }


    bool cell::same_quadrant(const experimental_point & epa, const experimental_point & epb)const{
      // check if the angular position of points epa and epb
      // is less than 90 degrees apart around the cell

      experimental_double initial_phi1 = experimental_vector(ep(), epa).phi();
      experimental_double initial_phi2 = experimental_vector(ep(), epb).phi();

      double re_initial_phi1 = initial_phi1.value();
      double re_initial_phi2 = initial_phi2.value();
      fix_angles(&re_initial_phi1, &re_initial_phi2);

      if( fabs(re_initial_phi1 - re_initial_phi2) > 1.57 ) return false;

      return true;

    }

    bool cell::same_cell(const topology::cell & c)const{

      if( block() == c.block() &&
          layer() == c.layer() &&
          iid() == c.iid() &&
          n3id() == c.n3id() ) 
        return true;

      return false;

    }

    bool cell::intersect(const topology::cell & c) const{

      double dist = experimental_vector(ep(), c.ep()).hor().length().value();
      experimental_double rsum = r() + c.r();

      if( rsum.value() > dist ){
        if( print_level() >= mybhep::VVERBOSE ){
          clog << " cells " << id() << " and " << c.id() << " intersect: dist " << dist << " radii " << r().value() << " and " << c.r().value() << " rsum " << rsum.value() << endl;
        }
        return true;
      }

      bool test = false;

      /*
        double chi2 = square((rsum.value() - dist)/rsum.error());

        bool test = probof(chi2, 1) > prob();
        if( test )
        if( print_level() >= mybhep::VVERBOSE ){
        clog <<" cells " << id() << " and " <<c.id() << " intersect: dist " << dist << " radii " << r().value() << " and " <<c.r().value()<< " rsum " << rsum.value() << " +- " << rsum.error() << " prob " << probof(chi2, 1) << " chi2 " << chi2 << " problimit " << prob() << endl;
        }
      */

      return test;

    }

  
  }
}

