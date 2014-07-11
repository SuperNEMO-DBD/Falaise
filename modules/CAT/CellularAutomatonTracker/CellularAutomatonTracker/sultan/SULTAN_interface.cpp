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
    clusterizer_level             = "normal";
    sequentiator_level            = "normal";
    SuperNemo                     = true;
    max_time                      = 5000.0; // ms
    print_event_display           = false;
    use_clocks                    = false;
    use_endpoints                 = true;
    use_legendre                  = false;
    clusterize_with_helix_model   = false;
    assign_helices_to_clusters    = true;
    probmin                       = 0.;
    nsigma_r                      = 5.;
    nsigma_z                      = 3.;
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
    nsigmas                 = 1.;
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
    if (nsigma_r < 0.0)
      {
        _set_error_message ("Invalid 'nsigma_r'");
        return false;
      }
    if (nsigma_z < 0.0)
      {
        _set_error_message ("Invalid 'nsigma_z'");
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
    if ((int) planes_per_block.size () != num_blocks)
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
    if (nsigmas < 0.0)
      {
        _set_error_message ("Invalid 'nsigmas'");
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

    czer_.set_xsize (setup_.xsize);
    czer_.set_ysize (setup_.ysize);
    czer_.set_zsize (setup_.zsize);

    // Geometry description :
    if (setup_.SuperNemo)
      {
        /// Activate the special new mode :
        czer_.set_SuperNemoChannel(true);

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
    stor_.set_print_event_display (setup_.print_event_display );
    stor_.set_use_clocks (setup_.use_clocks );
    stor_.set_use_endpoints (setup_.use_endpoints );
    stor_.set_assign_helices_to_clusters (setup_.assign_helices_to_clusters );
    stor_.set_use_legendre (setup_.use_legendre );
    stor_.set_clusterize_with_helix_model (setup_.clusterize_with_helix_model );
    std::string leveltmp = setup_.sequentiator_level;
    boost::to_upper(leveltmp);

    stor_.set_level (leveltmp); //mybhep::get_info_level (leveltmp));

    // Algorithm parameters :
    stor_.set_probmin (setup_.probmin);
    stor_.set_nsigma_r (setup_.nsigma_r);
    stor_.set_nsigma_z (setup_.nsigma_z);
    stor_.set_nofflayers (setup_.nofflayers);
    stor_.set_first_event (setup_.first_event);
    stor_.set_min_ncells_in_cluster (setup_.min_ncells_in_cluster);
    stor_.set_ncells_between_triplet_min (setup_.ncells_between_triplet_min);
    stor_.set_ncells_between_triplet_range (setup_.ncells_between_triplet_range);
    stor_.set_cell_distance (setup_.cell_distance);
    stor_.set_foil_radius (setup_.foil_radius);

    stor_.set_bfield (setup_.bfield);
    stor_.set_nsigmas (setup_.nsigmas);
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

  topology::calorimeter_hit & input_data::add_calo_cell ()
  {

    if (calo_cells.size () == 0)
      {
        // memory preallocation at the first calo_cell
        calo_cells.reserve (50);
      }
    {
      topology::calorimeter_hit tmp;
      calo_cells.push_back (tmp);
    }
    return calo_cells.back ();
  }

  bool input_data::check () const{
    return gg_check() && calo_check();
  }


  bool input_data::gg_check () const
  {
    // A map would be better to check cell IDs :
    std::map<int,bool> mids;
    for (int i = 0; i < (int) cells.size (); i++)
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
    for (int i = 0; i < (int) cells.size (); i++)
      {
        const topology::cell & c = cells.at(i);
        int cell_id = c.id();
        if ((cell_id < 0) || (cell_id >= (int) cells.size ()))
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
    for (int i = 0; i < (int) ids.size (); i++)
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

  bool input_data::calo_check () const
  {
    // A map would be better to check cell IDs :
    std::map<int,bool> mids;
    for (int i = 0; i < (int) calo_cells.size (); i++)
      {
        const topology::calorimeter_hit & c = calo_cells.at(i);
        int calo_cell_id = c.id();
        if (calo_cell_id < 0 || calo_cell_id > 10000)
          {
            std::cerr << "ERROR: SULTAN::input_data::calo_check: "
                      << "Out of range calo_cell ID '" <<  calo_cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (mids.find (calo_cell_id) != mids.end ())
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Duplicate calo_cell ID '" <<  calo_cell_id << "' !"
                      << std::endl;
            return false;
          }
        mids[calo_cell_id] = true;
      }

    // Duplicate test for now :
    std::vector<bool> ids;
    ids.assign (calo_cells.size (), false);
    for (int i = 0; i < (int) calo_cells.size (); i++)
      {
        const topology::calorimeter_hit & c = calo_cells.at(i);
        int calo_cell_id = c.id();
        if ((calo_cell_id < 0) || (calo_cell_id >= (int) calo_cells.size ()))
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Invalid calo_cell ID '" <<  calo_cell_id << "' !"
                      << std::endl;
            return false;
          }
        if (ids[calo_cell_id])
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Duplicate calo_cell ID '" <<  calo_cell_id << "' !"
                      << std::endl;
            return false;
          }
        ids[calo_cell_id] = true;
      }
    for (int i = 0; i < (int) ids.size (); i++)
      {
        if (! ids[i])
          {
            std::cerr << "ERROR: SULTAN::input_data::check: "
                      << "Calo_Cell ID '" << i << "' is not used ! There are some missing calo_cells !"
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
