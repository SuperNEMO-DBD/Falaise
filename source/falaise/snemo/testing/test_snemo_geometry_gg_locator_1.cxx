// -*- mode: c++ ; -*-
// test_gg_locator_1.cxx
/*
 * Copyright 2011-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <algorithm>
#include <list>

// Third party:
// - Boost:
#include <boost/foreach.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/time_tools.h>
#include <datatools/temporary_files.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools:
#include <geomtools/smart_id_locator.h>
#include <geomtools/manager.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/box.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/geometry/gg_locator.h>

using namespace std;

void test1 (geomtools::manager & mgr_)
{
  clog << "********** test1..." << endl;
  try
    {
      snemo::geometry::gg_locator badGGL(mgr_, 666);
    }
  catch (exception & x)
    {
      cerr << "ERROR: test1: As expected, there is no way to use a gg_locator for module #666 !"  << endl;
    }
  return;
}

void test2 (geomtools::manager & a_mgr, size_t a_nhits, bool a_file)
{
  clog << "********** test2..." << endl;
  int32_t my_module_number = 0;
  snemo::geometry::gg_locator GGL;
  GGL.set_geo_manager (a_mgr);
  GGL.set_module_number (my_module_number);
  GGL.initialize ();
  GGL.dump (clog);
  size_t counts = 0;
  ofstream f1, f2;
  if (a_file)
    {
      f1.open ("f1a.data");
      f2.open ("f2a.data");
    }
  datatools::computing_time CT;
  CT.start ();
  for (unsigned int i = 0; i < a_nhits; i++)
    {
      CT.pause ();
      double dim = 5 * CLHEP::m;
      double x = dim * (-1 + 2 * drand48 ());
      double y = dim * (-1 + 2 * drand48 ());
      double z = dim * (-1 + 2 * drand48 ());
      z = 0;
      geomtools::vector_3d hit_pos (x, y, z);
      if ((i % 10000) == 0) clog << "#i = " << i << endl;
      CT.resume ();
      geomtools::geom_id gid;
      GGL.find_geom_id (hit_pos, 1204, gid);
      CT.pause ();
      if (a_file)
        {
          if ((i % 100) == 0)
            {
              f1 << hit_pos.x () << ' '
                 << hit_pos.y () << ' '
                 << hit_pos.z () << endl;
            }
        }
      if (gid.is_valid ())
        {
          counts++;
          if (a_file)
            {
              f2 << hit_pos.x () << ' '
                 << hit_pos.y () << ' '
                 << hit_pos.z () << endl;
            }
        }
      CT.resume ();
    }
  CT.stop ();
  CT.tree_dump (clog);
  clog << "Counts (gg_locator) = " << counts << endl;
  return;
}

void test3 (geomtools::manager & a_mgr, size_t a_nhits, bool a_file)
{
  clog << "********** test3..." << endl;
  // int32_t my_module_number = 0;
  uint32_t geiger_cell_type
    = a_mgr.get_id_mgr ().categories_by_name ().find ("drift_cell_core")->second.get_type ();
  geomtools::smart_id_locator geiger_cell_locator (a_mgr.get_mapping (), geiger_cell_type);
  size_t counts = 0;
  ofstream f1, f2;
  if (a_file)
    {
      f1.open ("f1b.data");
      f2.open ("f2b.data");
    }
  datatools::computing_time CT;
  CT.start ();
  for (unsigned int i = 0; i < a_nhits; i++)
    {
      CT.pause ();
      double dim = 5 * CLHEP::m;
      double x = dim * (-1 + 2 * drand48 ());
      double y = dim * (-1 + 2 * drand48 ());
      double z = dim * (-1 + 2 * drand48 ());
      z = 0;
      geomtools::vector_3d hit_pos (x, y, z);
      if ((i % 10000) == 0) clog << "#i = " << i << endl;
      CT.resume ();
      geomtools::geom_id gid = geiger_cell_locator.get_geom_id (hit_pos, 1204);
      CT.pause ();
      if (a_file)
        {
          if ((i % 100) == 0)
            {
              f1 << hit_pos.x () << ' '
                 << hit_pos.y () << ' '
                 << hit_pos.z () << endl;
            }
        }
      if (gid.is_valid ())
        {
          counts++;
          if (a_file)
            {
              f2 << hit_pos.x () << ' '
                 << hit_pos.y () << ' '
                 << hit_pos.z () << endl;
            }
        }
      CT.resume ();
    }
  CT.stop ();
  CT.tree_dump (clog);
  clog << "Counts (smart_id_locator)  = " << counts << endl;
  return;
}

void test4 (geomtools::manager & a_mgr)
{
  clog << "********** test4..." << endl;
  int32_t my_module_number = 0;
  snemo::geometry::gg_locator GGL;
  GGL.set_geo_manager (a_mgr);
  GGL.set_module_number (my_module_number);
  GGL.initialize ();
  GGL.dump (clog);

  clog << "Number of neighbours [0,0,0] = "
       << GGL.get_number_of_neighbours (0, 0, 0) << endl;

  clog << "Number of neighbours [0,0,112] = "
       << GGL.get_number_of_neighbours (0, 0, 112) << endl;

  clog << "Number of neighbours [0,0,56] = "
       << GGL.get_number_of_neighbours (0, 0, 56) << endl;

  clog << "Number of neighbours [0,6,154] = "
       << GGL.get_number_of_neighbours (0,6,154) << endl;

  return;
}

void test5 (geomtools::manager & a_mgr)
{
  clog << "********** test5..." << endl;
  /**
   * Validated 2011-04-13 FM.
   *
   */
  int32_t my_module_number = 0;
  snemo::geometry::gg_locator GGL;
  GGL.set_geo_manager (a_mgr);
  GGL.set_module_number (my_module_number);
  GGL.initialize ();
  GGL.dump (clog);

  vector<geomtools::geom_id> ids;
  for (uint32_t side = 0;  side < 2; side++)
    {
      {
        GGL.get_neighbours_ids (side, 0, 0, ids, true);
        clog << "Neighbour cells for cell [" << side << ",0,0] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog <<  ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 0, 0, ids);
        clog << "Neighbour cells for cell [" << side << ",0,0] (w/o other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 0, 112, ids, true);
        clog << "Neighbour cells for cell [" << side << ",0,112] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 0, 56, ids, true);
        clog << "Neighbour cells for cell [" << side << ",0,56] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 6, 54, ids, true);
        clog << "Neighbour cells for cell [" << side << ",6,54] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 8, 0, ids, true);
        clog << "Neighbour cells for cell [" << side << ",8,0] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 8, 112, ids, true);
        clog << "Neighbour cells for cell [" << side << ",8,112] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        try
          {
            GGL.get_neighbours_ids (side, 123, 0, ids);
          }
        catch (exception & x)
          {
            cerr << "ERROR: test5: As expected, cell [" << side << ",123,0] is not valid !"  << endl << endl;
          }
      }

      {
        GGL.get_neighbours_ids (side, 8, 57, ids, true);
        clog << "Neighbour cells for cell [" << side << ",8,57] (with other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 4, 112, ids);
        clog << "Neighbour cells for cell [" << side << ",4,112] (w/o other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }

      {
        GGL.get_neighbours_ids (side, 4, 0, ids);
        clog << "Neighbour cells for cell [" << side << ",4,0] (w/o other side)=" << ids.size () << endl;
        for (unsigned int i = 0; i < ids.size (); i++)
          {
            clog << ids[i] << ' ';
          }
        clog << endl << endl;
      }
    }

  return;
}

void test6 (geomtools::manager & a_mgr, bool draw_)
{
  clog << "********** test6..." << endl;
  /**
   * Validated 2011-05-08 FM.
   *
   */
  int32_t my_module_number = 0;
  snemo::geometry::gg_locator GGL;
  GGL.set_geo_manager (a_mgr);
  GGL.set_module_number (my_module_number);
  GGL.initialize ();
  GGL.dump (clog);

  datatools::version_id geom_mgr_setup_vid;
  a_mgr.fetch_setup_version_id (geom_mgr_setup_vid);

  const geomtools::mapping & the_mapping = a_mgr.get_mapping ();


  const geomtools::id_mgr::categories_by_name_col_type & categories
    = a_mgr.get_id_mgr ().categories_by_name ();
  geomtools::geom_id world_id (0, 0);
  const geomtools::geom_info & world_geom_info = the_mapping.get_geom_info (world_id);
  const geomtools::placement & wp = world_geom_info.get_world_placement ();
  const geomtools::logical_volume & wlog = world_geom_info.get_logical ();
  const geomtools::i_shape_3d & wshape = wlog.get_shape ();

  uint32_t hall_type = categories.find ("hall")->second.get_type ();
  geomtools::geom_id hall_id (hall_type, 0);
  const geomtools::geom_info & hall_geom_info = the_mapping.get_geom_info (hall_id);
  const geomtools::placement & hp = hall_geom_info.get_world_placement ();
  const geomtools::logical_volume & hlog = hall_geom_info.get_logical ();
  const geomtools::i_shape_3d & hshape = hlog.get_shape ();

  uint32_t module_type = categories.find ("module")->second.get_type ();
  geomtools::geom_id module_id (module_type, my_module_number);
  const geomtools::geom_info & module_geom_info = the_mapping.get_geom_info (module_id);
  const geomtools::placement & mp = module_geom_info.get_world_placement ();
  const geomtools::logical_volume & mlog = module_geom_info.get_logical ();
  const geomtools::i_shape_3d & mshape = mlog.get_shape ();

  string source_cat = "source_strip";
  if (geom_mgr_setup_vid.has_major () && geom_mgr_setup_vid.get_major () == 1)
    {
      source_cat = "source_pad";
    }
  uint32_t source_volume_type
    = categories.find (source_cat)->second.get_type ();
  uint32_t scin_block_type
    = categories.find ("calorimeter_block")->second.get_type ();
  uint32_t geiger_cell_type
    = categories.find ("drift_cell_core")->second.get_type ();

  geomtools::smart_id_locator calo_block_locator (the_mapping, scin_block_type);

  // define the track as a list of 3D-vector:
  list<geomtools::vector_3d> track;

  // define a list of geometry IDs of the hits:
  list<geomtools::geom_id> hit_ids;

  // the vertex:
  geomtools::vector_3d vertex (-3.*CLHEP::m, 0.0, 0.0);

  // the first position of the track IS the vertex:
  geomtools::placement vertex_placement;
  vertex_placement.set_translation (vertex);
  track.push_back (vertex);

  // define an arbitrary propagation direction:
  geomtools::vector_3d propagation_dir (1.0, 0.2, 0.2);
  propagation_dir = propagation_dir.unit ();
  double step_length =  1. * CLHEP::cm;

  // define the propagation step:
  geomtools::vector_3d step = step_length * propagation_dir;
  double track_length = 0.0;
  double tolerance = 1.0 * CLHEP::um;

  // generate a track:
  for (geomtools::vector_3d track_pos = vertex;
       track_length < 5. * CLHEP::m;
       track_pos += step)
    {
      geomtools::geom_id hit_id;
      // First search for the ID correponding to this Geiger hit:
      GGL.find_geom_id (track_pos, geiger_cell_type, hit_id);
      // if this hit lies in some Geiger cell:
      if (hit_id.is_valid ())
        {
          // is the Geiger cell already hit ?
          if (find (hit_ids.begin (), hit_ids.end (), hit_id) == hit_ids.end ())
            {
              clog << "Adding a new 'geiger hit' : " << hit_id << endl;
              hit_ids.push_back (hit_id);

              // add spurious hit in one neighbour cell :
              vector<geomtools::geom_id> neighbour_ids;
              GGL.get_neighbours_ids (hit_id, neighbour_ids);
              size_t sz = neighbour_ids.size ();
              int noise_index = (int) (sz * drand48 ());
              hit_ids.push_back (neighbour_ids[noise_index]);
            }
        }
      else
        {
          // Search for the ID correponding to this Calo hit:
          hit_id = calo_block_locator.get_geom_id (track_pos,
                                                   scin_block_type,
                                                   tolerance);
          // if this hit lies in some scintillator block:
          if (hit_id.is_valid ())
            {
              // is the scintillator block already hit ?
              if (find (hit_ids.begin (), hit_ids.end (), hit_id) == hit_ids.end ())
                {
                  clog << "Adding a new 'calo hit' : " << hit_id << endl;
                  hit_ids.push_back (hit_id);
                }
            }
        }
      track_length += step.mag ();
      track.push_back (track_pos);
    }


  bool test_gid = true;
  if (test_gid)
    {
      uint32_t side = 0;
      {
        geomtools::geom_id calo_hit_id;
        calo_hit_id.set_type (scin_block_type);
        calo_hit_id.set (0, 0);
        calo_hit_id.set (1, side);
        calo_hit_id.set (2, 0);
        calo_hit_id.set (3, 0);
        if (geom_mgr_setup_vid.has_major () && geom_mgr_setup_vid.get_major () > 1)
          {
            calo_hit_id.set (4, 1);
          }
        hit_ids.push_back (calo_hit_id);
      }
      {
        geomtools::geom_id calo_hit_id;
        calo_hit_id.set_type (scin_block_type);
        calo_hit_id.set (0, 0);
        calo_hit_id.set (1, side);
        calo_hit_id.set (2, 1);
        calo_hit_id.set (3, 1);
        if (geom_mgr_setup_vid.has_major () && geom_mgr_setup_vid.get_major () > 1)
          {
            calo_hit_id.set (4, 1);
          }
        hit_ids.push_back (calo_hit_id);
      }
      {
        geomtools::geom_id calo_hit_id;
        calo_hit_id.set_type (scin_block_type);
        calo_hit_id.set (0, 0);
        calo_hit_id.set (1, side);
        calo_hit_id.set (2, 19);
        calo_hit_id.set (3, 12);
        if (geom_mgr_setup_vid.has_major () && geom_mgr_setup_vid.get_major () > 1)
          {
            calo_hit_id.set (4, 1);
          }
        hit_ids.push_back (calo_hit_id);
      }
    }

  // Draw:
  if (draw_) {
    Gnuplot g1 ("lines");
    g1.set_title ("sngeometry::test_manager_2\\nMapping test");
    g1.set_xlabel ("x");
    g1.set_ylabel ("y");
    g1.set_zlabel ("z");

    // Create 3 temp files:
    //datatools::temp_file::g_devel = true;
    datatools::temp_file fout1 (string ("test_manager_2."));
    datatools::temp_file fout2 (string ("test_manager_2."));
    datatools::temp_file fout3 (string ("test_manager_2."));

    // Draw world volume:
    geomtools::gnuplot_draw::draw (fout1.out (), wp, wshape);
    // Draw hall volume:
    geomtools::gnuplot_draw::draw (fout1.out (), hp, hshape);
    // Draw module #0 volume:
    geomtools::gnuplot_draw::draw (fout1.out (), mp, mshape);

    // Draw vertex:
    // define a box representing the position along the track for display:
    geomtools::box point_shape (.5 * CLHEP::cm, .5 * CLHEP::cm, .5 * CLHEP::cm);
    geomtools::gnuplot_draw::draw (fout1.out (), vertex_placement, point_shape);

    /** 2011-01-15, FM: The default  'resources/snemo/config_1.0/manager.conf' file
     * defines only one unique marge source pad as the source foil.
     * Draw it:
     */
    {
      const geomtools::geom_map::ginfo_ptr_collection_type & source_ginfo_array
        = the_mapping.get_ginfo_collection_with_type (source_volume_type);
      if (source_ginfo_array.size () == 0)
        {
          clog << "No source volumes are mapped with type '" << source_volume_type << "' !" << endl;
        }
      for (int i = 0; i < (int) source_ginfo_array.size (); i++)
        {
          const geomtools::geom_info & a_geom_info = *source_ginfo_array[i];
          //    const geomtools::geom_info & a_geom_info = the_mapping.get_geom_info (a_id);
          const geomtools::placement & a_placement = a_geom_info.get_world_placement ();
          const geomtools::logical_volume & plog = a_geom_info.get_logical ();
          const geomtools::i_shape_3d & a_shape = plog.get_shape ();
          clog << "Source volume geometry info: " << a_geom_info << endl;
          geomtools::gnuplot_draw::draw (fout1.out (), a_placement, a_shape);
        }
    }
    // geomtools::geom_id a_id (pad_type, my_module_number, 0, 0);
    // const geomtools::geom_info & a_geom_info = the_mapping.get_geom_info (a_id);
    // const geomtools::placement & a_placement = a_geom_info.get_world_placement ();
    // const geomtools::logical_volume & plog = a_geom_info.get_logical ();
    // const geomtools::i_shape_3d & a_shape = plog.get_shape ();
    // clog << "Source pad geometry info: " << a_geom_info << endl;
    // geomtools::gnuplot_draw::draw (fout1.out (), a_placement, a_shape);

    // Draw the steps along the track:
    for (list<geomtools::vector_3d>::const_iterator i = track.begin ();
         i != track.end ();
         i++)
      {
        geomtools::placement a_placement;
        a_placement.set_translation (*i);
        geomtools::gnuplot_draw::draw (fout2.out (), a_placement, point_shape);
      }

    // Draw the shapes of the volumes associated to geom ID of the hits
    // along the track:
    for (list<geomtools::geom_id>::const_iterator i = hit_ids.begin ();
         i != hit_ids.end ();
         i++)
      {
        const geomtools::geom_id & a_id = *i;
        const geomtools::geom_info & a_geom_info = the_mapping.get_geom_info (a_id);
        const geomtools::placement & a_placement = a_geom_info.get_world_placement ();
        const geomtools::logical_volume & a_log = a_geom_info.get_logical ();
        const geomtools::i_shape_3d & a_shape = a_log.get_shape ();
        clog << "Hit geometry info: " << a_geom_info << endl;
        geomtools::gnuplot_draw::draw (fout3.out (), a_placement, a_shape);
      }
    cerr << "**************** wait_for_key ********************" << endl;
    {
      ostringstream cmdstr;
      cmdstr << "splot ";
      cmdstr << "'"   << fout1.get_filename () << "' title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename () << "' title 'track'  with lines";
      cmdstr << ", '" << fout3.get_filename () << "' title 'hits' with lines";
      g1.cmd (cmdstr.str ());
      g1.showonscreen (); // window output
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }
    {
      ostringstream cmdstr;
      cmdstr << "set size ratio -1; set grid; plot ";
      cmdstr << "'"   << fout1.get_filename () << "' using 2:3 title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename () << "' using 2:3 title 'track' with lines";
      cmdstr << ", '" << fout3.get_filename () << "' using 2:3 title 'hits' with lines";
      g1.cmd (cmdstr.str ());
      g1.showonscreen (); // window output
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }

  } // Draw

  return;
}

int main (int argc_, char ** argv_)
{
  FALAISE_INIT_MAIN(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'snemo::geometry::gg_locator' !" << endl;

      bool   debug = false;
      bool   devel = false;
      bool   verbose = false;
      bool   file = false;
      string manager_config_file;
      bool   dump = true;
      bool   draw = false;
      bool   with_mapping = true;
      bool   do_test1 = true;
      bool   do_test2 = true;
      bool   do_test3 = false;
      bool   do_test4 = true;
      bool   do_test5 = true;
      bool   do_test6 = true;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if ((option == "-D") || (option == "--devel"))
                {
                  devel = true;
                }
             else if ((option == "-t1") || (option == "--test1"))
                {
                  do_test1 = true;
                }
              else if ((option == "-t2") || (option == "--test2"))
                {
                  do_test2 = true;
                }
              else if ((option == "-t3") || (option == "--test3"))
                {
                  do_test3 = true;
                }
              else if ((option == "-t4") || (option == "--test4"))
                {
                  do_test4 = true;
                }
              else if ((option == "-t5") || (option == "--test5"))
                {
                  do_test5 = true;
                }
              else if ((option == "-t6") || (option == "--test6"))
                {
                  do_test6 = true;
                }
              else if ((option == "-T1") || (option == "--no-test1"))
                {
                  do_test1 = false;
                }
              else if ((option == "-T2") || (option == "--no-test2"))
                {
                  do_test2 = false;
                }
              else if ((option == "-T3") || (option == "--no-test3"))
                {
                  do_test3 = false;
                }
              else if ((option == "-T4") || (option == "--no-test4"))
                {
                  do_test4 = false;
                }
              else if ((option == "-T5") || (option == "--no-test5"))
                {
                  do_test5 = false;
                }
              else if ((option == "-T6") || (option == "--no-test6"))
                {
                  do_test6 = false;
                }
              else if ((option == "-V") || (option == "--verbose"))
                {
                  verbose = true;
                }
              else if ((option == "-F") || (option == "--file"))
                {
                  file = true;
                }
              else if ((option == "-p") || (option == "--dump"))
                {
                  dump = true;
                }
              else if ((option == "-m") || (option == "--with-mapping"))
                {
                  with_mapping = true;
                }
              else if ((option == "-M") || (option == "--without-mapping"))
                {
                  with_mapping = false;
                }
             else if (option == "--draw")
                {
                  draw = true;
                }
             else if (option == "--no-draw")
               {
                 draw = false;
               }
             else
               {
                 clog << datatools::io::warning << "ignoring option '"
                      << option << "'!" << endl;
               }
            }
          else
            {
              string argument = token;
              {
               if (manager_config_file.empty ())
                  {
                    manager_config_file = argument;
                  }
                else
                  {
                    clog << datatools::io::warning << "ignoring argument '"
                         << argument << "'!" << endl;
                  }
              }
            }
          iarg++;
        }

      if (manager_config_file.empty ())
        {
          manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
        }
      datatools::fetch_path_with_env (manager_config_file);
      clog << datatools::io::notice << "Manager config. file : '"
           << manager_config_file << "'" << endl;
      // load properties from the configuration file:
      datatools::properties manager_config;
      datatools::properties::read_config (manager_config_file,
                                          manager_config);
      geomtools::manager my_manager;
      if (verbose) my_manager.set_logging_priority (datatools::logger::PRIO_NOTICE);
      if (debug)   my_manager.set_logging_priority (datatools::logger::PRIO_DEBUG);
      if (devel)   my_manager.set_logging_priority (datatools::logger::PRIO_TRACE);
      if (with_mapping)
        {
          // prepare mapping configuration with a limited set
          // of geometry categories:

          // ensure the mapping features will be built:
          manager_config.update ("build_mapping", true);

          // remove the 'exclusion' property if any:
          if (manager_config.has_key ("mapping.excluded_categories"))
            {
              manager_config.erase ("mapping.excluded_categories");
            }

          // define a set of geometry categories to be mapped
          // through the embedded mapping system:
          vector<string> only_categories;
          only_categories.push_back ("hall");
          only_categories.push_back ("module");
          only_categories.push_back ("source_pad");
          only_categories.push_back ("source_strip");
          only_categories.push_back ("tracker_submodule");
          only_categories.push_back ("drift_cell_core");
          only_categories.push_back ("xcalo_block");
          only_categories.push_back ("gveto_block");
          only_categories.push_back ("calorimeter_block");

          // set the 'only' property:
          manager_config.update ("mapping.only_categories", only_categories);
        }
      my_manager.initialize (manager_config);
      if (dump)
        {
          my_manager.get_factory ().tree_dump (clog, "The SuperNEMO geometry model factory");
          my_manager.get_id_mgr ().tree_dump (clog, "The SuperNEMO geometry ID manager");
        }


      long seed = 12345;
      srand48 (seed);

      if (do_test1)
        {
          test1 (my_manager);
        }

      size_t nhits = 100000;
      if (do_test2)
        {
          test2 (my_manager, nhits, file);
        }

      if (do_test3)
        {
          test3 (my_manager, nhits, file);
        }

     if (do_test4)
       {
         test4 (my_manager);
       }

     if (do_test5)
       {
         test5 (my_manager);
       }

     if (do_test6)
       {
         test6 (my_manager, draw);
       }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  FALAISE_FINI();
  return (error_code);
}

// end of test_gg_locator_1.cxx
