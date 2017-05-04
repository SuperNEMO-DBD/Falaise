/* -*- mode: c++ -*- */
#ifndef __sultan__cell_triplet_h
#define __sultan__cell_triplet_h

#include <sultan/tracking_object.h>
#include <string>
#include <iostream>
#include <vector>
#include <sultan/cell.h>
#include <sultan/cell_couplet.h>
#include <sultan/experimental_helix.h>

namespace SULTAN{
  namespace topology{


    class cell_triplet : public tracking_object{

      // a cell_triplet is composed of three cells
      // and a list of joints

    protected:

      std::string appname_;

      // first cell
      cell ca_;

      // second cell
      cell cb_;

      // third cell
      cell cc_;

    public:

      std::vector<experimental_helix> helices_;

      //!Default constructor
      cell_triplet();

      //!Default destructor
      virtual ~cell_triplet();

      //! constructor
      cell_triplet(const cell &ca, const cell &cb, const cell &cc, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set cells
      void set(const cell_couplet &cca, const cell_couplet &ccb);

      //! set cells
      void set(const cell &ca, const cell &cb, const cell &cc);

      //! get first cell couplet
      cell_couplet cca();

      //! get second cell couplet
      cell_couplet ccb();

      //! get first cell
      const cell& ca()const;

      //! get second cell
      const cell& cb()const;

      //! get third cell
      const cell& cc()const;

    public:

      const std::vector<experimental_helix> helices()const{ return helices_;}


      void calculate_helices(double Rmin, double Rmax, double nsigmas);

      cell_triplet invert();

      friend bool operator==(const cell_triplet& left,
                             const cell_triplet& right);

      bool has_cell_of_id(size_t id)const;

      void print_ids()const{
        std::clog << " ( " << ca_.id() << ", " << cb_.id() << ", " << cc_.id() << ") ";
      }

    };


  }

}

#endif // __sultan__cell_triplet_h
