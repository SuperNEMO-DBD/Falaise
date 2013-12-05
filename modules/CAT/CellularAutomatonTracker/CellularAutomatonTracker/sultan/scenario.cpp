/* -*- mode: c++ -*- */
#include <sultan/scenario.h>

namespace SULTAN {
  namespace topology{

    //!Default constructor
    scenario::scenario()
    {
      appname_= "scenario: ";
      set_print_level(mybhep::NORMAL);
      set_probmin(10.);
    }

    //!Default destructor
    scenario::~scenario()
    {
    }

    //! constructor
    scenario::scenario(const std::vector<sequence> & seqs, 
                       mybhep::prlevel level, double probmin){
      appname_= "scenario: ";
      set_print_level(mybhep::NORMAL);
      set_probmin(probmin);
      sequences_ = seqs;
    }

    /*** dump ***/
    void scenario::dump (std::ostream & a_out      ,
                         const std::string & a_title ,
                         const std::string & a_indent,
                         bool a_inherit          )const{
      {
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " -------------- " << std::endl;
        for(std::vector<sequence>::const_iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq)
          iseq->dump();

        return;
      }
    }



    //! set experimental_point, radius, error and id;
    void scenario::set(const std::vector<sequence> & seqs){
      appname_= "scenario: ";
      sequences_ = seqs;
    }

    //! set sequences
    void scenario::set_sequences(const std::vector<sequence> & seqs){
      sequences_ = seqs;
    }

    //! get sequences
    const std::vector<sequence> & scenario::sequences()const
    {
      return sequences_;
    }

  }
}
