/* -*- mode: c++ -*- */
#ifndef __sultan__ICELL
#define __sultan__ICELL
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/tracking_object.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>

namespace SULTAN {

  namespace topology {


    class cell : public tracking_object {

      // a cell is composed of an experimental point
      // and an experimental radius

    private:
      std::string appname_;

      // experimental point
      experimental_point ep_;

      // radius
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
        set_probmin(10.);
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
      cell(experimental_point &p, experimental_double r, size_t id, bool fast=true, double probmin=1.e-200, mybhep::prlevel level=mybhep::NORMAL){
        appname_= "cell: ";
        set_print_level(level);
        set_probmin(probmin);
        ep_ = p;
        r_ = r;
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //! constructor
      cell(experimental_point &p, double r, double er, size_t id, bool fast=true, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200){
        appname_= "cell: ";
        set_print_level(level);
        set_probmin(probmin);
        ep_ = p;
        r_.set_value(r);
        r_.set_error(er);
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      //! constructor
      cell(experimental_point &p, double r, size_t id, bool fast=true){
        appname_= "cell: ";
        set_print_level(mybhep::NORMAL);
        set_probmin(10.);
        ep_ = p;
        r_.set_value(r);
        r_.set_error(mybhep::small_neg);
        id_ = id;
        layer_ = mybhep::default_integer;
        block_ = mybhep::default_integer;
        iid_ = mybhep::default_integer;
        n3id_ = mybhep::default_integer;
        fast_ = fast;
        free_ = false;
        begun_ = false;
        type_ ="SN";
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool /* a_inherit */          = false) const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << " -------------- " << std::endl;
          a_out << indent << "id : " << this->id() << " layer " << this->layer() << " block " << this->block() << " iid " << this->iid() << " n3id " << this->n3id() << " fast " << this->fast() << " unknown vertical " << this->unknown_vertical() << std::endl;
          a_out << indent << " point " << std::endl;
          this->ep().dump(a_out,"", indent + "   ");
          a_out << indent << "radius : "; r().dump(); a_out << std::endl;

          return;
        }
      }



      //! set experimental_point, radius, error and id;
      void set(experimental_point p, double r,double er, size_t id, bool fast)
      {
        ep_ = p;
        r_.set_value(r);
        r_.set_error(er);
        id_ = id;
        fast_ = fast;
      }

      //! set experimental_point
      void set_p(experimental_point p)
      {
        ep_ = p;
      }

      //! set radius
      void set_r(double r)
      {
        r_.set_value(r);
      }

      //! set radius error
      void set_er(double er)
      {
        r_.set_error(er);
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
      void set_block(int block)
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

      //! set type
      void set_type(std::string type){
        type_ = type;
      }

      bool unknown_vertical() const
      {
        bool uv = false;

        if( ep().z().value() == 0. &&
            ep().z().error() > 1000. ) uv = true;

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

      //! get type
      std::string type(){
        return type_;
      }

      //! get cell number
      int cell_number() const{

        if( type_ == "SN" )
          {
            return iid();
          }
        else if( type_ == "N3" )
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
      void dump_point(experimental_point ep) const;
      void dump_point_phi(experimental_point ep) const;
      bool same_cell(topology::cell c) const;
      size_t near_level( const topology::cell & cell, double nofflayers, double cell_distance )const;
      bool is_near_foil()const;
      bool is_near_calo()const;
      bool is_near_xcalo()const;
      bool is_near_gveto()const;


      bool operator<(const topology::cell& c) const{

        if( id_ > mybhep::default_integer || c.id() > mybhep::default_integer ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: trying to compare cells with ids " << id_ << " and " << c.id() << " just returning false " << std::endl;
          return false;
        }

        if( this->id() == c.id() ) return false;

        // side of foil
        if( this->block() < 0 && c.block() > 0 ){
          return false;
        }
        if( this->block() > 0 && c.block() < 0 )
          return true;


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
        if(this->iid() > c.iid()){
          return true;
        }

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

    };
  }
}

#endif
