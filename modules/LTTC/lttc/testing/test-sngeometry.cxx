// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// Falaise:
#include <falaise/falaise.h>
#include <falaise/snemo/geometry/manager.h>
#include <falaise/snemo/processing/detector_description.h>

// This project:
#include <lttc/sngeometry.hh>

int main(void)
{
  int code = EXIT_SUCCESS;
  falaise::initialize();
  try {
    std::ofstream fout("test-sngeometry.data");

    geomtools::manager geoMgr;
    snemo::geometry::initialize_manager(geoMgr);
    snemo::processing::detector_description detectorDesc;
    detectorDesc.set_geometry_manager(geoMgr);
    
    lttc::tracker sntracker(detectorDesc.get_gg_locator());
    lttc::tracker_drawer trackerDrawer(sntracker);
    trackerDrawer.draw(fout);
    fout << '\n';
    fout << '\n';

    falaise::geometry::point2 start(-34.0 * CLHEP::cm, -23.0 * CLHEP::cm);
    falaise::geometry::point2 stop(+14.0 * CLHEP::cm, +42.0 * CLHEP::cm);
    falaise::geometry::draw_point(fout, start);
    falaise::geometry::draw_point(fout, stop);
    fout << '\n';
    fout << '\n';

    std::set<lttc::cell_id> crossedCells;
    if (sntracker.intersect_segment(start, stop, crossedCells)) {    
      std::clog << "Nb of crossed cells = " << crossedCells.size() << '\n';
      for (const auto & cid : crossedCells) {
        falaise::geometry::point2 cellPos = sntracker.cell_position(cid);
        double rcell = sntracker.rcell;
        fout << "#@tracker-intersect-segment=crossed-cell" << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() + rcell << '\n';
        fout << cellPos.x() + rcell << ' ' <<  cellPos.y() + rcell << '\n';
        fout << cellPos.x() + rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << '\n';
      }
    }
    fout << '\n';
    fout << '\n';
    
    fout.close();
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  falaise::terminate();
  return code;
}
