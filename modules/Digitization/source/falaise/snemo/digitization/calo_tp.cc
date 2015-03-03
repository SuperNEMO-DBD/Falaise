// calo_tp.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

// Ourselves:
#include <snemo/digitization/calo_tp.h>

// Third party:
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_tp, "snemo::digitalization::calo_tp")

    calo_tp::calo_tp()
    {
      _clocktick_25ns_ = -1;
      _tp_ = 0x0;
      return;
    }

    calo_tp::~calo_tp()
    {
      return;
    }

    int32_t calo_tp::get_clocktick_25ns() const
    {
      return _clocktick_25ns_;
    }

    void calo_tp::set_clocktick_25ns( int32_t value_ )
    {
      _clocktick_25ns_ = value_;
      _store |= STORE_CLOCKTICK_25NS;
      return;
    }

    void calo_tp::reset_clocktick_25ns()
    {
      _clocktick_25ns_ = -1;
      _store &= ~STORE_CLOCKTICK_25NS;
      return;
    }

    const std::bitset<5> & calo_tp::get_tp() const
    {
      return _tp_;
    }
    
    std::bitset<5> & calo_tp::grab_tp()
    {
      _store |= STORE_TP;
       return _tp_;
    }

    void calo_tp::set_tp(const std::bitset<5> & tp_)
    {
      _tp_ = tp_;
      _store |= STORE_TP;
      return;
    }
    
    void calo_tp::reset_tp()
    {
      _tp_ = 0x0;
      _store &= STORE_TP;
      return;
    }

    bool calo_tp::is_valid() const
    {
      return _clocktick_25ns_ >= 0;
    }

    void  calo_tp::reset()
    {
      reset_tp();
      reset_clocktick_25ns();
      geomtools::base_hit::reset();
      return;
    }

    void calo_tp::tree_dump (std::ostream & out_,
			     const std::string & title_,
			     const std::string & indent_,
			     bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Clock tick (25 ns)  : " << _clocktick_25ns_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "TP (5 bits) : " << _tp_  << std::endl;

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
