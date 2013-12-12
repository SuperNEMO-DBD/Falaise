/* -*- mode: c++ -*- */

#ifndef __sultan__node_h
#define __sultan__node_h 1

#include <iostream>
#include <vector>
#include <string>
#include <mybhep/utilities.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <algorithm>
#include <map>

namespace SULTAN {
  namespace topology{


    class node : public tracking_object {

      // a node is composed of a cell
      // and a fitted point

    protected:
      std::string appname_;

    public:

      // main cell
      cell c_;

      // fitted point
      experimental_point ep_;

      // angle along circle
      double circle_phi_;

    public:

      //!Default constructor
      node();

      //!Default destructor
      virtual ~node();

      //! constructor
      node(const cell &c, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set main cell
      void set_c(const cell& c);

      //! set fitted experimental_point
      void set_ep( const experimental_point &ep );

      //! set circle phi
      void set_circle_phi(double a){circle_phi_=a;}

      //! get main cell
      const cell& c()const;

      //! get fitted experimental_point
      const experimental_point& ep()const;

      //! get circle phi
      const double circle_phi()const{return circle_phi_;}

      node invert();

      friend bool operator==(const node& left,
                             const node& right);

      static bool circle_order(const topology::node& c1, const topology::node& c) {
	// order nodes based on their angle along an assigned circle

        return( c1.circle_phi() > c.circle_phi() );
      }
      


  };
}
}


#endif // __sultan__node_h
