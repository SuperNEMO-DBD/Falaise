// test_snemo_processing_calo_uniformity.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/processing/calo_signal_baseline.h>
#include <falaise/snemo/processing/mock_calorimeter_s2c_module_utils.h>
#include <falaise/snemo/processing/calo_uniformity_correction_factor.h>

enum calo_type {
  MWALL8 = 0,
  MWALL5 = 1,
  XWALL  = 2,
  GVETO  = 3
};

void test0(const calo_type ctype_);
void test1(const calo_type ctype_);

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::processing::calo_signal_baseline'!" << std::endl;
    test0(MWALL8);
    test0(MWALL5);
    test0(XWALL);
    test0(GVETO);
    // test1(MWALL8);
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::clog << "The end." << std::endl;
  falaise::terminate();
  return (error_code);
}

void test0(const calo_type ctype_)
{
  std::clog << "\ntest0" << std::endl;
  namespace snproc = snemo::processing;

  snproc::CaloUniformityCorrection caloUniformityCorrection;
  std::string pol3dParametersPath[4] = {
    "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_8inch.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_5inch.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_XW.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_GV.db"};
  for (int i = 0; i < 4; i++) {
    datatools::fetch_path_with_env(pol3dParametersPath[i]);
  }
  // Parse all fit parameters files:
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[0]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_mwall_8inch.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[1]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_mwall_5inch.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[2]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_xwall.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[3]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_gveto.push_back(par);
    }
  }
 
  geomtools::geom_id gid1(1302, 0, 0, 2, 4, 1);
  geomtools::geom_id gid2(1302, 0, 0, 2, 0, 1);
  geomtools::geom_id gid3(1232, 0, 0, 0, 0, 3);
  geomtools::geom_id gid4(1252, 0, 0, 1, 4);
  
  std::string title = "MWALL8";
  geomtools::geom_id * gidPtr = &gid1;
  // Implicit unit: mm
  double xmin = -snproc::CaloUniformityCorrection::mwallScinFrontBlockX_mm / 2;
  double ymin = -snproc::CaloUniformityCorrection::mwallScinFrontBlockY_mm / 2;
  double z    =  snproc::CaloUniformityCorrection::mwallScinFrontBlockZ_mm / 2;
  if (ctype_ == MWALL5) {
    title = "MWALL5";
    gidPtr = &gid2;
  } else if (ctype_ == XWALL) {
    title = "XWALL";
    gidPtr = &gid3;
    xmin = -snproc::CaloUniformityCorrection::xwallScinBlockX_mm / 2;
    ymin = -snproc::CaloUniformityCorrection::xwallScinBlockY_mm / 2;
    z    =  snproc::CaloUniformityCorrection::xwallScinBlockZ_mm / 2;
  } else if (ctype_ == GVETO) {
    title = "GVETO";
    gidPtr = &gid4;
    xmin = -snproc::CaloUniformityCorrection::gvetoScinBlockX_mm / 2;
    ymin = -snproc::CaloUniformityCorrection::gvetoScinBlockY_mm / 2;
    z    =  snproc::CaloUniformityCorrection::gvetoScinBlockZ_mm / 2;
  }
  xmin *= CLHEP::mm;
  ymin *= CLHEP::mm;
  z *= CLHEP::mm;
  double xmax = -xmin;
  double ymax = -ymin;
  double dx = 5.0 * CLHEP::mm;
  double dy = 5.0 * CLHEP::mm;
  std::ofstream uniformityXyCoeff("calo_uniformity_xy_coeff-" + title + ".data");
  for (double x = xmin; x < xmax + 1e-4 * dx; x += dx) {
    for (double y = ymin; y < ymax + 1e-4 * dy; y += dy) {
      geomtools::vector_3d scinPosVector(x,y,z);
      double c = caloUniformityCorrection.correction_factor(*gidPtr, scinPosVector);
      uniformityXyCoeff << x << ' ' << y << ' ' << c << '\n';
    }
    uniformityXyCoeff << '\n';
  }
   
  return;
}


void test1(const calo_type ctype_)
{
  std::clog << "\ntest1" << std::endl;
  namespace snproc = snemo::processing;

  snproc::CaloUniformityCorrection caloUniformityCorrection;
  std::string pol3dParametersPath[4] = {
    "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_8inch.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_5inch.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_XW.db",
      "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_GV.db"};
  for (int i = 0; i < 4; i++) {
    datatools::fetch_path_with_env(pol3dParametersPath[i]);
  }
  // Parse all fit parameters files:
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[0]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_mwall_8inch.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[1]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_mwall_5inch.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[2]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_xwall.push_back(par);
    }
  }
  {
    std::ifstream pol3dParametersFile(pol3dParametersPath[3]);
    double par, par_err;
    while(pol3dParametersFile >> par >> par_err) {
      caloUniformityCorrection.parameters_gveto.push_back(par);
    }
  }
 
  geomtools::geom_id gid1(1302, 0, 0, 2, 4, 1);
  geomtools::geom_id gid2(1302, 0, 0, 2, 0, 1);
  geomtools::geom_id gid3(1232, 0, 0, 0, 0, 3);
  geomtools::geom_id gid4(1252, 0, 0, 1, 4);
  
  std::string title = "MWALL8";
  geomtools::geom_id * gidPtr = &gid1;
  // Implicit unit: mm
  double xmin = -snproc::CaloUniformityCorrection::mwallScinFrontBlockX_mm / 2;
  double zmin = -snproc::CaloUniformityCorrection::mwallScinFrontBlockZ_mm / 2;
  double y    =  0.0;
  if (ctype_ == MWALL5) {
    title = "MWALL5";
    gidPtr = &gid2;
  } else if (ctype_ == XWALL) {
    title = "XWALL";
    gidPtr = &gid3;
    xmin = -snproc::CaloUniformityCorrection::xwallScinBlockX_mm / 2;
    zmin = -snproc::CaloUniformityCorrection::xwallScinBlockZ_mm / 2;
  } else if (ctype_ == GVETO) {
    title = "GVETO";
    gidPtr = &gid4;
    xmin = -snproc::CaloUniformityCorrection::gvetoScinBlockX_mm / 2;
    zmin = -snproc::CaloUniformityCorrection::gvetoScinBlockZ_mm / 2;
  }
  xmin *= CLHEP::mm;
  zmin *= CLHEP::mm;
  double xmax = -xmin;
  double zmax = -zmin;
  double dx = 2.0 * CLHEP::mm;
  double dz = 2.0 * CLHEP::mm;
  std::ofstream uniformityXzCoeff("calo_uniformity_xz_coeff-" + title + ".data");
  for (double z = zmin - 163 * CLHEP::mm ; z < zmax + 1e-4 * dz; z += dz) {
    for (double x = xmin; x < xmax + 1e-4 * dx; x += dx) {
      geomtools::vector_3d scinPosVector(x,y,z);
      double c = caloUniformityCorrection.correction_factor(*gidPtr, scinPosVector);
      if (ctype_ == MWALL8 and z < zmin) {
	if (std::fabs(x) > std::fabs(xmin) - 10 * CLHEP::mm) {
	  c = std::numeric_limits<double>::quiet_NaN();
	}
	if (std::hypot(x, z + 215 * CLHEP::mm) < 100 * CLHEP::mm) {
	  c = std::numeric_limits<double>::quiet_NaN();
	}
      }
      uniformityXzCoeff << x << ' ' << z << ' ' << c << '\n';
    }
    uniformityXzCoeff << '\n';
  }
   
  return;
}


void test2()
{
  std::clog << "\ntest2" << std::endl;
  namespace snproc = snemo::processing;

  // double posMm[3] = {0.0, 0.0, 0.0};
  
}
