/* -*- mode: c++ -*- */

#include <sultan/node.h>

namespace SULTAN {
  namespace topology{

    using namespace std;
    using namespace mybhep;

      //!Default constructor
    node::node()
      {
        appname_= "node: ";
      }

      //!Default destructor
      node::~node()
      {
        return;
      }

      //! constructor
      node::node(const cell &c, prlevel level, double probmin){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "node: ";
        c_ = c;
        circle_phi_ = mybhep::small_neg;
      }

      /*** dump ***/
      void node::dump (ostream & a_out,
                       const std::string & a_title,
                       const std::string & a_indent,
                       bool /* a_inherit */) const{
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " --------------------- " << std::endl;
        a_out << indent  << " main cell ";
        this->c().dump(a_out,"",indent + "   ");
        a_out << indent << " fitted point: "; ep().dump();

        return;
      }

      //! set main cell
      void node::set_c(const cell& c){
        c_ = c;
      }

      //! set fitted experimental_point
      void node::set_ep( const experimental_point &ep )
      {
        ep_ = ep;
      }

      //! get main cell
      const cell& node::c()const
      {
        return c_;
      }

      //! get fitted experimental_point
      const experimental_point& node::ep()const
      {
        return ep_;
      }

      bool operator==(const node& left,
                      const node& right)
      {

        return left.c().id() == right.c().id();

      }

  }
}
