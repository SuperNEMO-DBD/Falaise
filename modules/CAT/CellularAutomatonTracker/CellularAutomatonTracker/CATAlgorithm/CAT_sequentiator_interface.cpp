/* -*- mode: c++ -*- */
// CAT_sequentiator_interface.cpp

#include <CATAlgorithm/tracked_data.h>
#include <stdexcept>
#include <limits>
#include <map>
#include <vector>
#include <sstream>
#include <CATAlgorithm/CAT_sequentiator_interface.h>
#include <boost/algorithm/string.hpp>

namespace CAT {

  using namespace std;

  const std::string & sequentiator_setup_data::get_error_message () const
  {
    return _error_message;
  }

  void sequentiator_setup_data::_set_error_message (const std::string & message_)
  {
    std::ostringstream oss;
    oss << "CAT::sequentiator_setup_data: ";
    oss << message_;
    _error_message = oss.str ();
    return;
  }

  sequentiator_setup_data::sequentiator_setup_data ()
  {
    _set_defaults ();
    return;
  }

  void sequentiator_setup_data::reset ()
  {
    _set_defaults ();
    return;
  }

  void sequentiator_setup_data::_set_defaults ()
  {
    _error_message.clear ();
    level         = "normal";
    SuperNemo     = true;
    MaxTime       = 5000.  * CLHEP::ms;
    SmallRadius   =    0.1 * CLHEP::mm;
    TangentPhi    =   20.  *  CLHEP::degree;
    TangentTheta  =  160.  * CLHEP::degree;
    SmallNumber   =    0.1 * CLHEP::mm;
    QuadrantAngle =   90.  *  CLHEP::degree;
    Ratio         =   10.;
    CompatibilityDistance = 4.0 * CLHEP::mm;
    MaxChi2       = 3.;
    nsigma        = 4.;
    nofflayers    = 1;
    len           = 2503. * CLHEP::mm;
    rad           = 30. * CLHEP::mm;
    vel           = 0.06 * CLHEP::mm;
    CellDistance           = 30. * CLHEP::mm;
    FoilRadius           = 0.;

    // SuperNEMO geometry default parameters :
    num_blocks = 1;
    planes_per_block.clear ();
    planes_per_block.push_back (num_blocks);
    planes_per_block.at(0) = 9;
    num_cells_per_plane    = 113;
    cell_size              = 44.0  * CLHEP::mm;

    return;
  }

  bool sequentiator_setup_data::check () const
  {
    sequentiator_setup_data * mutable_this = const_cast<sequentiator_setup_data *> (this);
    return mutable_this->_check_snemo ();
  }

  bool sequentiator_setup_data::_check_snemo ()
  {
    if (SmallRadius <= 0.0)
      {
        _set_error_message ("Invalid 'SmallRadius'");
        return false;
      }
    if (TangentPhi <= 0.0)
      {
        _set_error_message ("Invalid 'TangentPhi'");
        return false;
      }
    if (TangentTheta <= 0.0)
      {
        _set_error_message ("Invalid 'TangentTheta'");
        return false;
      }
    if (SmallNumber <= 0.0)
      {
        _set_error_message ("Invalid 'SmallNumber'");
        return false;
      }
    if (QuadrantAngle <= 0.0)
      {
        _set_error_message ("Invalid 'QuadrantAngle'");
        return false;
      }
    if (Ratio <= 0.0)
      {
        _set_error_message ("Invalid 'Ratio'");
        return false;
      }
    if (CompatibilityDistance <= 0.0)
      {
        _set_error_message ("Invalid 'CompatibilityDistance'");
        return false;
      }
    if (MaxChi2 <= 0.0)
      {
        _set_error_message ("Invalid 'MaxChi2'");
        return false;
      }
    if (nsigma <= 0.0)
      {
        _set_error_message ("Invalid 'nsigma'");
        return false;
      }
    if (nofflayers < 0.0)
      {
        _set_error_message ("Invalid 'nofflayers'");
        return false;
      }
    if (num_blocks < 1)
      {
        _set_error_message ("Invalid 'num_blocks'");
        return false;
      }
    if (planes_per_block.size () != num_blocks)
      {
        _set_error_message ("Invalid size of 'planes_per_block'");
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

  void sequentiator_configure (sequentiator & stor_,
                              const sequentiator_setup_data & setup_)
  {
    if (! setup_.check ())
      {
        std::cerr << "ERROR: CAT::sequentiator_configure: "
                  << setup_.get_error_message () << std::endl;
        throw std::logic_error ("CAT::sequentiator_configure: Invalid setup data !");
      }

    // General parameters :
    stor_.set_PrintMode (false);
    stor_.set_MaxTime (setup_.MaxTime / CLHEP::ms);
    std::string leveltmp = setup_.level;
    boost::to_upper(leveltmp);

    stor_.set_level (leveltmp); //mybhep::get_info_level (leveltmp));

    // Algorithm parameters :
    stor_.set_SmallRadius (setup_.SmallRadius / CLHEP::mm);
    stor_.set_TangentPhi (setup_.TangentPhi / CLHEP::degree);
    stor_.set_TangentTheta (setup_.TangentTheta / CLHEP::degree);
    stor_.set_SmallNumber (setup_.SmallNumber / CLHEP::mm);
    stor_.set_QuadrantAngle (setup_.QuadrantAngle / CLHEP::degree);
    stor_.set_Ratio (setup_.Ratio);
    stor_.set_CompatibilityDistance (setup_.CompatibilityDistance);
    stor_.set_MaxChi2 (setup_.MaxChi2);
    stor_.set_nsigma (setup_.nsigma);
    stor_.set_nofflayers (setup_.nofflayers);
    stor_.set_len (setup_.len);
    stor_.set_rad (setup_.rad);
    stor_.set_vel (setup_.vel);
    stor_.set_CellDistance (setup_.CellDistance);
    stor_.set_FoilRadius (setup_.FoilRadius);

    // Geometry description :
    if (setup_.SuperNemo)
      {

        /// Activate the special new mode :
        stor_.set_SuperNemoChannel(true);

        // Layout of the tracking chamber :
        stor_.set_num_blocks (setup_.num_blocks);
        for (int i = 0; i < setup_.num_blocks; i++)
          {
            stor_.set_planes_per_block (i, (int)(setup_.planes_per_block.at (i)+0.5));
          }
        stor_.set_num_cells_per_plane (setup_.num_cells_per_plane);
        stor_.set_GG_CELL_pitch (setup_.cell_size / CLHEP::mm);
      }
    else
      {
        throw std::logic_error ("CAT::sequentiator_configure: Only SuperNEMO setup is supported !");
      }

    return;
  }


  /***********************************************************/

  topology::cell & sequentiator_input_data::add_cell ()
  {
    if (cells.size () == 0)
      {
        // memory preallocation at the first cell
        cells.reserve (50);
      }
    {
      topology::cell tmp;
      cells.push_back (tmp);
    }
    return cells.back ();
  }

  bool sequentiator_input_data::check () const
  {
    // A map would be better to check cell IDs :
    std::map<int,bool> mids;
    for (int i = 0; i < cells.size (); i++)
      {
        const topology::cell & c = cells.at(i);
        int cell_id = c.id();
        if (cell_id < 0 || cell_id > 10000)
          {
            std::cerr << "ERROR: CAT::sequentiator_input_data::check: "
                      << "Out of range cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (mids.find (cell_id) != mids.end ())
          {
            std::cerr << "ERROR: CAT::sequentiator_input_data::check: "
                      << "Duplicate cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        mids[cell_id] = true;
      }

    // Duplicate test for now :
    std::vector<bool> ids;
    ids.assign (cells.size (), false);
    for (int i = 0; i < cells.size (); i++)
      {
        const topology::cell & c = cells.at(i);
        int cell_id = c.id();
        if ((cell_id < 0) || (cell_id >= cells.size ()))
          {
            std::cerr << "ERROR: CAT::sequentiator_input_data::check: "
                      << "Invalid cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (ids[cell_id])
          {
            std::cerr << "ERROR: CAT::sequentiator_input_data::check: "
                      << "Duplicate cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        ids[cell_id] = true;
      }
    for (int i = 0; i < ids.size (); i++)
      {
        if (! ids[i])
          {
            std::cerr << "ERROR: CAT::sequentiator_input_data::check: "
                      << "Cell ID '" << i << "' is not used ! There are some missing cells !"
                      << std::endl;
            return false;
          }
      }
    return true;
  }

  sequentiator_input_data::sequentiator_input_data ()
  {
    return;
  }

  /***********************************************************/

  sequentiator_output_data::sequentiator_output_data ()
  {
    return;
  }

}

// end of CAT_sequentiator_interface.cpp
