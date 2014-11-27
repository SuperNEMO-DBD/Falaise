// -*- mode: c++ ; -*-
// test_root_volume.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <falaise/snemo/detector/box_volume.h>
#include <geomtools/box.h>

#include <falaise/snemo/detector/sphere_volume.h>
#include <geomtools/sphere.h>

#include <falaise/snemo/detector/cylinder_volume.h>
#include <geomtools/cylinder.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_FAILURE;
  try {
    clog << "Test program for class 'i_root_volume'!" << endl;

    namespace det = snemo::visualization::detector;

    // make a box
    det::i_root_volume* my_box_volume = new det::box_volume;
    geomtools::box mbox(1., 1., 1.);
    my_box_volume->_construct(mbox);
    my_box_volume->dump();

    // make a sphere
    det::i_root_volume* my_sphere_volume = new det::sphere_volume;
    geomtools::sphere msphere(2.);
    my_sphere_volume->_construct(msphere);
    my_sphere_volume->dump();

    // make a cylinder
    det::i_root_volume* my_cylinder_volume = new det::cylinder_volume;
    geomtools::cylinder mcylinder(0.5, 5.);
    my_cylinder_volume->_construct(mcylinder);
    my_cylinder_volume->dump();

    return EXIT_SUCCESS;
  } catch (exception & x) {
    cerr << x.what () << endl;
  } catch (...) {
    cerr << "unexpected error!" << endl;
  }
  return error_code;
}

// end of test_root_volume.cxx
