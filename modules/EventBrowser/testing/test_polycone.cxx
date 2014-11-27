// -*- mode: c++ ; -*-
// test_polycone.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <snvisualization/detector/polycone_volume.h>
#include <geomtools/polycone.h>
#include <datatools/ioutils.h>

#include <TRint.h>
#include <TCanvas.h>
#include <TGeoVolume.h>
#include <TGeoManager.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  const string appname_prefix = "test_polycone: ";

  int error_code = EXIT_SUCCESS;
  try
    {
      clog << datatools::io::notice
           << appname_prefix
           << "Test program for class 'polycone_volume'!" << endl;

      bool debug     = false;
      bool verbose   = false;

      bool visu_mode     = false;
      bool ogl_mode      = false;
      bool raytrace_mode = false;

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
              else if ((option == "-v") || (option == "--verbose"))
                {
                  verbose = true;
                }
              else if (option == "--visu-mode")
                {
                  visu_mode = true;
                }
              else if (option == "--ogl-mode")
                {
                  visu_mode = true;
                  ogl_mode  = true;
                }
              else if (option == "--raytrace-mode")
                {
                  visu_mode     = true;
                  raytrace_mode = true;
                }
              else
                {
                  clog << datatools::io::warning
                       << appname_prefix
                       << "Ignoring option '" << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << datatools::io::warning
                     << appname_prefix
                     << "Ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      namespace det = snemo::visualization::detector;

      const double pmt_radius      = 100. * CLHEP::mm;
      const double pmt_front_a     = 74.  * CLHEP::mm;
      const double pmt_back_a      = 80.  * CLHEP::mm;
      const double pmt_base_radius = 50.  * CLHEP::mm;
      const double pmt_length      = 240. * CLHEP::mm;

      geomtools::polycone my_polycone;

      double a = pmt_front_a;
      double b = pmt_radius;
      double dtheta = M_PI / 50.;
      my_polycone.add (a, 0.0);
      for (double theta = dtheta;
	   theta < 0.5 * M_PI - 0.1 * dtheta;
	   theta += dtheta)
	{
	  double z = b / sqrt (pow (tan (theta), 2) + pow (b / a, 2));
	  double r = b * sqrt (1.0 - (z / a) * (z / a));
	  my_polycone.add (z, r);
	}
      double a2 = pmt_back_a;
      for (double theta = 0.5 * M_PI;
	   theta > 0.2 * M_PI;
	   theta -= dtheta)
	{
	  double z = b / sqrt (pow (tan (theta), 2) + pow (b / a2, 2));
	  double r = b * sqrt (1.0 - (z / a2) * (z / a2));
	  my_polycone.add (- z, r);
	}
      my_polycone.add (-pmt_front_a, pmt_base_radius);
      my_polycone.add ((pmt_front_a - pmt_length), pmt_base_radius);

      // make a polycone from a geomtools::polycone volume
      det::polycone_volume* my_polycone_volume = new det::polycone_volume ();
      my_polycone_volume->construct (my_polycone);
      if (debug)
        {
          my_polycone_volume->dump ();
        }

      if (visu_mode)
        {
          TRint my_app ("Polycone Visualization", 0, 0);

          TCanvas* canvas = new TCanvas ();
          TGeoVolume* top = gGeoManager->MakeBox ("top", 0, 10., 10., 10.);
          gGeoManager->SetTopVolume (top);

          TGeoVolume* gvolume
            = static_cast<TGeoVolume*>(my_polycone_volume->grab_volume ());

          gvolume->SetLineColor (kOrange-1);
          gvolume->SetFillColor (kOrange-1);
          if (ogl_mode)
            {
              gvolume->SetTransparency (60);
            }

          top->AddNodeOverlap (gvolume, 1, 0);
          gGeoManager->CloseGeometry ();

          if (raytrace_mode) top->Raytrace ();
          else if (ogl_mode) top->Draw ("ogl");
          else               top->Draw ();

          my_app.Run ();
        }

    }
  catch (exception & x)
    {
      cerr << datatools::io::error
           << appname_prefix
           << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << datatools::io::error
           << appname_prefix
           << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_polycone.cxx
