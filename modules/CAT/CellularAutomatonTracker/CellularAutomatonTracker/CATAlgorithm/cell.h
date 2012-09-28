/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICELL
#define __CATAlgorithm__ICELL
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>

namespace CAT {

  namespace topology {


    class cell : public tracking_object {

      // a cell is composed of an experimental point
      // and an experimental radius

    private:
      std::string appname_;

      // experimental point
      experimental_point ep_;

      // radius (original value)
      experimental_double r0_;

      // radius (modified value if the cell is "small")
      experimental_double r_;

      // id
      size_t id_;

      // characterize fast and delayed cells
      bool fast_;

      // layer number
      int layer_;

      // block number
      int block_;

      // iid number
      int iid_;

      // n3id number
      int n3id_;

      // N3 or SN
      std::string type_;

    public:


      // status of cell couplet
      bool free_;

      // begun cell couplet
      bool begun_;

      //!Default constructor
      cell()
      {
        set_print_level(mybhep::NORMAL);
        set_nsigma(10.);
        //ep_ = experimental_point();
        //r0_= experimental_double();
        //r_= experimental_double();
        id_ = mybhep::default_integer;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = true;
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //!Default destructor
      virtual ~cell(){};

      //! constructor
      cell(experimental_point p, experimental_double r, size_t id, bool fast=true, double nsigma=10., mybhep::prlevel level=mybhep::NORMAL){
        appname_= "cell: ";
        set_print_level(level);
        set_nsigma(nsigma);
        ep_ = p;
        r0_ = r;
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        set_radius();
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //! constructor
      cell(experimental_point p, double r, double er, size_t id, bool fast=true, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        appname_= "cell: ";
        set_print_level(level);
        set_nsigma(nsigma);
        ep_ = p;
        r0_.set_value(r);
        r0_.set_error(er);
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        set_radius();
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //! constructor
      cell(experimental_point p, double r, size_t id, bool fast=true){
        appname_= "cell: ";
        set_print_level(mybhep::NORMAL);
        set_nsigma(10.);
        ep_ = p;
        r0_.set_value(r);
        r0_.set_error(mybhep::small_neg);
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        set_radius();
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //! constructor from bhep hit
      cell(mybhep::hit hit, size_t id, bool SuperNemo, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "cell: ";
        //ep_ = experimental_point(hit);
        r0_.set_value(mybhep::small_neg);
        r0_.set_error(0.5*mybhep::mm); // radial error

        if( hit.find_property("DIST")){
          double rr = mybhep::double_from_string(hit.fetch_property("DIST"));
          if( isnan(rr) || isinf(rr) ) rr = mybhep::small_neg;
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
            std::string value = hit.fetch_property("CELL"); // GG_CELL_block_plane_id
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
            std::string value = hit.fetch_property("BLK");  // BLK = sector_io_layer
            // sector = petal of the detector
            // io = 1 if hit is between foil and external calorimeter
            //     0 if hit is between foil and internal calorimeter
            // layer = 0-8

            size_t io;
            sscanf(value.c_str(),"%d_%u_%d",&block_,&io,&iid_);

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

            std::string val = hit.fetch_property("CELL");  // cell number
            sscanf(val.c_str(),"%d",&n3id_);
          }

        set_radius();
        free_ = false;
        begun_ = false;
      }

      //! constructor from bhep hit
      cell(mybhep::hit hit){
        appname_= "cell: ";
        experimental_point ep_tmp(hit);
        ep_ = ep_tmp;
        r0_.set_value( mybhep::small_neg);
        r0_.set_error( 0.5*mybhep::mm); // radial error

        if( hit.find_property("DIST")){
          double rr = mybhep::double_from_string(hit.fetch_property("DIST"));
          if( isnan(rr) || isinf(rr) ) rr = mybhep::small_neg;
          r0_.set_value(rr);
        }

        if( hit.find_property("SLOW"))
          fast_ = false;
        else
          fast_ = true;

        id_ = mybhep::default_integer;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        set_radius();
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }


      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << " -------------- " << std::endl;
          a_out << indent << "id : " << this->id() << " layer " << this->layer() << " block " << this->block() << " iid " << this->iid() << " n3id " << this->n3id() << " fast " << this->fast() << " small " << this->small() << " unknown vertical " << this->unknown_vertical() << std::endl;
          a_out << indent << " point " << std::endl;
          this->ep().dump(a_out,"", indent + "   ");
          a_out << indent << "radius : "; (r()/mybhep::mm).dump(); a_out << " [mm ] " << std::endl;
          if( small() && fast() ){
            a_out << indent << "original radius : "; (r0()/mybhep::mm).dump(); a_out << " [mm ] " << std::endl;
          }
          a_out << indent << " -------------- " << std::endl;

          return;
        }
      }



      //! set experimental_point, radius, error and id;
      void set(experimental_point p, double r,double er, size_t id, bool fast)
      {
        ep_ = p;
        r0_.set_value(r);
        r0_.set_error(er);
        id_ = id;
        fast_ = fast;
        set_radius();
      }

      //! set experimental_point
      void set_p(experimental_point p)
      {
        ep_ = p;
      }

      //! set radius
      void set_r(double r)
      {
        r0_.set_value(r);
        set_radius();
      }

      //! set radius error
      void set_er(double er)
      {
        r0_.set_error(er);
        set_radius();
      }

      //! set id
      void set_id(size_t id)
      {
        id_ = id;
      }

      //! set layer
      void set_layer(size_t layer)
      {
        layer_ = layer;
      }

      //! set block
      void set_block(size_t block)
      {
        block_ = block;
      }

      //! set iid
      void set_iid(size_t iid)
      {
        iid_ = iid;
      }

      //! set n3id
      void set_n3id(size_t n3id)
      {
        n3id_ = n3id;
      }

      //! set fast flag
      void set_fast(bool fast)
      {
        fast_ = fast;
      }


      //! set free level
      void set_free(bool free){
        free_ = free;
      }

      //! set begun level
      void set_begun(bool begun){
        begun_ = begun;
      }

      //! set type
      void set_type(std::string type){
        type_ = type;
      }


      bool small() const
      {
        bool sm = false;

        //        if (r0_.value() <= nsigma()*r0_.error() ) sm = true;
        if (r0_.value() <= 2. ) sm = true;

        return sm;
      }

      bool unknown_vertical() const
      {
        bool uv = false;

        if( ep().y().value() == 0. &&
            ep().y().error() > 1000. ) uv = true;

        return uv;
      }

      //! get experimental_point
      const experimental_point& ep()const
      {
        return ep_;
      }

      //!get experimental r
      const experimental_double& r() const
      {
        return r_;
      }

      //!get original experimental r
      const experimental_double& r0() const
      {
        return r0_;
      }

      //!get id
      const size_t& id() const {return id_;}

      //!get layer
      const int& layer() const {return layer_;}

      //!get block
      const int& block() const {return block_;}

      //!get iid
      const int& iid() const {return iid_;}

      //!get n3id
      const int& n3id() const {return n3id_;}

      //!get fast flag
      const bool& fast() const {return fast_;}

      //! get free level
      const bool free()const{
        return free_;
      }

      //! get begun level
      const bool begun()const{
        return begun_;
      }

      //! get type
      std::string type(){
        return type_;
      }

      //! get cell number
      int cell_number(){

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
          std::clog << " problem: unknown cell type " << type_ << std::endl;
        return 0;

      }


    public:

      experimental_double distance(cell c) const;
      experimental_point angular_average(experimental_point epa, experimental_point epb, experimental_double* angle);
      experimental_point build_from_cell(experimental_vector forward, experimental_vector transverse, experimental_double cos, int sign);
      void dump_point(experimental_point ep) const;
      void dump_point_phi(experimental_point ep) const;
      double error_x_in_build_from_cell(experimental_vector forward, experimental_vector transverse, experimental_double cos, double sin)const;
      double error_z_in_build_from_cell(experimental_vector forward, experimental_vector transverse, experimental_double cos, double sin)const;
      bool same_quadrant(experimental_point epa, experimental_point epb) const;
      bool same_cell(topology::cell c) const;
      bool intersect(topology::cell c) const;



      bool operator<(const topology::cell& c) const{

        if( id_ > mybhep::default_integer ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: trying to compare cells with ids " << id_ << " and " << c.id() << " just returning false " << std::endl;
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

      static bool compare(const topology::cell& c1, const topology::cell& c) {

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

      };

    private:
      void set_radius(){
        r_ = r0_;
        if( small() && fast() )
          r_.set_error(std::max(r0_.value(), r0_.error()));
      }


    };
  }
}

#endif
