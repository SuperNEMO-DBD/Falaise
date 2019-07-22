
#include <mybhep/histogram.h>

#include <iostream>

static double ShootRandomFlat();
static double ShootRandomGauss(double, double);
static double ShootRandomBreitWigner(double, double);

//////////////////////////////////////////////////////////////////////////////
void HCL_print(mybhep::HCL::Base1D& aHistogram)
//////////////////////////////////////////////////////////////////////////////
// A la HPRINT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::cout << aHistogram.dimension() << aHistogram.title() << std::endl;
  std::cout << " * ENTRIES = " << aHistogram.allEntries()
            << " * ALL CHANNELS = " << aHistogram.sumBinHeights()
            << " * UNDERFLOW = " << aHistogram.binHeight(mybhep::HCL::Axis::UNDERFLOW_BIN)
            << " * OVERFLOW = " << aHistogram.binHeight(mybhep::HCL::Axis::OVERFLOW_BIN)
            << std::endl;
  std::cout << " * BIN WID = " << aHistogram.axis().binWidth(0)
            << " * MEAN VALUE = " << aHistogram.mean() << " * R . M . S = " << aHistogram.rms()
            << std::endl;

  // Some bins :
  int bins = aHistogram.axis().bins();
  std::cout << " * ENTRIES[0]   = " << aHistogram.binEntries(0)
            << " * HEIGHT[0] = " << aHistogram.binHeight(0)
            << " * ERROR[0] = " << aHistogram.binError(0) << std::endl;
  std::cout << " * ENTRIES[N/2] = " << aHistogram.binEntries(bins / 2)
            << " * HEIGHT[N/2] = " << aHistogram.binHeight(bins / 2)
            << " * ERROR[N/2] = " << aHistogram.binError(bins / 2) << std::endl;
  std::cout << " * ENTRIES[N-1] = " << aHistogram.binEntries(bins - 1)
            << " * HEIGHT[N-1] = " << aHistogram.binHeight(bins - 1)
            << " * ERROR[N-1] = " << aHistogram.binError(bins - 1) << std::endl;
}
//////////////////////////////////////////////////////////////////////////////
void HCL_print(mybhep::HCL::Base2D& aHistogram)
//////////////////////////////////////////////////////////////////////////////
// A la HPRINT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::cout << aHistogram.dimension() << aHistogram.title() << std::endl;
  std::cout << " * ENTRIES = " << aHistogram.allEntries() << std::endl;

  //   6 | 7 | 8
  //  -----------
  //   3 | 4 | 5
  //  -----------
  //   0 | 1 | 2

  double height_0 =
      aHistogram.binHeight(mybhep::HCL::Axis::UNDERFLOW_BIN, mybhep::HCL::Axis::UNDERFLOW_BIN);
  double height_2 =
      aHistogram.binHeight(mybhep::HCL::Axis::OVERFLOW_BIN, mybhep::HCL::Axis::UNDERFLOW_BIN);
  double height_6 =
      aHistogram.binHeight(mybhep::HCL::Axis::UNDERFLOW_BIN, mybhep::HCL::Axis::OVERFLOW_BIN);
  double height_8 =
      aHistogram.binHeight(mybhep::HCL::Axis::OVERFLOW_BIN, mybhep::HCL::Axis::OVERFLOW_BIN);

  int i, j;

  double height_1 = 0;
  double height_7 = 0;
  for (i = 0; i < aHistogram.xAxis().bins(); i++) {
    height_1 += aHistogram.binHeight(i, mybhep::HCL::Axis::UNDERFLOW_BIN);
    height_7 += aHistogram.binHeight(i, mybhep::HCL::Axis::OVERFLOW_BIN);
  }

  double height_3 = 0;
  double height_5 = 0;
  for (j = 0; j < aHistogram.yAxis().bins(); j++) {
    height_3 += aHistogram.binHeight(mybhep::HCL::Axis::UNDERFLOW_BIN, j);
    height_5 += aHistogram.binHeight(mybhep::HCL::Axis::OVERFLOW_BIN, j);
  }

  double height_4 = 0;
  for (i = 0; i < aHistogram.xAxis().bins(); i++) {
    for (j = 0; j < aHistogram.yAxis().bins(); j++) {
      height_4 += aHistogram.binHeight(i, j);
    }
  }

  std::cout << " " << height_6 << " " << height_7 << " " << height_8 << std::endl;
  std::cout << " " << height_3 << " " << height_4 << " " << height_5 << std::endl;
  std::cout << " " << height_0 << " " << height_1 << " " << height_2 << std::endl;

  // Some bins :
  int xbins = aHistogram.xAxis().bins();
  int ybins = aHistogram.yAxis().bins();
  std::cout << " * ENTRIES[0,0]     = " << aHistogram.binEntries(0, 0)
            << " * HEIGHT[0,0] = " << aHistogram.binHeight(0, 0)
            << " * ERROR[0,0] = " << aHistogram.binError(0, 0) << std::endl;
  std::cout << " * ENTRIES[N/2,N/2] = " << aHistogram.binEntries(xbins / 2, ybins / 2)
            << " * HEIGHT[N/2,N/2] = " << aHistogram.binHeight(xbins / 2, ybins / 2)
            << " * ERROR[N/2,N/2] = " << aHistogram.binError(xbins / 2, ybins / 2) << std::endl;
  std::cout << " * ENTRIES[N-1,N-1] = " << aHistogram.binEntries(xbins - 1, ybins - 1)
            << " * HEIGHT[N-1,N-1] = " << aHistogram.binHeight(xbins - 1, ybins - 1)
            << " * ERROR[N-1,N-1] = " << aHistogram.binError(xbins - 1, ybins - 1) << std::endl;
}
//////////////////////////////////////////////////////////////////////////////
int main(int aArgc, char** aArgs)
//////////////////////////////////////////////////////////////////////////////
//  Book and fill some histos.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  bool print = true;
  if (aArgc == 2) {
    std::string s = aArgs[1];
    if (s == "-noprint") print = false;
  }

  int entries = 1000000;
  {
    mybhep::HCL::Histogram1D* histogram = new mybhep::HCL::Histogram1D("Gauss", 100, -5, 5);
    for (int count = 0; count < entries; count++) {
      histogram->fill(ShootRandomGauss(1, 2), 1.4);
    }
    if (print) HCL_print(*histogram);
    delete histogram;
  }

  {
    mybhep::HCL::Histogram1D* histogram = new mybhep::HCL::Histogram1D("BW", 100, -5, 5);
    for (int count = 0; count < entries; count++) {
      histogram->fill(ShootRandomBreitWigner(0, 1), 2.3);
    }
    if (print) HCL_print(*histogram);
    delete histogram;
  }

  {
    mybhep::HCL::Profile1D* histogram = new mybhep::HCL::Profile1D("Profile", 100, -5, 5, -2, 2);
    for (int count = 0; count < entries; count++) {
      histogram->fill(ShootRandomGauss(1, 2), ShootRandomBreitWigner(0, 1), 1);
    }
    if (print) HCL_print(*histogram);
    delete histogram;
  }

  {
    mybhep::HCL::Histogram2D* histogram =
        new mybhep::HCL::Histogram2D("Gauss_BW", 100, -5, 5, 100, -2, 2);
    for (int count = 0; count < entries; count++) {
      histogram->fill(ShootRandomGauss(1, 2), ShootRandomBreitWigner(0, 1), 0.8);
    }
    if (print) HCL_print(*histogram);

    {
      mybhep::HCL::Base1D* projection = histogram->projectionX();
      if (!projection) return -1;
      projection->setTitle("Gauss_BW_projectionX");
      if (print) HCL_print(*projection);
      delete projection;
    }

    {
      mybhep::HCL::Base1D* projection = histogram->projectionY();
      if (!projection) return -1;
      projection->setTitle("Gauss_BW_projectionY");
      if (print) HCL_print(*projection);
      delete projection;
    }

    {
      mybhep::HCL::Base1D* slice = histogram->sliceX(40, 60);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceX");
      if (print) HCL_print(*slice);
      delete slice;
    }

    {
      mybhep::HCL::Base1D* slice =
          histogram->sliceX(mybhep::HCL::Axis::UNDERFLOW_BIN, mybhep::HCL::Axis::UNDERFLOW_BIN);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceX_UNDER");
      if (print) HCL_print(*slice);
      delete slice;
    }

    {
      mybhep::HCL::Base1D* slice =
          histogram->sliceX(mybhep::HCL::Axis::OVERFLOW_BIN, mybhep::HCL::Axis::OVERFLOW_BIN);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceX_OVER");
      if (print) HCL_print(*slice);
      delete slice;
    }

    {
      mybhep::HCL::Base1D* slice = histogram->sliceY(30, 50);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceY");
      if (print) HCL_print(*slice);
      delete slice;
    }

    {
      mybhep::HCL::Base1D* slice =
          histogram->sliceY(mybhep::HCL::Axis::UNDERFLOW_BIN, mybhep::HCL::Axis::UNDERFLOW_BIN);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceY_UNDER");
      if (print) HCL_print(*slice);
      delete slice;
    }

    {
      mybhep::HCL::Base1D* slice =
          histogram->sliceY(mybhep::HCL::Axis::OVERFLOW_BIN, mybhep::HCL::Axis::OVERFLOW_BIN);
      if (!slice) return -1;
      slice->setTitle("Gauss_BW_sliceY_OVER");
      if (print) HCL_print(*slice);
      delete slice;
    }

    delete histogram;
  }

  return 0;
}

#include <mybhep/stdlib.h>  //rand
#include <math.h>
#ifndef M_PI
#define M_PI 3.1415926535897931160E0
#define M_PI_2 1.5707963267948965580E0
#endif
//////////////////////////////////////////////////////////////////////////////
double ShootRandomFlat()
//////////////////////////////////////////////////////////////////////////////
// Shoot random numbers according a flat distribution.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  double value = (double)::rand();
  value /= (double)RAND_MAX;
  return value;
}
//////////////////////////////////////////////////////////////////////////////
double ShootRandomGauss(double aMean, double aStdDev)
//////////////////////////////////////////////////////////////////////////////
//  Shoot random numbers according a
// gaussian distribution of mean 0 and sigma 1.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  double v1, v2, r, fac;
  do {
    v1 = 2.0 * ShootRandomFlat() - 1.0;
    v2 = 2.0 * ShootRandomFlat() - 1.0;
    r = v1 * v1 + v2 * v2;
  } while (r > 1.0);
  fac = ::sqrt(-2.0 * ::log(r) / r);
  return (v2 * fac) * aStdDev + aMean;
}
//////////////////////////////////////////////////////////////////////////////
double ShootRandomBreitWigner(double aMean, double aGamma)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  double rval = 2.0 * ShootRandomFlat() - 1.0;
  double displ = 0.5 * aGamma * ::tan(rval * M_PI_2);
  return aMean + displ;
}
