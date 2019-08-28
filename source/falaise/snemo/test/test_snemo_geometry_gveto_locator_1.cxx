// test_gveto_locator_1.cxx
/*
 * Copyright 2011-2014 F. Mauger
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
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

// Third party:
// - Boost:
#include <boost/foreach.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/temporary_files.h>
#include <datatools/time_tools.h>
#include <datatools/utils.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools:
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/manager.h>
#include <geomtools/smart_id_locator.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

using namespace std;

void test1(geomtools::manager& mgr_) {
  clog << "********** test1..." << endl;
  try {
    snemo::geometry::gveto_locator badCL(666, mgr_, {});
  } catch (exception& x) {
    cerr << "ERROR: test1: As expected, there is no way to use a gveto_locator for module #666 !"
         << endl;
  }
}

void test2(geomtools::manager& a_mgr, size_t a_nhits, bool a_file) {
  clog << "********** test2..." << endl;
  uint32_t my_module_number = 0;
  snemo::geometry::gveto_locator GL{my_module_number, a_mgr, {}};
  // GL.set_geo_manager(a_mgr);
  // GL.set_module_number(my_module_number);
  // GL.initialize(datatools::properties{});
  size_t counts = 0;
  ofstream f1, f2;
  if (a_file) {
    f1.open("f1a.data");
    f2.open("f2a.data");
  }
  datatools::computing_time CT;
  CT.start();
  for (unsigned int i = 0; i < a_nhits; i++) {
    CT.pause();
    double dim = 5 * CLHEP::m;
    double x = dim * (-1 + 2 * drand48());
    double y = dim * (-1 + 2 * drand48());
    double z = dim * (-1 + 2 * drand48());
    z = 0;
    geomtools::vector_3d hit_pos(x, y, z);
    if ((i % 10000) == 0) {
      clog << "#i = " << i << endl;
    }
    CT.resume();
    geomtools::geom_id gid;
    GL.find_geom_id(hit_pos, 1252, gid);
    CT.pause();
    if (a_file) {
      if ((i % 100) == 0) {
        f1 << hit_pos.x() << ' ' << hit_pos.y() << ' ' << hit_pos.z() << endl;
      }
    }
    if (gid.is_valid()) {
      counts++;
      if (a_file) {
        f2 << hit_pos.x() << ' ' << hit_pos.y() << ' ' << hit_pos.z() << endl;
      }
    }
    CT.resume();
  }
  CT.stop();
  CT.tree_dump(clog);
  clog << "Counts(gveto_locator) = " << counts << endl;
}

void test3(geomtools::manager& a_mgr, size_t a_nhits, bool a_file) {
  clog << "********** test3..." << endl;
  // int32_t my_module_number = 0;
  uint32_t calo_block_type =
      a_mgr.get_id_mgr().categories_by_name().find("gveto_block")->second.get_type();
  geomtools::smart_id_locator gveto_locator(a_mgr.get_mapping(), calo_block_type);
  size_t counts = 0;
  ofstream f1, f2;
  if (a_file) {
    f1.open("f1b.data");
    f2.open("f2b.data");
  }
  datatools::computing_time CT;
  CT.start();
  for (unsigned int i = 0; i < a_nhits; i++) {
    CT.pause();
    double dim = 5 * CLHEP::m;
    double x = dim * (-1 + 2 * drand48());
    double y = dim * (-1 + 2 * drand48());
    double z = dim * (-1 + 2 * drand48());
    z = 0;
    geomtools::vector_3d hit_pos(x, y, z);
    if ((i % 10000) == 0) {
      clog << "#i = " << i << endl;
    }
    CT.resume();
    const geomtools::geom_id& gid = gveto_locator.get_geom_id(hit_pos, calo_block_type);
    CT.pause();
    if (a_file) {
      if ((i % 100) == 0) {
        f1 << hit_pos.x() << ' ' << hit_pos.y() << ' ' << hit_pos.z() << endl;
      }
    }
    if (gid.is_valid()) {
      counts++;
      if (a_file) {
        f2 << hit_pos.x() << ' ' << hit_pos.y() << ' ' << hit_pos.z() << endl;
      }
    }
    CT.resume();
  }
  CT.stop();
  CT.tree_dump(clog);
  clog << "Counts(smart_id_locator)  = " << counts << endl;
}

void test4(geomtools::manager& a_mgr) {
  clog << "********** test4..." << endl;
  int32_t my_module_number = 0;
  snemo::geometry::gveto_locator GL;
  GL.set_geo_manager(a_mgr);
  GL.setModuleNumber(my_module_number);
  GL.initialize(datatools::properties{});

  clog << "Number of sides = " << GL.numberOfSides() << endl;

  clog << "Number of walls = " << GL.numberOfWalls() << endl;

  clog << "Number of columns = " << GL.numberOfColumns(0, 0) << endl;

  clog << "Number of neighbours [0,0,0,0] = " << GL.countNeighbours(0, 0, 0, 0) << endl;

  clog << "Number of neighbours [0,0,0,12] = " << GL.countNeighbours(0, 0, 0, 12) << endl;

  clog << "Number of neighbours [0,0,0,5] = " << GL.countNeighbours(0, 0, 0, 5) << endl;

  clog << "Number of neighbours [0,0,1,15] = " << GL.countNeighbours(0, 0, 1, 15) << endl;
}

void test5(geomtools::manager& a_mgr) {
  clog << "********** test5..." << endl;
  /**
   * Validated 2011-04-13 FM.
   *
   */
  int32_t my_module_number = 0;
  snemo::geometry::gveto_locator GL;
  GL.set_geo_manager(a_mgr);
  GL.setModuleNumber(my_module_number);
  GL.initialize(datatools::properties{});

  vector<geomtools::geom_id> ids;
  for (uint32_t side = 0; side < 2; side++) {
    {
      ids = GL.getNeighbourGIDs(side, 0, 0, snemo::geometry::grid_mask_t::FIRST);
      clog << "Neighbour blocks for block [" << side << ",0,0](with first)=" << ids.size() << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      ids = GL.getNeighbourGIDs(side, 0, 0, snemo::geometry::grid_mask_t::DIAG);
      clog << "Neighbour blocks for block [" << side << ",0,0](with diagonal)=" << ids.size()
           << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      ids = GL.getNeighbourGIDs(side, 0, 1, snemo::geometry::grid_mask_t::FIRST);
      clog << "Neighbour blocks for block [" << side << ",0,1](with first)=" << ids.size() << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      ids = GL.getNeighbourGIDs(side, 1, 4, snemo::geometry::grid_mask_t::FIRST);
      clog << "Neighbour blocks for block [" << side << ",1,4](with first)=" << ids.size() << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      ids = GL.getNeighbourGIDs(side, 0, 12, snemo::geometry::grid_mask_t::FIRST);
      clog << "Neighbour blocks for block [" << side << ",0,12](with first)=" << ids.size() << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      ids = GL.getNeighbourGIDs(side, 1, 12, snemo::geometry::grid_mask_t::FIRST);
      clog << "Neighbour blocks for block [" << side << "1,12](with first)=" << ids.size() << endl;
      for (const auto& id : ids) {
        clog << id << ' ';
      }
      clog << endl << endl;
    }

    {
      try {
        ids = GL.getNeighbourGIDs(side, 3, 14, snemo::geometry::grid_mask_t::FIRST);
      } catch (exception& x) {
        cerr << "ERROR: test5: As expected, block [" << side << "3,0,14] is not valid !" << endl
             << endl;
      }
    }
  }
}

void test6(geomtools::manager& a_mgr, bool draw_) {
  clog << "********** test6..." << endl;
  /**
   * Validated 2011-05-08 FM.
   *
   */
  uint32_t my_module_number = 0;
  snemo::geometry::gveto_locator GL;
  GL.set_geo_manager(a_mgr);
  GL.setModuleNumber(my_module_number);
  GL.initialize(datatools::properties{});

  snemo::geometry::gg_locator GGL{my_module_number, a_mgr, falaise::property_set{}};

  datatools::version_id geom_mgr_setup_vid;
  a_mgr.fetch_setup_version_id(geom_mgr_setup_vid);

  const geomtools::mapping& the_mapping = a_mgr.get_mapping();
  const geomtools::id_mgr::categories_by_name_col_type& categories =
      a_mgr.get_id_mgr().categories_by_name();

  geomtools::geom_id world_id(0, 0);
  const geomtools::geom_info& world_geom_info = the_mapping.get_geom_info(world_id);
  const geomtools::placement& wp = world_geom_info.get_world_placement();
  const geomtools::logical_volume& wlog = world_geom_info.get_logical();
  const geomtools::i_shape_3d& wshape = wlog.get_shape();

  uint32_t hall_type = categories.find("hall")->second.get_type();
  geomtools::geom_id hall_id(hall_type, 0);
  const geomtools::geom_info& hall_geom_info = the_mapping.get_geom_info(hall_id);
  const geomtools::placement& hp = hall_geom_info.get_world_placement();
  const geomtools::logical_volume& hlog = hall_geom_info.get_logical();
  const geomtools::i_shape_3d& hshape = hlog.get_shape();

  uint32_t module_type = categories.find("module")->second.get_type();
  geomtools::geom_id module_id(module_type, my_module_number);
  const geomtools::geom_info& module_geom_info = the_mapping.get_geom_info(module_id);
  const geomtools::placement& mp = module_geom_info.get_world_placement();
  const geomtools::logical_volume& mlog = module_geom_info.get_logical();
  const geomtools::i_shape_3d& mshape = mlog.get_shape();

  string source_cat = "source_strip";
  if (geom_mgr_setup_vid.has_major() && geom_mgr_setup_vid.get_major() == 1) {
    source_cat = "source_pad";
  }
  uint32_t source_volume_type = categories.find(source_cat)->second.get_type();
  uint32_t scin_block_type = categories.find("gveto_block")->second.get_type();
  uint32_t geiger_cell_type = categories.find("drift_cell_core")->second.get_type();

  geomtools::smart_id_locator CL2(the_mapping, scin_block_type);

  // define the track as a list of 3D-vector:
  list<geomtools::vector_3d> track;

  // define a list of geometry IDs of the hits:
  list<geomtools::geom_id> hit_ids;

  // the vertex:
  // geomtools::vector_3d vertex(-190.*CLHEP::mm, 1989.0*CLHEP::mm, 1600*CLHEP::mm);
  geomtools::vector_3d vertex(-40.0 * CLHEP::cm, -4.0 * CLHEP::m, -3.0 * CLHEP::m);

  // the first position of the track IS the vertex:
  geomtools::placement vertex_placement;
  vertex_placement.set_translation(vertex);
  track.push_back(vertex);

  // define an arbitrary propagation direction:
  geomtools::vector_3d propagation_dir(0.02, 0.2, 0.1);
  propagation_dir = propagation_dir.unit();
  double step_length = 1.0 * CLHEP::cm;

  // define the propagation step:
  geomtools::vector_3d step = step_length * propagation_dir;
  double track_length = 0.0;
  // double tolerance = 1.0 * CLHEP::um;

  // generate a track:
  for (geomtools::vector_3d track_pos = vertex; track_length < 10. * CLHEP::m; track_pos += step) {
    geomtools::geom_id hit_id;
    // First search for the ID correponding to this Geiger hit:
    GGL.find_geom_id(track_pos, geiger_cell_type, hit_id);
    // if this hit lies in some Geiger cell:
    if (hit_id.is_valid()) {
      // is the Geiger cell already hit ?
      if (find(hit_ids.begin(), hit_ids.end(), hit_id) == hit_ids.end()) {
        // clog << "Adding a new 'geiger hit' : " << hit_id << endl;
        // hit_ids.push_back(hit_id);
      }
    } else {
      // Search for the ID correponding to this Calo hit:
      // First search for the ID correponding to this Geiger hit:
      GL.find_geom_id(track_pos, scin_block_type, hit_id);
      // hit_id = CL2.get_geom_id(track_pos, scin_block_type);

      /*
      // Search for the ID correponding to this Calo hit:
      geomtools::geom_id hit_id2;
      hit_id2 = SCL.get_geom_id(track_pos,
      scin_block_type,
      tolerance);
      if(hit_id2.is_valid())
      {
      cerr << "DEVEL **** hit_id2=" << hit_id2 << "\n";
      if(! hit_id.is_valid())
      {
      cerr << "ERROR ******** hit_id=" << hit_id << "\n";
      }
      }
      */

      // if this hit lies in some scintillator block:
      if (hit_id.is_valid()) {
        /*
          if(hit_id2 != hit_id)
          {
          cerr << "ERROR **** " << hit_id2 << " != " << hit_id << "\n";
          }
        */

        // is the scintillator block already hit ?
        if (find(hit_ids.begin(), hit_ids.end(), hit_id) == hit_ids.end()) {
          clog << "Adding a new 'calo hit' : " << hit_id << endl;
          hit_ids.push_back(hit_id);

          bool spurious = false;
          if (spurious) {
            // add spurious hit in one neighbour block :
            vector<geomtools::geom_id> neighbour_ids;
            int neighbours_mask = snemo::geometry::grid_mask_t::DIAG;
            if (hit_id.get(1) == 0) {
              neighbours_mask = snemo::geometry::grid_mask_t::SIDE;
            }

            neighbour_ids = GL.getNeighbourGIDs(hit_id, neighbours_mask);
            for (const auto& neighbour_id : neighbour_ids) {
              hit_ids.push_back(neighbour_id);
            }
          }
        }
      }
    }
    track_length += step.mag();
    track.push_back(track_pos);
  }

  bool test_gid = false;
  if (test_gid) {
    uint32_t side = 0;
    {
      geomtools::geom_id calo_hit_id;
      calo_hit_id.set_type(scin_block_type);
      calo_hit_id.set(0, 0);
      calo_hit_id.set(1, side);
      calo_hit_id.set(2, 1);
      calo_hit_id.set(3, 14);
      hit_ids.push_back(calo_hit_id);
    }
    {
      geomtools::geom_id calo_hit_id;
      calo_hit_id.set_type(scin_block_type);
      calo_hit_id.set(0, 0);
      calo_hit_id.set(1, side);
      calo_hit_id.set(2, 1);
      calo_hit_id.set(3, 1);
      hit_ids.push_back(calo_hit_id);
    }
    {
      geomtools::geom_id calo_hit_id;
      calo_hit_id.set_type(scin_block_type);
      calo_hit_id.set(0, 0);
      calo_hit_id.set(1, side);
      calo_hit_id.set(2, 1);
      calo_hit_id.set(3, 12);
      hit_ids.push_back(calo_hit_id);
    }
  }

  // Draw:
  if (draw_) {
    Gnuplot g1("lines");
    g1.set_title("sngeometry::test_manager_2\\nMapping test");
    g1.set_xlabel("x");
    g1.set_ylabel("y");
    g1.set_zlabel("z");

    // Create 3 temp files:
    // datatools::temp_file::g_devel = true;
    datatools::temp_file fout1(string("test_manager_2."));
    datatools::temp_file fout2(string("test_manager_2."));
    datatools::temp_file fout3(string("test_manager_2."));

    // Draw world volume:
    geomtools::gnuplot_draw::draw(fout1.out(), wp, wshape);
    // Draw hall volume:
    geomtools::gnuplot_draw::draw(fout1.out(), hp, hshape);
    // Draw module #0 volume:
    geomtools::gnuplot_draw::draw(fout1.out(), mp, mshape);

    // Draw vertex:
    // define a box representing the position along the track for display:
    geomtools::box point_shape(0.5 * CLHEP::cm, 0.5 * CLHEP::cm, 0.5 * CLHEP::cm);
    geomtools::gnuplot_draw::draw(fout1.out(), vertex_placement, point_shape);

    /** 2011-01-15, FM: The default  'resources/snemo/config_1.0/manager.conf' file
     * defines only one unique marge source pad as the source foil.
     * 2012-05-30, FM: 'resources/snemo/config_2.0/manager.conf' use multiple
     * source strips.
     * We draw them.
     */
    {
      const geomtools::geom_map::ginfo_ptr_collection_type& source_ginfo_array =
          the_mapping.get_ginfo_collection_with_type(source_volume_type);
      if (source_ginfo_array.empty()) {
        clog << "No source volumes are mapped with type '" << source_volume_type << "' !" << endl;
      }
      for (auto i : source_ginfo_array) {
        const geomtools::geom_info& a_geom_info = *i;
        //    const geomtools::geom_info & a_geom_info = the_mapping.get_geom_info(a_id);
        const geomtools::placement& a_placement = a_geom_info.get_world_placement();
        const geomtools::logical_volume& plog = a_geom_info.get_logical();
        const geomtools::i_shape_3d& a_shape = plog.get_shape();
        // clog << "Source volume geometry info: " << a_geom_info << endl;
        geomtools::gnuplot_draw::draw(fout1.out(), a_placement, a_shape);
      }
    }

    {
      // Draw some arbitrary scintillator blocks:
      // uint32_t side = 1;
      vector<geomtools::geom_id> neighbour_ids;
      int neighbours_mask = snemo::geometry::grid_mask_t::DIAG;
      geomtools::geom_id gid1;
      gid1.set_type(scin_block_type);
      gid1.set_address(my_module_number, 0, 0, 3, 1);
      // if (GL.is_block_partitioned()) {
      //  gid1.set(GL.get_part_address_index(), 1);
      //}
      neighbour_ids = GL.getNeighbourGIDs(gid1, neighbours_mask);
      for (vector<geomtools::geom_id>::const_iterator i = neighbour_ids.begin();
           i != neighbour_ids.end(); i++) {
        const geomtools::geom_info& a_geom_info = the_mapping.get_geom_info(*i);
        const geomtools::placement& a_placement = a_geom_info.get_world_placement();
        const geomtools::logical_volume& a_log = a_geom_info.get_logical();
        const geomtools::i_shape_3d& a_shape = a_log.get_shape();
        clog << "Scintillator block geometry info: " << a_geom_info << endl;
        geomtools::gnuplot_draw::draw(fout1.out(), a_placement, a_shape);
      }
      neighbours_mask = snemo::geometry::grid_mask_t::SIDE;
      geomtools::geom_id gid2;
      gid2.set_type(scin_block_type);
      gid2.set_address(my_module_number, 1, 1, 9, 1);
      // if (GL.is_block_partitioned()) {
      //  gid2.set(GL.get_part_address_index(), 1);
      //}
      neighbour_ids = GL.getNeighbourGIDs(gid2, neighbours_mask);
      for (vector<geomtools::geom_id>::const_iterator i = neighbour_ids.begin();
           i != neighbour_ids.end(); i++) {
        const geomtools::geom_info& a_geom_info = the_mapping.get_geom_info(*i);
        const geomtools::placement& a_placement = a_geom_info.get_world_placement();
        const geomtools::logical_volume& a_log = a_geom_info.get_logical();
        const geomtools::i_shape_3d& a_shape = a_log.get_shape();
        clog << "Scintillator block geometry info: " << a_geom_info << endl;
        geomtools::gnuplot_draw::draw(fout1.out(), a_placement, a_shape);
      }
    }

    // Draw the steps along the track:
    for (list<geomtools::vector_3d>::const_iterator i = track.begin(); i != track.end(); i++) {
      geomtools::placement a_placement;
      a_placement.set_translation(*i);
      geomtools::gnuplot_draw::draw(fout2.out(), a_placement, point_shape);
    }

    // Draw the shapes of the volumes associated to geom ID of the hits
    // along the track:
    for (list<geomtools::geom_id>::const_iterator i = hit_ids.begin(); i != hit_ids.end(); i++) {
      const geomtools::geom_id& a_id = *i;
      const geomtools::geom_info& a_geom_info = the_mapping.get_geom_info(a_id);
      const geomtools::placement& a_placement = a_geom_info.get_world_placement();
      const geomtools::logical_volume& a_log = a_geom_info.get_logical();
      const geomtools::i_shape_3d& a_shape = a_log.get_shape();
      clog << "Hit geometry info: " << a_geom_info << endl;
      geomtools::gnuplot_draw::draw(fout3.out(), a_placement, a_shape);
    }
    cerr << "**************** wait_for_key ********************" << endl;
    {
      ostringstream cmdstr;
      cmdstr << "splot ";
      cmdstr << "'" << fout1.get_filename() << "' title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename() << "' title 'track'  with lines";
      cmdstr << ", '" << fout3.get_filename() << "' title 'hits' with lines";
      g1.cmd(cmdstr.str());
      g1.showonscreen();  // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      ostringstream cmdstr;
      cmdstr << "set title \"XY-view\"; set size ratio -1; set grid; plot ";
      cmdstr << "'" << fout1.get_filename() << "' using 1:2 title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename() << "' using 1:2 title 'track' with lines";
      cmdstr << ", '" << fout3.get_filename() << "' using 1:2 title 'hits' with lines";
      g1.cmd(cmdstr.str());
      g1.showonscreen();  // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      ostringstream cmdstr;
      cmdstr << "set title \"YZ-view\"; set size ratio -1; set grid; plot ";
      cmdstr << "'" << fout1.get_filename() << "' using 2:3 title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename() << "' using 2:3 title 'track' with lines";
      cmdstr << ", '" << fout3.get_filename() << "' using 2:3 title 'hits' with lines";
      g1.cmd(cmdstr.str());
      g1.showonscreen();  // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      ostringstream cmdstr;
      cmdstr << "set title \"XZ-view\"; set size ratio -1; set grid; plot ";
      cmdstr << "'" << fout1.get_filename() << "' using 1:3 title 'geometry' with lines";
      cmdstr << ", '" << fout2.get_filename() << "' using 1:3 title 'track' with lines";
      cmdstr << ", '" << fout3.get_filename() << "' using 1:3 title 'hits' with lines";
      g1.cmd(cmdstr.str());
      g1.showonscreen();  // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

  }  // Draw
}

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'snemo::geometry::gveto_locator' !" << endl;

    string manager_config_file;
    bool do_test1 = true;
    bool do_test2 = true;
    bool do_test3 = false;
    bool do_test4 = true;
    bool do_test5 = true;
    bool do_test6 = true;

    if (manager_config_file.empty()) {
      manager_config_file = "@falaise:snemo/demonstrator/geometry/GeometryManager.conf";
    }
    datatools::fetch_path_with_env(manager_config_file);
    clog << datatools::io::notice << "Manager config. file : '" << manager_config_file << "'"
         << endl;

    // load properties from the configuration file:
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);
    geomtools::manager my_manager;
    my_manager.set_logging_priority(datatools::logger::PRIO_TRACE);

    my_manager.initialize(manager_config);
    my_manager.get_factory().tree_dump(clog, "The SuperNEMO geometry model factory");
    my_manager.get_id_mgr().tree_dump(clog, "The SuperNEMO geometry ID manager");

    long seed = 12345;
    srand48(seed);

    if (do_test1) {
      clog << "\n*** TEST 1 *** \n : ";
      test1(my_manager);
    }

    size_t nhits = 10000;
    if (do_test2) {
      clog << "\n*** TEST 2 *** \n : ";
      test2(my_manager, nhits, false);
    }

    if (do_test3) {
      clog << "\n*** TEST 3 *** \n : ";
      test3(my_manager, nhits, false);
    }

    if (do_test4) {
      clog << "\n*** TEST 4 *** \n : ";
      test4(my_manager);
    }

    if (do_test5) {
      clog << "\n*** TEST 5 *** \n : ";
      test5(my_manager);
    }

    if (do_test6) {
      clog << "\n*** TEST 6 *** \n : ";
      test6(my_manager, false);
    }

  } catch (exception& x) {
    cerr << "ERROR: " << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "ERROR: "
         << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
