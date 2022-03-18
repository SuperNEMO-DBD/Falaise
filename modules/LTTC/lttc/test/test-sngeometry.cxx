// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/sngeometry.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-sngeometry.data");

    lttc::tracker sntracker;
    lttc::tracker_drawer trackerDrawer(sntracker);
    trackerDrawer.draw(fout);
    fout << '\n';
    fout << '\n';

    lttc::point2 start(-34.0 * CLHEP::cm, -23.0 * CLHEP::cm);
    lttc::point2 stop(+14.0 * CLHEP::cm, +42.0 * CLHEP::cm);
    lttc::draw_point(fout, start);
    lttc::draw_point(fout, stop);
    fout << '\n';
    fout << '\n';

    std::set<lttc::cell_id> crossedCells;
    if (sntracker.intersect_segment(start, stop, crossedCells)) {    
      std::clog << "Nb of crossed cells = " << crossedCells.size() << '\n';
      for (const auto & cid : crossedCells) {
        lttc::point2 cellPos = sntracker.cell_position(cid);
        double rcell = sntracker.rcell;
        fout << "#@tracker-intersect-segment=crossed-cell" << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() + rcell << '\n';
        fout << cellPos.x() + rcell << ' ' <<  cellPos.y() + rcell << '\n';
        fout << cellPos.x() + rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << cellPos.x() - rcell << ' ' <<  cellPos.y() - rcell << '\n';
        fout << '\n';
        // fout << cellPos.x() - rcell << ' ' <<  cellPos.y() - rcell << '\n';
        // fout << cellPos.x() + rcell << ' ' <<  cellPos.y() + rcell << '\n';
        // fout << '\n';
        // fout << cellPos.x() - rcell << ' ' <<  cellPos.y() + rcell << '\n';
        // fout << cellPos.x() + rcell << ' ' <<  cellPos.y() - rcell << '\n';
        // fout << '\n';
      }
    }
    fout << '\n';
    fout << '\n';
    
    fout.close();
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
