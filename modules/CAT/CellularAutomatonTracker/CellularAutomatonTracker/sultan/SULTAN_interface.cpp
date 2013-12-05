/* -*- mode: c++ -*- */
// SULTAN_interface.cpp

#include <stdexcept>
#include <limits>
#include <map>
#include <vector>
#include <sstream>
#include <sultan/SULTAN_interface.h>
#include <sultan/tracked_data.h>
#include <boost/algorithm/string.hpp>

namespace SULTAN {

  const std::string & setup_data::get_error_message () const
  {
    return _error_message;
  }

  void setup_data::_set_error_message (const std::string & message_)
  {
    std::ostringstream oss;
    oss << "SULTAN::setup_data: ";
    oss << message_;
    _error_message = oss.str ();
    return;
  }

  setup_data::setup_data ()
  {
    _set_defaults ();
    return;
  }

  void setup_data::reset ()
  {
    _set_defaults ();
    return;
  }

  void setup_data::_set_defaults ()
  {
    _error_message.clear ();
    clusterizer_level                         = "normal";
    sequentiator_level                         = "normal";
    SuperNemo                     = true;
    max_time                      = 5000.0; // ms
    probmin                       = 0.;
    nsigma                        = 3.;
    nofflayers                    = 1;
    first_event                   = -1;
    min_ncells_in_cluster         = 0;
    ncells_between_triplet_min    = 0;
    ncells_between_triplet_range  = 0;
    cell_distance                 = 30.; // mm
    foil_radius                   = 0.;

    // SuperNEMO geometry default parameters :
    num_blocks             = 1;
    planes_per_block.clear ();
    planes_per_block.push_back (num_blocks);
    planes_per_block.at(0) = 9;
    num_cells_per_plane    = 113;
    bfield                 = 0.0025;
    xsize                  = 450.; // mm
    ysize                  = 2500.; // mm
    zsize                  = 1350.; // mm
    Emin                   = 0.2; // MeV
    Emax                   = 7.; // MeV

    return;
  }

  bool setup_data::check () const
  {
    setup_data * mutable_this = const_cast<setup_data *> (this);
    return mutable_this->_check_snemo ();
  }

  bool setup_data::_check_snemo ()
  {
    if (max_time < 0.0)
      {
        _set_error_message ("Invalid 'max_time'");
        return false;
      }
    if (probmin < 0.0)
      {
        _set_error_message ("Invalid 'probmin'");
        return false;
      }
    if (nsigma < 0.0)
      {
        _set_error_message ("Invalid 'nsigma'");
        return false;
      }
    if (nofflayers < 0.0)
      {
        _set_error_message ("Invalid 'nofflayers'");
        return false;
      }
    if (min_ncells_in_cluster < 0.0)
      {
        _set_error_message ("Invalid 'min_ncells_in_cluster'");
        return false;
      }
    if (ncells_between_triplet_min < 0.0)
      {
        _set_error_message ("Invalid 'ncells_between_triplet_min'");
        return false;
      }
    if (ncells_between_triplet_range < 0.0)
      {
        _set_error_message ("Invalid 'ncells_between_triplet_range'");
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
    if (cell_distance <= 0.0)
      {
        _set_error_message ("Invalid 'cell_distance'");
        return false;
      }
    if (foil_radius < 0.0)
      {
        _set_error_message ("Invalid 'foil_radius'");
        return false;
      }
    if (bfield < 0.0)
      {
        _set_error_message ("Invalid 'bfield'");
        return false;
      }
    if (xsize < 0.0)
      {
        _set_error_message ("Invalid 'xsize'");
        return false;
      }
    if (ysize < 0.0)
      {
        _set_error_message ("Invalid 'ysize'");
        return false;
      }
    if (zsize < 0.0)
      {
        _set_error_message ("Invalid 'zsize'");
        return false;
      }
    if (Emin < 0.0)
      {
        _set_error_message ("Invalid 'Emin'");
        return false;
      }

    if (Emax < 0.0)
      {
        _set_error_message ("Invalid 'Emax'");
        return false;
      }

    return true;
  }

  void clusterizer_configure (clusterizer & czer_,
                              const setup_data & setup_)
  {
    if (! setup_.check ())
      {
        std::cerr << "ERROR: SULTAN::clusterizer_configure: "
                  << setup_.get_error_message () << std::endl;
        throw std::logic_error ("SULTAN::clusterizer_configure: Invalid setup data !");
      }

    std::string leveltmp = setup_.clusterizer_level;
    boost::to_upper(leveltmp);
    czer_.set_level (leveltmp);

    // Algorithm parameters :
    czer_.set_max_time (setup_.max_time);
    czer_.set_probmin (setup_.probmin);
    czer_.set_nofflayers (setup_.nofflayers);
    czer_.set_first_event (setup_.first_event);
    czer_.set_cell_distance (setup_.cell_distance);
    czer_.set_foil_radius (setup_.foil_radius);

    czer_.set_bfield (setup_.bfield);
    czer_.set_xsize (setup_.xsize);
    czer_.set_ysize (setup_.ysize);
    czer_.set_zsize (setup_.zsize);
    czer_.set_Emin (setup_.Emin);

    // Geometry description :
    if (setup_.SuperNemo)
      {
        /// Activate the special new mode :
        czer_.set_SuperNemoChannel(true);

        // Layout of the tracking chamber :
        czer_.set_num_blocks (setup_.num_blocks);
        for (int i = 0; i < setup_.num_blocks; i++)
          {
            czer_.set_planes_per_block (i, (int)(setup_.planes_per_block.at (i)+0.5));
          }
        czer_.set_num_cells_per_plane (setup_.num_cells_per_plane);
      }
    else
      {
        throw std::logic_error ("SULTAN::clusterizer_configure: Only SuperNEMO setup is supported !");
      }

    return;
  }

  void sultan_configure (sultan & stor_,
                               const setup_data & setup_)
  {
    if (! setup_.check ())
      {
        std::cerr << "ERROR: SULTAN::sultan_configure: "
                  << setup_.get_error_message () << std::endl;
        throw std::logic_error ("SULTAN::sultan_configure: Invalid setup data !");
      }

    // General parameters :
    stor_.set_max_time (setup_.max_time );
    std::string leveltmp = setup_.sequentiator_level;
    boost::to_upper(leveltmp);

    stor_.set_level (leveltmp); //mybhep::get_info_level (leveltmp));

    // Algorithm parameters :
    stor_.set_probmin (setup_.probmin);
    stor_.set_nsigma (setup_.nsigma);
    stor_.set_nofflayers (setup_.nofflayers);
    stor_.set_first_event (setup_.first_event);
    stor_.set_min_ncells_in_cluster (setup_.min_ncells_in_cluster);
    stor_.set_ncells_between_triplet_min (setup_.ncells_between_triplet_min);
    stor_.set_ncells_between_triplet_range (setup_.ncells_between_triplet_range);
    stor_.set_cell_distance (setup_.cell_distance);
    stor_.set_foil_radius (setup_.foil_radius);

    stor_.set_bfield (setup_.bfield);
    stor_.set_xsize (setup_.xsize);
    stor_.set_ysize (setup_.ysize);
    stor_.set_zsize (setup_.zsize);
    stor_.set_Emin (setup_.Emin);
    stor_.set_Emax (setup_.Emax);

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
      }
    else
      {
        throw std::logic_error ("SULTAN::sultan_configure: Only SuperNEMO setup is supported !");
      }

    return;
  }


  /***********************************************************/

  topology::cell & input_data::add_cell ()
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

  bool input_data::check () const
  {
    // A map would be better to check cell IDs :
    std::map<int,bool> mids;
    for (int i = 0; i < cells.size (); i++)
      {
        const topology::cell & c = cells.at(i);
        int cell_id = c.id();
        if (cell_id < 0 || cell_id > 10000)
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Out of range cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (mids.find (cell_id) != mids.end ())
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
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
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Invalid cell ID '" <<  cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (ids[cell_id])
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
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
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Cell ID '" << i << "' is not used ! There are some missing cells !"
                      << std::endl;
            return false;
          }
      }
    return true;
  }

  input_data::input_data ()
  {
    return;
  }

  /***********************************************************/

  output_data::output_data ()
  {
    return;
  }

}

// end of SULTAN_interface.cpp
