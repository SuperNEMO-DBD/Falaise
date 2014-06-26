/* -*- mode: c++ -*- */
// SULTAN_interface.cpp

#include <stdexcept>
#include <limits>
#include <map>
#include <vector>
#include <sstream>
#include <CATAlgorithm/SULTAN_interface.h>
#include <boost/algorithm/string.hpp>

namespace CAT {

  const std::string & setup_data_sultan::get_error_message() const
  {
    return _error_message;
  }

  void setup_data_sultan::_set_error_message(const std::string & message_)
  {
    std::ostringstream oss;
    oss << "SULTAN::setup_data_sultan: ";
    oss << message_;
    _error_message = oss.str();
    return;
  }

  setup_data_sultan::setup_data_sultan()
  {
    _set_defaults();
    return;
  }

  void setup_data_sultan::reset()
  {
    _set_defaults();
    return;
  }

  void setup_data_sultan::_set_defaults()
  {
    _error_message.clear();
    level                 = "normal";
    SuperNemo             = true;
    MaxTime               = 5000.0 * CLHEP::ms;
    probmin                = 0.;
    nofflayers            = 1;
    first_event            = -1;
    len                   = 2503. * CLHEP::mm;
    rad                   = 30.   * CLHEP::mm;
    vel                   = 0.06  * CLHEP::mm;
    CellDistance          = 30.   * CLHEP::mm;
    FoilRadius            = 0.;

    // SuperNEMO geometry default parameters :
    num_cells_per_plane    = 113;
    cell_size              = 44.0  * CLHEP::mm;
    bfield                 = 0.0025;
    xsize                  = 2500. * CLHEP::mm; // this is y in SnWare coordinates
    ysize                  = 1350. * CLHEP::mm; // this is z in SnWare coordinates
    zsize                  = 450. * CLHEP::mm; // this is x in SnWare coordinates

    return;
  }

  bool setup_data_sultan::check() const
  {
    setup_data_sultan * mutable_this = const_cast<setup_data_sultan *>(this);
    return mutable_this->_check_snemo();
  }

  bool setup_data_sultan::_check_snemo()
  {
    if (probmin < 0.0)
      {
        _set_error_message ("Invalid 'probmin'");
        return false;
      }
    if (num_cells_per_plane < 1)
      {
        _set_error_message ("Invalid 'num_cells_per_plane'");
        return false;
      }
    if (cell_size <= 0.0)
      {
        _set_error_message ("Invalid 'cell_size'");
        return false;
      }
    if (len < 0.0)
      {
        _set_error_message ("Invalid 'len'");
        return false;
      }
    if (rad < 0.0)
      {
        _set_error_message ("Invalid 'rad'");
        return false;
      }
    if (vel < 0.0)
      {
        _set_error_message ("Invalid 'vel'");
        return false;
      }
    if (CellDistance < 0.0)
      {
        _set_error_message ("Invalid 'CellDistance'");
        return false;
      }
    if (FoilRadius < 0.0)
      {
        _set_error_message ("Invalid 'FoilRadius'");
        return false;
      }

    return true;
  }

  void sultan_configure (Sultan & sultan_,
                              const setup_data_sultan & setup_)
  {
    if (! setup_.check ())
      {
        std::cerr << "ERROR: SULTAN::sultan_configure: "
                  << setup_.get_error_message () << std::endl;
        throw std::logic_error ("SULTAN::sultan_configure: Invalid setup data !");
      }

    // General parameters :
    sultan_.set_PrintMode (false);
    sultan_.set_MaxTime (setup_.MaxTime / CLHEP::ms);
    std::string leveltmp = setup_.level;
    boost::to_upper(leveltmp);

    sultan_.set_level (leveltmp); //mybhep::get_info_level (leveltmp));

    // Algorithm parameters :
    sultan_.set_probmin (setup_.probmin);
    sultan_.set_nofflayers (setup_.nofflayers);
    sultan_.set_first_event (setup_.first_event);
    sultan_.set_len (setup_.len);
    sultan_.set_rad (setup_.rad);
    sultan_.set_vel (setup_.vel);
    sultan_.set_CellDistance (setup_.CellDistance);
    sultan_.set_FoilRadius (setup_.FoilRadius);

    sultan_.set_bfield (setup_.bfield);
    sultan_.set_xsize (setup_.xsize);
    sultan_.set_ysize (setup_.ysize);
    sultan_.set_zsize (setup_.zsize);

    // Geometry description :
    if (setup_.SuperNemo)
      {
        /// Activate the special new mode :
        sultan_.set_SuperNemoChannel(true);

        sultan_.set_num_cells_per_plane (setup_.num_cells_per_plane);
        sultan_.set_GG_CELL_pitch (setup_.cell_size / CLHEP::mm);
      }
    else
      {
        throw std::logic_error ("SULTAN::sultan_configure: Only SuperNEMO setup is supported !");
      }

    return;
  }

  /***********************************************************/

  topology::Cell & input_data_sultan::add_cell ()
  {
    if (cells.size () == 0)
      {
        // memory preallocation at the first cell
        cells.reserve (50);
      }
    {
      topology::Cell tmp;
      cells.push_back (tmp);
    }
    return cells.back ();
  }

  bool input_data_sultan::check () const
  {
    std::vector<int> mids;
    for (int i = 0; i < (int) cells.size (); i++)
      {
        const topology::Cell & c = cells.at(i);
        int cell_id = c.id();
        if (cell_id < 0 || cell_id > 10000)
          {
            std::cerr << "ERROR: SULTAN::input_data_sultan::check: "
                      << "Out of range cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (std::find(mids.begin(), mids.end(), cell_id) != mids.end ())
          {
            std::cerr << "ERROR: SULTAN::input_data_sultan::check: "
                      << "Duplicate cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
      }

    return true;
  }

  input_data_sultan::input_data_sultan ()
  {
    return;
  }

  /***********************************************************/

  output_data_sultan::output_data_sultan ()
  {
    return;
  }

}

// end of SULTAN_interface.cpp
