// -*- mode: c++ ; -*-
// test_composite_volume.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include <geomtools/box.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/polycone.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>

#include <snvisualization/detector/box_volume.h>
#include <snvisualization/detector/composite_volume.h>
#include <snvisualization/detector/polycone_volume.h>
#include <snvisualization/detector/sphere_volume.h>

#include <datatools/ioutils.h>

#include <TGeoCompositeShape.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoVolume.h>
#include <TRint.h>

using namespace std;

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'i_root_volume'!" << endl;

    bool debug = false;
    bool verbose = false;

    bool visu_mode = false;
    bool ogl_mode = false;
    bool raytrace_mode = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else if (option == "--visu-mode") {
          visu_mode = true;
        } else if (option == "--ogl-mode") {
          visu_mode = true;
          ogl_mode = true;
        } else if (option == "--raytrace-mode") {
          visu_mode = true;
          raytrace_mode = true;
        } else {
          clog << datatools::io::warning << "Ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        { clog << datatools::io::warning << "Ignoring argument '" << argument << "'!" << endl; }
      }
      iarg++;
    }

    const double x = 300.0;
    const double y = 300.0;
    const double w = 300.0;
    const double z = 140.0;
    const double h = 100.0;
    const double optical_glue_thickness = 1.0;
    const double r = 85.0;
    const double tapered_r = 80.0;
    const double tapered_angle = 50.0 / 57.2957;

    geomtools::box mbox(x, y, z);
    geomtools::sphere msphere(r);

    const double ze = 0.5 * z - r - h;
    geomtools::placement extrusion_placement(0, 0, ze, 0, 0, 0);
    geomtools::subtraction_3d solid_0;
    solid_0.set_shapes(mbox, msphere, extrusion_placement);

    // should use some intrinsic skin value:
    const double eps = 0.0 * z;
    const double angle_e = acos(abs(ze + 0.5 * z) / r);
    const double re = r * sin(angle_e);
    const double lt = re / tan(tapered_angle);
    const double zt = -0.5 * z - lt;

    geomtools::polycone removed_cone;
    removed_cone.add(zt, 0.0, false);
    removed_cone.add(0.5 * z + eps, (0.5 * z - zt) * tan(tapered_angle), true);

    geomtools::placement cone_placement;
    geomtools::intersection_3d solid;
    solid.set_shapes(solid_0, removed_cone, cone_placement);

    // make a composite volume
    namespace det = snemo::visualization::detector;
    det::composite_volume* my_composite_volume = new det::composite_volume;
    my_composite_volume->construct(solid);

    if (visu_mode) {
      TRint my_app("Volume Visualization", 0, 0);

      TGeoVolume* top = gGeoManager->MakeBox("top", 0, 1000., 1000., 1000.);
      gGeoManager->SetTopVolume(top);

      top->AddNodeOverlap(static_cast<const TGeoVolume*>(my_composite_volume->get_volume()), 1, 0);

      gGeoManager->CloseGeometry();
      gGeoManager->SetNsegments(100);

      if (raytrace_mode)
        top->Raytrace();
      else if (ogl_mode)
        top->Draw("ogl");
      else
        top->Draw();

      my_app.Run();
    }
  } catch (exception& x) {
    cerr << datatools::io::error << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << datatools::io::error << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_composite_volume.cxx
