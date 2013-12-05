/* -*- mode: c++ -*- */
#ifndef __sultan__ISCENARIO
#define __sultan__ISCENARIO
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <boost/cstdint.hpp>

#include <sultan/tracking_object.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/sequence.h>
#include <sultan/node.h>

namespace SULTAN {
  namespace topology{


    class scenario : public tracking_object{

      // a reconstruction scenario is composed of
      // a collection of tracks

    private:
      std::string appname_;

      // n of free families
      size_t n_free_families_;

      // n of overlapping cells
      size_t n_overlaps_;

    public:

      // tracks
      std::vector<topology::sequence> sequences_;

      //!Default constructor
      scenario();

      //!Default destructor
      virtual ~scenario();

      //! constructor
      scenario(const std::vector<sequence> & seqs, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false)const;


      //! set experimental_point, radius, error and id;
      void set(const std::vector<sequence> & seqs);

      //! set sequences
      void set_sequences(const std::vector<sequence> & seqs);

      //! get sequences
      const std::vector<sequence> & sequences()const;

    };
  }
}

#endif
