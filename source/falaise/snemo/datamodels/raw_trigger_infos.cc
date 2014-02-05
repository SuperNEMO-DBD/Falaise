// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/raw_trigger_infos.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/raw_trigger_infos.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_trigger_infos, "snemo::datamodel::raw_trigger_infos")

    void raw_trigger_infos::set_trigger_number(uint32_t tn_)
    {
      _trigger_number_ = tn_;
      return;
    }

    uint32_t raw_trigger_infos::get_trigger_number() const
    {
      return _trigger_number_;
    }

    const std::bitset<3> & raw_trigger_infos::get_vtzw(int side_) const
    {
      return _VTZW_[side_];
    }

    std::bitset<3> & raw_trigger_infos::grab_vtzw(int side_)
    {
      return _VTZW_[side_];
    }

    const std::bitset<10> & raw_trigger_infos::get_ctzw(int side_) const
    {
      return _CTZW_[side_];
    }

    std::bitset<10> & raw_trigger_infos::grab_ctzw(int side_)
    {
      return _CTZW_[side_];
    }

    const std::bitset<9> & raw_trigger_infos::get_ttp(int side_, int row_) const
    {
      return _TTP_[side_][row_];
    }

    std::bitset<9> & raw_trigger_infos::grab_ttp(int side_, int row_)
    {
      return _TTP_[side_][row_];
    }

    raw_trigger_infos::raw_trigger_infos()
    {
      return;
    }

    raw_trigger_infos::~raw_trigger_infos()
    {
      return;
    }

    void raw_trigger_infos::reset()
    {
      for (unsigned int side = 0; side < NUMBER_OF_SIDES; side++) {
        _VTZW_[side] = 0;
        _CTZW_[side] = 0;
        for (unsigned int row = 0; row < NUMBER_OF_ROWS; row++) {
          _TTP_[side][row] = 0;
        }
      }
      return;
    }

    void raw_trigger_infos::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if(! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }

      // Trigger number:
      out_ << indent << datatools::i_tree_dumpable::tag;
      out_ << "Trigger number: " << _trigger_number_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Trigger primitives: " << std::endl;
      // Sided gamma-veto trigger zoning words:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::tag  << "VTZW[0] : " << _VTZW_[0] << std::endl;

      // Sided calorimeter trigger zoning words:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::tag  << "CTZW[0] : " << _CTZW_[0] << std::endl;

      /*
.................................................................................................................
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

      */

      // Sided tracker trigger primitives:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::tag << "TTP[0]  : " << std::endl;
      /*
      for (int layer = NUMBER_OF_LAYERS-1; layer >= 0; layer--) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::skip_tag;
        for (unsigned int row = 0; row < NUMBER_OF_ROWS; row++) {
          bool left  = _TTP_[0][row][layer];
          char sym = '.';
          if (left) {
            sym = 'o';
          }
          out_ << sym;
        }
        out_ << std::endl;
      }
      */
      // Sided tracker trigger primitives:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::tag << "TTP[1]  : " << std::endl;
      /*
      for (unsigned int layer = 0; layer < NUMBER_OF_LAYERS; layer++) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::skip_tag;
        for (unsigned int row = 0; row < NUMBER_OF_ROWS; row++) {
          bool left  = _TTP_[1][row][layer];
          char sym = '.';
          if (left) {
            sym = 'o';
          }
          out_ << sym;
        }
        out_ << std::endl;
      }
      */

      // Sided calorimeter trigger zoning words:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::tag << "CTZW[1] : " << _CTZW_[1] << std::endl;

      // Sided gamma-veto trigger zoning words:
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      out_ << datatools::i_tree_dumpable::last_tag << "VTZW[1] : " << _VTZW_[1] << std::endl;

      // Bar:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "More: " << "<not available yet>" << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/raw_trigger_infos.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
