// this :
#include <mybhep/histogram.h>

#include <math.h>

#define NotFound (-1)

#define AxisX 0
#define AxisY 1
#define AxisZ 2

//#define HCL_CHECK_OPTIMIZE
//#define PREC 1e10-24
//#define IS_EQUAL(a,b) (fabs((a)-(b))<=PREC)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::HistogramData::HistogramData()
    : fDimension(0),
      fBinNumber(0),
      fMode(0),
      fProfile(false),
      fCutV(false),
      fMinV(0),
      fMaxV(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::HistogramData::HistogramData(const mybhep::HCL::HistogramData& aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  copy(aFrom);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::HistogramData::~HistogramData()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::HistogramData::copy(const mybhep::HCL::HistogramData& aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aFrom.fTitle;
  fDimension = aFrom.fDimension;
  fBinNumber = aFrom.fBinNumber;
  fMode = aFrom.fMode;
  // Arrays :
  fAxes = aFrom.fAxes;
  fBinEntries = aFrom.fBinEntries;
  fBinSw = aFrom.fBinSw;
  fBinSw2 = aFrom.fBinSw2;
  fBinSxw = aFrom.fBinSxw;
  fBinSx2w = aFrom.fBinSx2w;
  // Profile :
  fProfile = aFrom.fProfile;
  fBinSvw = aFrom.fBinSvw;
  fBinSv2w = aFrom.fBinSv2w;
  fCutV = aFrom.fCutV;
  fMinV = aFrom.fMinV;
  fMaxV = aFrom.fMaxV;
  // Args :
  fDoubles = aFrom.fDoubles;
  fInts = aFrom.fInts;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::Histogram()
//////////////////////////////////////////////////////////////////////////////
// Used in Lab/THistogram.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // printf("debug : Histogram %lu\n",this);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::Histogram(int aDimension, const std::vector<int>& aNumbers,
                                  const std::vector<double>& aMins,
                                  const std::vector<double>& aMaxs, bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  configure(aDimension, aNumbers, aMins, aMaxs, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::Histogram(int aDimension, const std::vector<std::vector<double> >& aEdges,
                                  bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  configure(aDimension, aEdges, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::Histogram(const mybhep::HCL::Histogram& aHistogram)
    : HistogramData(aHistogram)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  updateFastGetters();
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::Histogram(const mybhep::HCL::HistogramData& aData)
    : HistogramData(aData)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  updateFastGetters();
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram::~Histogram()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram& mybhep::HCL::Histogram::operator=(const Histogram& aHistogram)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  copy(aHistogram);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::configure(int aDimension, const std::vector<int>& aNumbers,
                                       const std::vector<double>& aMins,
                                       const std::vector<double>& aMaxs, bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // Clear :
  clear();
  // Some checks :
  if (aDimension <= 0) return false;
  fAxes.resize(aDimension);
  // Setup axes :
  for (int iaxis = 0; iaxis < aDimension; iaxis++) {
    if (!fAxes[iaxis].configure(aNumbers[iaxis], aMins[iaxis], aMaxs[iaxis])) {
      fAxes.clear();
      return false;
    }
  }

  fDimension = aDimension;
  fProfile = aProfile;
  allocate();
  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::configure(int aDimension,
                                       const std::vector<std::vector<double> >& aEdges,
                                       bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // Clear :
  clear();
  // Some checks :
  if (aDimension <= 0) return false;
  fAxes.resize(aDimension);
  // Setup axes :
  for (int iaxis = 0; iaxis < aDimension; iaxis++) {
    if (!fAxes[iaxis].configure(aEdges[iaxis])) {
      fAxes.clear();
      return false;
    }
  }

  fDimension = aDimension;
  fProfile = aProfile;
  allocate();
  return true;
}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::allocate()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int iaxis;

  // Add two bins for the [under,out]flow data.
  int Binn = 1;
  for (iaxis = 0; iaxis < fDimension; iaxis++) Binn *= (fAxes[iaxis].bins() + 2);

  std::vector<double> empty;
  empty.resize(fDimension, 0);

  fBinEntries.resize(Binn, 0);
  fBinSw.resize(Binn, 0);
  fBinSw2.resize(Binn, 0);
  fBinSxw.resize(Binn, empty);
  fBinSx2w.resize(Binn, empty);

  fBinNumber = Binn;  // All bins : [in-range, underflow, outflow] bins.

  fAxes[0].fOffset = 1;
  for (iaxis = 1; iaxis < fDimension; iaxis++)
    fAxes[iaxis].fOffset = fAxes[iaxis - 1].fOffset * (fAxes[iaxis - 1].bins() + 2);

  if (fProfile) {
    fBinSvw.resize(Binn, 0);
    fBinSv2w.resize(Binn, 0);
  }

  fDoubles.resize(fDimension, 0);
  fInts.resize(fDimension, 0);
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::isValid() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (fDimension <= 0 ? false : true);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::HistogramData& mybhep::HCL::Histogram::getData()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::HistogramData& mybhep::HCL::Histogram::getData() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Histogram::getAxis(int aIndex) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[aIndex];
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::isCompatible(const Histogram& aHistogram) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fDimension != aHistogram.fDimension) return false;

  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    if (!fAxes[iaxis].isCompatible(aHistogram.fAxes[iaxis])) return false;
  }

  if (fProfile != aHistogram.fProfile) return false;

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::isProfile() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fProfile;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getDimension() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fDimension;
}
//////////////////////////////////////////////////////////////////////////////
const std::string& mybhep::HCL::Histogram::getTitle() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fTitle;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getBins() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fBinNumber;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getEntries() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int number = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      number += fBinEntries[ibin];
    }
  }
  return number;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getAllEntries() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int number = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    number += fBinEntries[ibin];
  }
  return number;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getSw() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  double sw = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sw += fBinSw[ibin];
    }
  }
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getSw2() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  double sw2 = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sw2 += fBinSw2[ibin];
    }
  }
  return sw2;
}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::updateFastGetters()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::baseReset()
//////////////////////////////////////////////////////////////////////////////
// Reset content (different of clear that deallocate all internal things).
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    fBinEntries[ibin] = 0;
    fBinSw[ibin] = 0;
    fBinSw2[ibin] = 0;
    for (int iaxis = 0; iaxis < fDimension; iaxis++) {
      fBinSxw[ibin][iaxis] = 0;
      fBinSx2w[ibin][iaxis] = 0;
    }
  }
  // Profile :
  if (fProfile) {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      fBinSvw[ibin] = 0;
      fBinSv2w[ibin] = 0;
    }
  }
  updateFastGetters();
}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::copy(const mybhep::HCL::Histogram& aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aFrom.fTitle;
  fDimension = aFrom.fDimension;
  fBinNumber = aFrom.fBinNumber;
  fMode = aFrom.fMode;
  // Arrays :
  fAxes = aFrom.fAxes;
  fBinEntries = aFrom.fBinEntries;
  fBinSw = aFrom.fBinSw;
  fBinSw2 = aFrom.fBinSw2;
  fBinSxw = aFrom.fBinSxw;
  fBinSx2w = aFrom.fBinSx2w;
  // Profile :
  fProfile = aFrom.fProfile;
  fBinSvw = aFrom.fBinSvw;
  fBinSv2w = aFrom.fBinSv2w;
  fCutV = aFrom.fCutV;
  fMinV = aFrom.fMinV;
  fMaxV = aFrom.fMaxV;
  // Args :
  fDoubles = aFrom.fDoubles;
  fInts = aFrom.fInts;

  updateFastGetters();
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::add(const Histogram& aHistogram)
//////////////////////////////////////////////////////////////////////////////
// The only histogram operation that makes sense.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!isCompatible(aHistogram)) return false;

  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    fBinEntries[ibin] += aHistogram.fBinEntries[ibin];
    fBinSw[ibin] += aHistogram.fBinSw[ibin];
    fBinSw2[ibin] += aHistogram.fBinSw2[ibin];
    for (int iaxis = 0; iaxis < fDimension; iaxis++) {
      fBinSxw[ibin][iaxis] += aHistogram.fBinSxw[ibin][iaxis];
      fBinSx2w[ibin][iaxis] += aHistogram.fBinSx2w[ibin][iaxis];
    }
  }

  if (fProfile) {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      fBinSvw[ibin] += aHistogram.fBinSvw[ibin];
      fBinSv2w[ibin] += aHistogram.fBinSv2w[ibin];
    }
  }

  updateFastGetters();

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::subtract(const Histogram& aHistogram)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!isCompatible(aHistogram)) return false;

  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    fBinEntries[ibin] -= aHistogram.fBinEntries[ibin];
    fBinSw[ibin] -= aHistogram.fBinSw[ibin];
    // Yes, it is a += in the below.
    fBinSw2[ibin] += aHistogram.fBinSw2[ibin];
    for (int iaxis = 0; iaxis < fDimension; iaxis++) {
      fBinSxw[ibin][iaxis] -= aHistogram.fBinSxw[ibin][iaxis];
      fBinSx2w[ibin][iaxis] -= aHistogram.fBinSx2w[ibin][iaxis];
    }
  }

  if (fProfile) {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      fBinSvw[ibin] -= aHistogram.fBinSvw[ibin];
      fBinSv2w[ibin] -= aHistogram.fBinSv2w[ibin];
    }
  }

  updateFastGetters();

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::multiply(const Histogram& aHistogram)
//////////////////////////////////////////////////////////////////////////////
// We build a new histo with one entry in each bean of weight :
//   this.w * aHistogram.w
// The current histo is overriden with this new histo.
// The fBinSw2 computation is consistent with FreeHEP and ROOT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!isCompatible(aHistogram)) return false;
  if (fDimension <= 0) return false;

  Histogram h(*this);
  h.baseReset();

  std::vector<int> is(fDimension);
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    getIndices(ibin, is);
    double swa = fBinSw[ibin];
    double swb = aHistogram.fBinSw[ibin];
    double sw2a = fBinSw2[ibin];
    double sw2b = aHistogram.fBinSw2[ibin];
    double sw = swa * swb;
    h.fBinEntries[ibin] = 1;
    h.fBinSw[ibin] = sw;
    h.fBinSw2[ibin] = sw2a * swb * swb + sw2b * swa * swa;
    for (int iaxis = 0; iaxis < fDimension; iaxis++) {
      double x = fAxes[iaxis].binCenter(is[iaxis]);
      h.fBinSxw[ibin][iaxis] = x * sw;
      h.fBinSx2w[ibin][iaxis] = x * x * sw;
    }
  }

  copy(h);

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::divide(const Histogram& aHistogram)
//////////////////////////////////////////////////////////////////////////////
// We build a new histo with one entry in each bean of weight :
//   this.w / aHistogram.w
// The current histo is overriden with this new histo.
// The fBinSw2 computation is consistent with FreeHEP and ROOT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!isCompatible(aHistogram)) return false;
  if (fDimension <= 0) return false;

  Histogram h(*this);
  h.baseReset();

  std::vector<int> is(fDimension);
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    getIndices(ibin, is);
    double swa = fBinSw[ibin];
    double swb = aHistogram.fBinSw[ibin];
    double sw2a = fBinSw2[ibin];
    double sw2b = aHistogram.fBinSw2[ibin];
    h.fBinEntries[ibin] = 1;
    if (swb != 0) {
      double sw = swa / swb;
      h.fBinSw[ibin] = sw;
      double swb2 = swb * swb;
      h.fBinSw2[ibin] = sw2a / swb2 + sw2b * swa * swa / (swb2 * swb2);
      for (int iaxis = 0; iaxis < fDimension; iaxis++) {
        double x = fAxes[iaxis].binCenter(is[iaxis]);
        h.fBinSxw[ibin][iaxis] = x * sw;
        h.fBinSx2w[ibin][iaxis] = x * x * sw;
      }
    }
  }

  copy(h);

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::multiply(double aFactor)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aFactor < 0) return false;
  double factor2 = aFactor * aFactor;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    fBinSw[ibin] *= aFactor;
    fBinSw2[ibin] *= factor2;
    for (int iaxis = 0; iaxis < fDimension; iaxis++) {
      fBinSxw[ibin][iaxis] *= aFactor;
      fBinSx2w[ibin][iaxis] *= aFactor;
    }
  }
  if (fProfile) {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      fBinSvw[ibin] *= aFactor;
    }
  }
  updateFastGetters();
  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////// Bins ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::fillBin(const std::vector<double>& aXs, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  int offset = getOffset(aXs);
  if (offset == NotFound) return false;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw;
  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    xw = aXs[iaxis] * aWeight;
    fBinSxw[offset][iaxis] += xw;
    fBinSx2w[offset][iaxis] += aXs[iaxis] * xw;
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram::fillBin(const std::vector<double>& aXs, double aV, double aWeight)
//////////////////////////////////////////////////////////////////////////////
// For profile.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (fCutV) {
    if ((aV < fMinV) || (aV >= fMaxV)) {
      return true;
    }
  }

  int offset = getOffset(aXs);
  if (offset == NotFound) return false;

  // Same as upper :
  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw;
  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    xw = aXs[iaxis] * aWeight;
    fBinSxw[offset][iaxis] += xw;
    fBinSx2w[offset][iaxis] += aXs[iaxis] * xw;
  }

  // Profile part :
  double vw = aV * aWeight;
  fBinSvw[offset] += vw;
  fBinSv2w[offset] += aV * vw;

  // fBinSxvw[offset][iaxis] += aXs[iaxis] * aV * aWeight;
  // fBinSx2vw[offset][iaxis] += aXs[iaxis] * aXs[iaxis] * aV * aWeight;

  return true;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getBinEntries(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int offset = getOffset(aIs);
  if (offset == NotFound) return 0;
  return fBinEntries[offset];
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getBinHeight(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int offset = getOffset(aIs);
  if (offset == NotFound) return 0;
  if (fProfile)
    return (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
  else
    return fBinSw[offset];
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getBinError(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int offset = getOffset(aIs);
  if (offset == NotFound) return 0;
  if (fProfile) {
    // FIXME Is it correct ?
    /* TProfile::GetBinError with kERRORMEAN mode does :
      Stat_t cont = fArray[bin];              //HCL Svw (see TProfile::Fill)
      Stat_t sum  = fBinEntries.fArray[bin];  //HCL Sw
      Stat_t err2 = fSumw2.fArray[bin];       //HCL Sv2w
      if (sum == 0) return 0;
      Stat_t eprim;
      Stat_t contsum = cont/sum;
      Stat_t eprim2  = TMath::Abs(err2/sum - contsum*contsum);
      eprim          = TMath::Sqrt(eprim2);
      ... ???
      if (fErrorMode == kERRORMEAN) return eprim/TMath::Sqrt(sum);
    */
    double sw = fBinSw[offset];  // ROOT sum
    if (sw == 0) return 0;
    double svw = fBinSvw[offset];                            // ROOT cont
    double sv2w = fBinSv2w[offset];                          // ROOT err2
    double mean = (svw / sw);                                // ROOT contsum
    double rms = ::sqrt(::fabs((sv2w / sw) - mean * mean));  // ROOT eprim
    // rms = getBinRMS.
    return rms / ::sqrt(sw);  // ROOT kERRORMEAN mode returned value
  } else {
    return ::sqrt(fBinSw2[offset]);
  }
}
//////////////////////////////////////////////////////////////////////////////
std::vector<double> mybhep::HCL::Histogram::getBinMean(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::vector<double> mean;
  mean.resize(fDimension, 0);
  if (fBinNumber == 0) return mean;
  int offset = getOffset(aIs);
  if (offset == NotFound) return mean;
  double sw = fBinSw[offset];
  if (sw == 0) return mean;
  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    mean[iaxis] = fBinSxw[offset][iaxis] / sw;
  }
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
std::vector<double> mybhep::HCL::Histogram::getBinStdDev(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::vector<double> rms;
  rms.resize(fDimension, 0);
  if (fBinNumber == 0) return rms;
  int offset = getOffset(aIs);
  if (offset == NotFound) return rms;
  double sw = fBinSw[offset];
  if (sw == 0) return rms;
  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    double sxw = fBinSxw[offset][iaxis];
    double sx2w = fBinSx2w[offset][iaxis];
    double mean = sxw / sw;
    rms[iaxis] = ::sqrt(::fabs((sx2w / sw) - mean * mean));
  }
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getBinRMS(const std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
// For profile histograms.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fProfile) return 0;
  if (fBinNumber == 0) return 0;
  int offset = getOffset(aIs);
  if (offset == NotFound) return 0;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double svw = fBinSvw[offset];
  double sv2w = fBinSv2w[offset];
  double mean = (svw / sw);
  return ::sqrt(::fabs((sv2w / sw) - mean * mean));
}
//////////////////////////////////////////////////////////////////////////////
/////// Axes /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getIthAxisSxw(int aAxis) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  double sxw = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sxw += fBinSxw[ibin][aAxis];
    }
  }
  return sxw;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getIthAxisSx2w(int aAxis) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  double sx2w = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sx2w += fBinSx2w[ibin][aAxis];
    }
  }
  return sx2w;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getIthAxisMean(int aAxis) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  double sw = 0;
  double sxw = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sw += fBinSw[ibin];
      sxw += fBinSxw[ibin][aAxis];
    }
  }
  if (sw == 0) return 0;
  return sxw / sw;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getIthAxisRMS(int aAxis) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  double sw = 0;
  double sxw = 0;
  double sx2w = 0;
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    if (!isOut(ibin)) {
      sw += fBinSw[ibin];
      sxw += fBinSxw[ibin][aAxis];
      sx2w += fBinSx2w[ibin][aAxis];
    }
  }
  if (sw == 0) return 0;
  double mean = sxw / sw;
  return ::sqrt(::fabs((sx2w / sw) - mean * mean));
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getIthAxisBinEntries(int aAxis, int aIndex) const
//////////////////////////////////////////////////////////////////////////////
// WARNING : slow.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  int entries = 0;
  std::vector<int> is(fDimension);
  for (int ibin = 0; ibin < fBinNumber; ibin++) {
    getIndices(ibin, is);
    if (is[aAxis] == aIndex) {
      entries += fBinEntries[ibin];
    }
  }
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Histogram::getIthAxisBinHeight(int aAxis, int aIndex) const
//////////////////////////////////////////////////////////////////////////////
// WARNING : slow.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if ((aAxis < 0) || (aAxis >= fDimension)) return 0;
  double sw = 0;
  std::vector<int> is(fDimension);
  if (fProfile) {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      getIndices(ibin, is);
      if (is[aAxis] == aIndex) {
        sw += (fBinSw[ibin] ? (fBinSvw[ibin] / fBinSw[ibin]) : 0);
      }
    }
  } else {
    for (int ibin = 0; ibin < fBinNumber; ibin++) {
      getIndices(ibin, is);
      if (is[aAxis] == aIndex) {
        sw += fBinSw[ibin];
      }
    }
  }
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::clear()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // HistogramData :
  fAxes.clear();
  fBinEntries.clear();
  fBinSw.clear();
  fBinSw2.clear();
  fBinSxw.clear();
  fBinSx2w.clear();
  fBinNumber = 0;
  fDimension = 0;
  // Profile :
  fProfile = false;
  fBinSvw.clear();
  fBinSv2w.clear();
  fCutV = false;
  fMinV = 0;
  fMaxV = 0;
  // Args :
  fDoubles.clear();
  fInts.clear();
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Histogram::getOffset(const std::vector<double>& aXs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fDimension <= 0) return NotFound;
  int offset = 0;
  for (int iaxis = 0; iaxis < fDimension; iaxis++) {
    int ibin = fAxes[iaxis].coordToAbsoluteIndex(aXs[iaxis]);
    if (ibin == NotFound) return NotFound;
    offset += ibin * fAxes[iaxis].fOffset;
  }
  return offset;
}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Histogram::getIndices(int aOffset, std::vector<int>& aIs) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int offset = aOffset;
  int iaxis;
  for (iaxis = fDimension - 1; iaxis >= 0; iaxis--) {
    aIs[iaxis] = offset / fAxes[iaxis].fOffset;
    offset -= aIs[iaxis] * fAxes[iaxis].fOffset;
  }
  for (iaxis = 0; iaxis < fDimension; iaxis++) {
    if (aIs[iaxis] == 0) {
      aIs[iaxis] = Axis::UNDERFLOW_BIN;
    } else if (aIs[iaxis] == fAxes[iaxis].fNumberOfBins + 1) {
      aIs[iaxis] = Axis::OVERFLOW_BIN;
    } else {
      aIs[iaxis]--;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define HISTOGRAM_SOURCE(aClass)                                    \
  const std::string& aClass::title() const { return fTitle; }       \
  bool aClass::setTitle(const std::string& aTitle) {                \
    fTitle = aTitle;                                                \
    return true;                                                    \
  }                                                                 \
  int aClass::dimension() const { return fDimension; }              \
  int aClass::entries() const { return getEntries(); }              \
  int aClass::allEntries() const {                                  \
    int number = 0;                                                 \
    for (int ibin = 0; ibin < fBinNumber; ibin++) {                 \
      number += fBinEntries[ibin];                                  \
    }                                                               \
    return number;                                                  \
  }                                                                 \
  int aClass::extraEntries() const {                                \
    int number = 0;                                                 \
    for (int ibin = 0; ibin < fBinNumber; ibin++) {                 \
      if (isOut(ibin)) {                                            \
        number += fBinEntries[ibin];                                \
      }                                                             \
    }                                                               \
    return number;                                                  \
  }                                                                 \
  double aClass::equivalentBinEntries() const {                     \
    double sw = 0;                                                  \
    double sw2 = 0;                                                 \
    for (int ibin = 0; ibin < fBinNumber; ibin++) {                 \
      if (!isOut(ibin)) {                                           \
        sw += fBinSw[ibin];                                         \
        sw2 += fBinSw2[ibin];                                       \
      }                                                             \
    }                                                               \
    if (sw2 == 0) return 0;                                         \
    return (sw * sw) / sw2;                                         \
  }                                                                 \
  double aClass::sumBinHeights() const {                            \
    if (fProfile) {                                                 \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        if (!isOut(ibin)) {                                         \
          if (fBinSw[ibin] == 0) break;                             \
          sw += (fBinSvw[ibin] / fBinSw[ibin]);                     \
        }                                                           \
      }                                                             \
      return sw;                                                    \
    } else {                                                        \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        if (!isOut(ibin)) {                                         \
          sw += fBinSw[ibin];                                       \
        }                                                           \
      }                                                             \
      return sw;                                                    \
    }                                                               \
  }                                                                 \
  double aClass::sumAllBinHeights() const {                         \
    if (fProfile) {                                                 \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        if (fBinSw[ibin] == 0) break;                               \
        sw += (fBinSvw[ibin] / fBinSw[ibin]);                       \
      }                                                             \
      return sw;                                                    \
    } else {                                                        \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        sw += fBinSw[ibin];                                         \
      }                                                             \
      return sw;                                                    \
    }                                                               \
  }                                                                 \
  double aClass::sumExtraBinHeights() const {                       \
    if (fProfile) {                                                 \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        if (isOut(ibin)) {                                          \
          if (fBinSw[ibin] == 0) break;                             \
          sw += (fBinSvw[ibin] / fBinSw[ibin]);                     \
        }                                                           \
      }                                                             \
      return sw;                                                    \
    } else {                                                        \
      double sw = 0;                                                \
      for (int ibin = 0; ibin < fBinNumber; ibin++) {               \
        if (isOut(ibin)) {                                          \
          sw += fBinSw[ibin];                                       \
        }                                                           \
      }                                                             \
      return sw;                                                    \
    }                                                               \
  }                                                                 \
  double aClass::minBinHeight() const {                             \
    double value = 0;                                               \
    bool first = true;                                              \
    for (int ibin = 0; ibin < fBinNumber; ibin++) {                 \
      if (!isOut(ibin)) {                                           \
        double vbin;                                                \
        if (fProfile) {                                             \
          vbin = (fBinSw[ibin] ? fBinSvw[ibin] / fBinSw[ibin] : 0); \
        } else {                                                    \
          vbin = fBinSw[ibin];                                      \
        }                                                           \
        if (first) {                                                \
          first = false;                                            \
          value = vbin;                                             \
        } else {                                                    \
          if (vbin <= value) value = vbin;                          \
        }                                                           \
      }                                                             \
    }                                                               \
    return value;                                                   \
  }                                                                 \
  double aClass::maxBinHeight() const {                             \
    double value = 0;                                               \
    bool first = true;                                              \
    for (int ibin = 0; ibin < fBinNumber; ibin++) {                 \
      if (!isOut(ibin)) {                                           \
        double vbin;                                                \
        if (fProfile) {                                             \
          vbin = (fBinSw[ibin] ? fBinSvw[ibin] / fBinSw[ibin] : 0); \
        } else {                                                    \
          vbin = fBinSw[ibin];                                      \
        }                                                           \
        if (first) {                                                \
          first = false;                                            \
          value = vbin;                                             \
        } else {                                                    \
          if (vbin >= value) value = vbin;                          \
        }                                                           \
      }                                                             \
    }                                                               \
    return value;                                                   \
  }                                                                 \
  bool aClass::reset() {                                            \
    baseReset();                                                    \
    return true;                                                    \
  }                                                                 \
  bool aClass::scale(double aFactor) { return multiply(aFactor); }
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HISTOGRAM_SOURCE(mybhep::HCL::Base1D)
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D::Base1D(const std::string& aString, int aXnumber, double aXmin, double aXmax,
                            bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<int> ns;
  ns.push_back(aXnumber);
  std::vector<double> mins;
  mins.push_back(aXmin);
  std::vector<double> maxs;
  maxs.push_back(aXmax);
  configure(1, ns, mins, maxs, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D::Base1D(const std::string& aString, const std::vector<double>& aEdges,
                            bool aProfile)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<std::vector<double> > edges(1);
  edges[0] = aEdges;
  configure(1, edges, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D::Base1D(const Base1D& aFrom)
    //:IBase(aFrom)
    //,IBase1D(aFrom)
    : Histogram(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D::~Base1D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::mean() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return getIthAxisMean(AxisX);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::rms() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return getIthAxisRMS(AxisX);
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base1D::binEntries(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int offset = ibin;
  int entries = fBinEntries[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base1D*)this)->fInts[0] = aI;
    int entriesNotOpt = getBinEntries(fInts);
    if (entries != entriesNotOpt)
      printf("debug : mybhep::HCL::Base1D::binEntries : anomaly %d %d\n", entries, entriesNotOpt);
  }
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::binHeight(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int offset = ibin;
  double height = 0;
  if (fProfile)
    height = (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
  else
    height = fBinSw[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base1D*)this)->fInts[0] = aI;
    double heightNotOpt = getBinHeight(fInts);
    if (!IS_EQUAL(height, heightNotOpt))
      printf("debug : mybhep::HCL::Base1D::binHeight : anomaly\n");
  }
#endif
  return height;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::binError(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ((Base1D*)this)->fInts[0] = aI;
  return getBinError(fInts);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::binCenter(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0].binCenter(aI);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::binMean(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int offset = ibin;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisX] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base1D*)this)->fInts[0] = aI;
    double meanNotOpt = getBinMean(fInts)[0];
    if (!IS_EQUAL(mean, meanNotOpt)) printf("debug : mybhep::HCL::Base1D::binMean : anomaly\n");
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base1D::binStdDev(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int offset = ibin;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisX];
  double sx2w = fBinSx2w[offset][AxisX];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base1D*)this)->fInts[0] = aI;
    double rmsNotOpt = getBinStdDev(fInts)[0];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base1D::binStdDev : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base1D::axis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0];
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base1D::coordToIndex(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return axis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram1D::Histogram1D(const std::string& aString, int aXnumber, double aXmin,
                                      double aXmax)
    : Base1D(aString, aXnumber, aXmin, aXmax)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram1D::Histogram1D(const std::string& aString, const std::vector<double>& aEdges)
    : Base1D(aString, aEdges)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram1D::Histogram1D(const Histogram1D& aFrom)
    //:IBase(aFrom)
    //,IBase1D(aFrom)
    //,IHistogram1D(aFrom)
    : Base1D(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram1D::~Histogram1D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram1D::fill(double aX, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // fDoubles[0] = aX;
  // return fillBin(fDoubles,aWeight);

  if (fDimension <= 0) return false;

  int ibin = fAxes[0].coordToAbsoluteIndex(aX);
  if (ibin == NotFound) return false;

  int offset = ibin;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw = aX * aWeight;
  double x2w = aX * xw;
  fBinSxw[offset][0] += xw;
  fBinSx2w[offset][0] += x2w;

  /*
    bool inRange = true;
    if(ibin==0) inRange = false;
    else if(ibin==(fAxes[0].fNumberOfBins+1)) inRange = false;

    if(inRange) {
      fInRangeEntries++;
      fInRangeSw += aWeight;

      fInRangeSxw += xw;
      fInRangeSx2w += x2w;
    }
  */

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::Profile1D(const std::string& aString, int aXnumber, double aXmin,
                                  double aXmax)
    : Base1D(aString, aXnumber, aXmin, aXmax, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::Profile1D(const std::string& aString, int aXnumber, double aXmin,
                                  double aXmax, double aVmin, double aVmax)
    : Base1D(aString, aXnumber, aXmin, aXmax, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCutV = true;
  fMinV = aVmin;
  fMaxV = aVmax;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::Profile1D(const std::string& aString, const std::vector<double>& aEdges)
    : Base1D(aString, aEdges, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::Profile1D(const std::string& aString, const std::vector<double>& aEdges,
                                  double aVmin, double aVmax)
    : Base1D(aString, aEdges, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCutV = true;
  fMinV = aVmin;
  fMaxV = aVmax;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::Profile1D(const Profile1D& aFrom)
    //:IBase(aFrom)
    //,IBase1D(aFrom)
    //,IProfile1D(aFrom)
    : Base1D(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile1D::~Profile1D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Profile1D::fill(double aX, double aV, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // fDoubles[0] = aX;
  // return fillBin(fDoubles,aV,aWeight);

  if (!fDimension) return false;
  if (fCutV) {
    if ((aV < fMinV) || (aV >= fMaxV)) {
      return true;
    }
  }

  int ibin = fAxes[0].coordToAbsoluteIndex(aX);
  if (ibin == NotFound) return false;

  int offset = ibin;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw = aX * aWeight;
  double x2w = aX * xw;
  fBinSxw[offset][0] += xw;
  fBinSx2w[offset][0] += x2w;

  // Profile part :
  double vw = aV * aWeight;
  fBinSvw[offset] += vw;
  fBinSv2w[offset] += aV * vw;

  return true;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Profile1D::binRms(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ((Profile1D*)this)->fInts[0] = aI;
  return getBinRMS(fInts);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HISTOGRAM_SOURCE(mybhep::HCL::Base2D)
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D::Base2D(const std::string& aString, int aXnumber, double aXmin, double aXmax,
                            int aYnumber, double aYmin, double aYmax, bool aProfile)
    : fInRangeEntries(0),
      fInRangeSw(0),
      fInRangeSxw(0),
      fInRangeSyw(0),
      fInRangeSx2w(0),
      fInRangeSy2w(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<int> ns;
  ns.push_back(aXnumber);
  ns.push_back(aYnumber);
  std::vector<double> mins;
  mins.push_back(aXmin);
  mins.push_back(aYmin);
  std::vector<double> maxs;
  maxs.push_back(aXmax);
  maxs.push_back(aYmax);
  configure(2, ns, mins, maxs, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D::Base2D(const std::string& aString, const std::vector<double>& aEdgesX,
                            const std::vector<double>& aEdgesY, bool aProfile)
    : fInRangeEntries(0),
      fInRangeSw(0),
      fInRangeSxw(0),
      fInRangeSyw(0),
      fInRangeSx2w(0),
      fInRangeSy2w(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<std::vector<double> > edges(2);
  edges[0] = aEdgesX;
  edges[1] = aEdgesY;
  configure(2, edges, aProfile);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D::Base2D(const Base2D& aFrom)
    //:IBase(aFrom)
    //,IBase2D(aFrom)
    : Histogram(aFrom),
      fInRangeEntries(aFrom.fInRangeEntries),
      fInRangeSw(aFrom.fInRangeSw),
      fInRangeSxw(aFrom.fInRangeSxw),
      fInRangeSyw(aFrom.fInRangeSyw),
      fInRangeSx2w(aFrom.fInRangeSx2w),
      fInRangeSy2w(aFrom.fInRangeSy2w)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D::~Base2D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Base2D::updateFastGetters()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fInRangeEntries = 0;
  fInRangeSw = 0;
  fInRangeSxw = 0;
  fInRangeSyw = 0;
  fInRangeSx2w = 0;
  fInRangeSy2w = 0;
  int ibin, jbin, offset;
  int xbins = fAxes[0].bins();
  int ybins = fAxes[1].bins();
  int yoffset = fAxes[1].fOffset;
  for (ibin = 1; ibin <= xbins; ibin++) {
    offset = ibin + yoffset;
    for (jbin = 1; jbin <= ybins; jbin++) {
      // offset = ibin + jbin * fAxes[1].fOffset;

      fInRangeEntries += fBinEntries[offset];
      fInRangeSw += fBinSw[offset];
      fInRangeSxw += fBinSxw[offset][0];
      fInRangeSyw += fBinSxw[offset][1];
      fInRangeSx2w += fBinSx2w[offset][0];
      fInRangeSy2w += fBinSx2w[offset][1];

      offset += yoffset;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::meanX() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSxw / fInRangeSw;
#ifdef HCL_CHECK_OPTIMIZE
  double meanNotOpt = getIthAxisMean(AxisX);
  if (!IS_EQUAL(mean, meanNotOpt))
    printf("debug : mybhep::HCL::Base2D::meanX : anomaly %g %g\n", mean, meanNotOpt);
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::meanY() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSyw / fInRangeSw;
#ifdef HCL_CHECK_OPTIMIZE
  double meanNotOpt = getIthAxisMean(AxisY);
  if (!IS_EQUAL(mean, meanNotOpt))
    printf("debug : mybhep::HCL::Base2D::meanY : anomaly %g %g\n", mean, meanNotOpt);
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::rmsX() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSxw / fInRangeSw;
  double rms = ::sqrt(::fabs((fInRangeSx2w / fInRangeSw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  double rmsNotOpt = getIthAxisRMS(AxisX);
  if (!IS_EQUAL(rms, rmsNotOpt))
    printf("debug : mybhep::HCL::Base2D::rmsX : anomaly %g %g\n", rms, rmsNotOpt);
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::rmsY() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSyw / fInRangeSw;
  double rms = ::sqrt(::fabs((fInRangeSy2w / fInRangeSw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  double rmsNotOpt = getIthAxisRMS(AxisY);
  if (!IS_EQUAL(rms, rmsNotOpt))
    printf("debug : mybhep::HCL::Base2D::rmsY : anomaly %g %g\n", rms, rmsNotOpt);
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base2D::binEntries(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  int entries = fBinEntries[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    int entriesNotOpt = getBinEntries(fInts);
    if (entries != entriesNotOpt)
      printf("debug : mybhep::HCL::Base2D::binEntries : anomaly %d %d\n", entries, entriesNotOpt);
  }
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binHeight(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  double height = 0;
  if (fProfile)
    height = (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
  else
    height = fBinSw[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    double heightNotOpt = getBinHeight(fInts);
    if (!IS_EQUAL(height, heightNotOpt))
      printf("debug : mybhep::HCL::Base2D::binHeight : anomaly\n");
  }
#endif
  return height;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binError(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ((Base2D*)this)->fInts[0] = aI;
  ((Base2D*)this)->fInts[1] = aJ;
  return getBinError(fInts);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binCenterX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0].binCenter(aI);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binCenterY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[1].binCenter(aJ);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binMeanX(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisX] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    double meanNotOpt = getBinMean(fInts)[0];
    if (!IS_EQUAL(mean, meanNotOpt)) printf("debug : mybhep::HCL::Base2D::binMeanX : anomaly\n");
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binMeanY(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisY] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    double meanNotOpt = getBinMean(fInts)[1];
    if (!IS_EQUAL(mean, meanNotOpt)) printf("debug : mybhep::HCL::Base2D::binMeanY : anomaly\n");
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binStdDevX(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisX];
  double sx2w = fBinSx2w[offset][AxisX];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    double rmsNotOpt = getBinStdDev(fInts)[0];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base2D::binStdDevX : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binStdDevY(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisY];
  double sx2w = fBinSx2w[offset][AxisY];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base2D*)this)->fInts[0] = aI;
    ((Base2D*)this)->fInts[1] = aJ;
    double rmsNotOpt = getBinStdDev(fInts)[1];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base2D::binStdDevY : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base2D::xAxis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0];
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base2D::yAxis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[1];
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base2D::coordToIndexX(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return xAxis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base2D::coordToIndexY(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return yAxis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D* mybhep::HCL::Base2D::sliceX(int aJbeg, int aJend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  mybhep::HCL::Base1D* sliceX =
      new Base1D("SliceX", xAxis().bins(), xAxis().lowerEdge(), xAxis().upperEdge());
  if (!fillSliceX(*sliceX, aJbeg, aJend)) {
    delete sliceX;
    return 0;
  }
  return sliceX;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Base2D::fillSliceX(mybhep::HCL::Base1D& aHisto, int aJbeg, int aJend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (!fAxes[1].inRangetoAbsoluteIndex(aJbeg)) return false;
  if (!fAxes[1].inRangetoAbsoluteIndex(aJend)) return false;
  if (aJbeg > aJend) return false;

  HistogramData& hdata = aHisto.getData();
  if (xAxis().bins() != aHisto.axis().bins()) return false;

  int aibin, aoffset, jbin, offset;
  int yoffset = fAxes[1].fOffset;

  // Fill also the outflow.
  int abins = hdata.fAxes[0].bins() + 2;
  for (aibin = 0; aibin < abins; aibin++) {
    // offset1D = ibin
    aoffset = aibin;
    for (jbin = aJbeg; jbin <= aJend; jbin++) {
      // offset2D = ibin + jbin * yoffset
      // hdata booked with x then :
      offset = aibin + jbin * yoffset;
      // Bin :
      hdata.fBinEntries[aoffset] += fBinEntries[offset];
      hdata.fBinSw[aoffset] += fBinSw[offset];
      hdata.fBinSw2[aoffset] += fBinSw2[offset];
      hdata.fBinSxw[aoffset][0] += fBinSxw[offset][0];
      hdata.fBinSx2w[aoffset][0] += fBinSx2w[offset][0];
      // FIXME hdata.fBinSvw[aoffset] += fBinSvw[offset];
    }
  }
  return true;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D* mybhep::HCL::Base2D::sliceY(int aIbeg, int aIend) const
//////////////////////////////////////////////////////////////////////////////
// Should be implemented by a managed histo class.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  mybhep::HCL::Base1D* sliceY =
      new Base1D("SliceY", yAxis().bins(), yAxis().lowerEdge(), yAxis().upperEdge());
  if (!fillSliceY(*sliceY, aIbeg, aIend)) {
    delete sliceY;
    return 0;
  }
  return sliceY;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Base2D::fillSliceY(mybhep::HCL::Base1D& aHisto, int aIbeg, int aIend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (!fAxes[0].inRangetoAbsoluteIndex(aIbeg)) return false;
  if (!fAxes[0].inRangetoAbsoluteIndex(aIend)) return false;
  if (aIbeg > aIend) return false;

  HistogramData& hdata = aHisto.getData();
  if (yAxis().bins() != aHisto.axis().bins()) return false;

  int aibin, aoffset, ibin, offset;
  int yoffset = fAxes[1].fOffset;

  // Fill also the outflow.
  int abins = hdata.fAxes[0].bins() + 2;
  for (aibin = 0; aibin < abins; aibin++) {
    // offset1D = ibin
    aoffset = aibin;
    for (ibin = aIbeg; ibin <= aIend; ibin++) {
      // offset2D = ibin + jbin * yoffset
      // hdata booked with y then :
      offset = ibin + aibin * yoffset;
      // Bin :
      hdata.fBinEntries[aoffset] += fBinEntries[offset];
      hdata.fBinSw[aoffset] += fBinSw[offset];
      hdata.fBinSw2[aoffset] += fBinSw2[offset];
      hdata.fBinSxw[aoffset][0] += fBinSxw[offset][1];
      hdata.fBinSx2w[aoffset][0] += fBinSx2w[offset][1];
      // FIXME hdata.fBinSvw[aoffset] += fBinSvw[offset];
    }
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base2D::binEntriesX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int entries = 0;
  int jbin, offset;
  int ybins = fAxes[1].bins() + 2;
  for (jbin = 0; jbin < ybins; jbin++) {
    offset = ibin + jbin * fAxes[1].fOffset;
    entries += fBinEntries[offset];
  }
#ifdef HCL_CHECK_OPTIMIZE
  if (entries != getIthAxisBinEntries(0, aI))
    printf("debug : binEntriesX : anomaly at %d : %d\n", aI, entries);
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binHeightX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  double sw = 0;
  int jbin, offset;
  int ybins = fAxes[1].bins() + 2;
  if (fProfile) {
    for (jbin = 0; jbin < ybins; jbin++) {
      offset = ibin + jbin * fAxes[1].fOffset;
      sw += (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
    }
  } else {
    for (jbin = 0; jbin < ybins; jbin++) {
      offset = ibin + jbin * fAxes[1].fOffset;
      sw += fBinSw[offset];
    }
  }
#ifdef HCL_CHECK_OPTIMIZE
  double swNotOpt = getIthAxisBinHeight(0, aI);
  if (!IS_EQUAL(sw, swNotOpt)) printf("debug : binHeightX : anomaly at %d : %g\n", aI, sw);
#endif
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base2D::binEntriesY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int entries = 0;
  int ibin, offset;
  int xbins = fAxes[0].bins() + 2;
  for (ibin = 0; ibin < xbins; ibin++) {
    offset = ibin + jbin * fAxes[1].fOffset;
    entries += fBinEntries[offset];
  }
#ifdef HCL_CHECK_OPTIMIZE
  if (entries != getIthAxisBinEntries(1, aJ))
    printf("debug : binEntriesY : anomaly at %d : %d\n", aJ, entries);
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base2D::binHeightY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  double sw = 0;
  int ibin, offset;
  int xbins = fAxes[0].bins() + 2;
  if (fProfile) {
    for (ibin = 0; ibin < xbins; ibin++) {
      offset = ibin + jbin * fAxes[1].fOffset;
      sw += (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
    }
  } else {
    for (ibin = 0; ibin < xbins; ibin++) {
      offset = ibin + jbin * fAxes[1].fOffset;
      sw += fBinSw[offset];
    }
  }
#ifdef HCL_CHECK_OPTIMIZE
  double swNotOpt = getIthAxisBinHeight(1, aJ);
  if (!IS_EQUAL(sw, swNotOpt)) printf("debug : binHeightY : anomaly at %d : %g\n", aJ, sw);
#endif
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D* mybhep::HCL::Base2D::projectionX() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return sliceX(Axis::UNDERFLOW_BIN, Axis::OVERFLOW_BIN);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base1D* mybhep::HCL::Base2D::projectionY() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return sliceY(Axis::UNDERFLOW_BIN, Axis::OVERFLOW_BIN);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram2D::Histogram2D(const std::string& aString, int aXnumber, double aXmin,
                                      double aXmax, int aYnumber, double aYmin, double aYmax)
    : Base2D(aString, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram2D::Histogram2D(const std::string& aString,
                                      const std::vector<double>& aEdgesX,
                                      const std::vector<double>& aEdgesY)
    : Base2D(aString, aEdgesX, aEdgesY)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram2D::Histogram2D(const Histogram2D& aFrom)
    //:IBase(aFrom)
    //,IBase2D(aFrom)
    //,IHistogram2D(aFrom)
    : Base2D(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram2D::~Histogram2D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram2D::fill(double aX, double aY, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // fDoubles[0] = aX;
  // fDoubles[1] = aY;
  // return fillBin(fDoubles,aWeight);

  if (fDimension <= 0) return false;

  int ibin = fAxes[0].coordToAbsoluteIndex(aX);
  if (ibin == NotFound) return false;
  int jbin = fAxes[1].coordToAbsoluteIndex(aY);
  if (jbin == NotFound) return false;

  int offset = ibin + jbin * fAxes[1].fOffset;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw = aX * aWeight;
  double x2w = aX * xw;
  fBinSxw[offset][0] += xw;
  fBinSx2w[offset][0] += x2w;

  double yw = aY * aWeight;
  double y2w = aY * yw;
  fBinSxw[offset][1] += yw;
  fBinSx2w[offset][1] += y2w;

  bool inRange = true;
  if (ibin == 0)
    inRange = false;
  else if (ibin == (fAxes[0].fNumberOfBins + 1))
    inRange = false;

  if (jbin == 0)
    inRange = false;
  else if (jbin == (fAxes[1].fNumberOfBins + 1))
    inRange = false;

  if (inRange) {
    fInRangeEntries++;
    fInRangeSw += aWeight;

    fInRangeSxw += xw;
    fInRangeSx2w += x2w;

    fInRangeSyw += yw;
    fInRangeSy2w += y2w;
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::Profile2D(const std::string& aString, int aXnumber, double aXmin,
                                  double aXmax, int aYnumber, double aYmin, double aYmax)
    : Base2D(aString, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::Profile2D(const std::string& aString, int aXnumber, double aXmin,
                                  double aXmax, int aYnumber, double aYmin, double aYmax,
                                  double aVmin, double aVmax)
    : Base2D(aString, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCutV = true;
  fMinV = aVmin;
  fMaxV = aVmax;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::Profile2D(const std::string& aString, const std::vector<double>& aEdgesX,
                                  const std::vector<double>& aEdgesY)
    : Base2D(aString, aEdgesX, aEdgesY, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::Profile2D(const std::string& aString, const std::vector<double>& aEdgesX,
                                  const std::vector<double>& aEdgesY, double aVmin, double aVmax)
    : Base2D(aString, aEdgesX, aEdgesY, true)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCutV = true;
  fMinV = aVmin;
  fMaxV = aVmax;
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::Profile2D(const Profile2D& aFrom)
    //:IBase(aFrom)
    //,IBase2D(aFrom)
    //,IProfile2D(aFrom)
    : Base2D(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Profile2D::~Profile2D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Profile2D::fill(double aX, double aY, double aV, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // fDoubles[0] = aX;
  // fDoubles[1] = aY;
  // return fillBin(fDoubles,aV,aWeight);

  if (fCutV) {
    if ((aV < fMinV) || (aV >= fMaxV)) {
      return true;
    }
  }

  if (fDimension <= 0) return false;

  int ibin = fAxes[0].coordToAbsoluteIndex(aX);
  if (ibin == NotFound) return false;
  int jbin = fAxes[1].coordToAbsoluteIndex(aY);
  if (jbin == NotFound) return false;

  int offset = ibin + jbin * fAxes[1].fOffset;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw = aX * aWeight;
  double x2w = aX * xw;
  fBinSxw[offset][0] += xw;
  fBinSx2w[offset][0] += x2w;

  double yw = aY * aWeight;
  double y2w = aY * yw;
  fBinSxw[offset][1] += yw;
  fBinSx2w[offset][1] += y2w;

  bool inRange = true;
  if (ibin == 0)
    inRange = false;
  else if (ibin == (fAxes[0].fNumberOfBins + 1))
    inRange = false;

  if (jbin == 0)
    inRange = false;
  else if (jbin == (fAxes[1].fNumberOfBins + 1))
    inRange = false;

  if (inRange) {
    fInRangeEntries++;
    fInRangeSw += aWeight;

    fInRangeSxw += xw;
    fInRangeSx2w += x2w;

    fInRangeSyw += yw;
    fInRangeSy2w += y2w;
  }

  // Profile part :
  double vw = aV * aWeight;
  fBinSvw[offset] += vw;
  fBinSv2w[offset] += aV * vw;

  return true;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Profile2D::binRms(int aI, int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ((Profile2D*)this)->fInts[0] = aI;
  ((Profile2D*)this)->fInts[1] = aJ;
  return getBinRMS(fInts);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HISTOGRAM_SOURCE(mybhep::HCL::Base3D)
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base3D::Base3D(const std::string& aString, int aXnumber, double aXmin, double aXmax,
                            int aYnumber, double aYmin, double aYmax, int aZnumber, double aZmin,
                            double aZmax)
    : fInRangeEntries(0),
      fInRangeSw(0),
      fInRangeSxw(0),
      fInRangeSyw(0),
      fInRangeSzw(0),
      fInRangeSx2w(0),
      fInRangeSy2w(0),
      fInRangeSz2w(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<int> ns;
  ns.push_back(aXnumber);
  ns.push_back(aYnumber);
  ns.push_back(aZnumber);
  std::vector<double> mins;
  mins.push_back(aXmin);
  mins.push_back(aYmin);
  mins.push_back(aZmin);
  std::vector<double> maxs;
  maxs.push_back(aXmax);
  maxs.push_back(aYmax);
  maxs.push_back(aZmax);
  configure(3, ns, mins, maxs);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base3D::Base3D(const std::string& aString, const std::vector<double>& aEdgesX,
                            const std::vector<double>& aEdgesY, const std::vector<double>& aEdgesZ)
    : fInRangeEntries(0),
      fInRangeSw(0),
      fInRangeSxw(0),
      fInRangeSyw(0),
      fInRangeSzw(0),
      fInRangeSx2w(0),
      fInRangeSy2w(0),
      fInRangeSz2w(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTitle = aString;
  std::vector<std::vector<double> > edges(3);
  edges[0] = aEdgesX;
  edges[1] = aEdgesY;
  edges[2] = aEdgesZ;
  configure(3, edges);
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base3D::Base3D(const Base3D& aFrom)
    //:IBase(aFrom)
    //,IBase3D(aFrom)
    : Histogram(aFrom),
      fInRangeEntries(aFrom.fInRangeEntries),
      fInRangeSw(aFrom.fInRangeSw),
      fInRangeSxw(aFrom.fInRangeSxw),
      fInRangeSyw(aFrom.fInRangeSyw),
      fInRangeSzw(aFrom.fInRangeSzw),
      fInRangeSx2w(aFrom.fInRangeSx2w),
      fInRangeSy2w(aFrom.fInRangeSy2w),
      fInRangeSz2w(aFrom.fInRangeSz2w)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base3D::~Base3D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
void mybhep::HCL::Base3D::updateFastGetters()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fInRangeEntries = 0;
  fInRangeSw = 0;
  fInRangeSxw = 0;
  fInRangeSyw = 0;
  fInRangeSzw = 0;
  fInRangeSx2w = 0;
  fInRangeSy2w = 0;
  fInRangeSz2w = 0;
  int ibin, jbin, kbin, joffset, offset;
  int xbins = fAxes[0].bins();
  int ybins = fAxes[1].bins();
  int zbins = fAxes[2].bins();
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  for (ibin = 1; ibin <= xbins; ibin++) {
    joffset = ibin + yoffset;
    for (jbin = 1; jbin <= ybins; jbin++) {
      // joffset = ibin + jbin * fAxes[1].fOffset;
      offset = joffset + zoffset;
      for (kbin = 1; kbin <= zbins; kbin++) {
        // offset = joffset + kbin * fAxes[2].fOffset;

        fInRangeEntries += fBinEntries[offset];
        fInRangeSw += fBinSw[offset];
        fInRangeSxw += fBinSxw[offset][0];
        fInRangeSyw += fBinSxw[offset][1];
        fInRangeSzw += fBinSxw[offset][2];
        fInRangeSx2w += fBinSx2w[offset][0];
        fInRangeSy2w += fBinSx2w[offset][1];
        fInRangeSz2w += fBinSx2w[offset][2];

        offset += zoffset;
      }
      joffset += yoffset;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::meanX() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSxw / fInRangeSw;
#ifdef HCL_CHECK_OPTIMIZE
  double meanNotOpt = getIthAxisMean(AxisX);
  if (!IS_EQUAL(mean, meanNotOpt))
    printf("debug : mybhep::HCL::Base3D::meanX : anomaly %g %g\n", mean, meanNotOpt);
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::meanY() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSyw / fInRangeSw;
#ifdef HCL_CHECK_OPTIMIZE
  double meanNotOpt = getIthAxisMean(AxisY);
  if (!IS_EQUAL(mean, meanNotOpt))
    printf("debug : mybhep::HCL::Base3D::meanY : anomaly %g %g\n", mean, meanNotOpt);
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::meanZ() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSzw / fInRangeSw;
#ifdef HCL_CHECK_OPTIMIZE
  double meanNotOpt = getIthAxisMean(AxisZ);
  if (!IS_EQUAL(mean, meanNotOpt))
    printf("debug : mybhep::HCL::Base3D::meanZ : anomaly %g %g\n", mean, meanNotOpt);
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::rmsX() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSxw / fInRangeSw;
  double rms = ::sqrt(::fabs((fInRangeSx2w / fInRangeSw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  double rmsNotOpt = getIthAxisRMS(AxisX);
  if (!IS_EQUAL(rms, rmsNotOpt))
    printf("debug : mybhep::HCL::Base3D::rmsX : anomaly %g %g\n", rms, rmsNotOpt);
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::rmsY() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSyw / fInRangeSw;
  double rms = ::sqrt(::fabs((fInRangeSy2w / fInRangeSw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  double rmsNotOpt = getIthAxisRMS(AxisY);
  if (!IS_EQUAL(rms, rmsNotOpt))
    printf("debug : mybhep::HCL::Base3D::rmsY : anomaly %g %g\n", rms, rmsNotOpt);
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::rmsZ() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fInRangeSw == 0) return 0;
  double mean = fInRangeSzw / fInRangeSw;
  double rms = ::sqrt(::fabs((fInRangeSz2w / fInRangeSw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  double rmsNotOpt = getIthAxisRMS(AxisZ);
  if (!IS_EQUAL(rms, rmsNotOpt))
    printf("debug : mybhep::HCL::Base3D::rmsZ : anomaly %g %g\n", rms, rmsNotOpt);
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::binEntries(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  int entries = fBinEntries[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    int entriesNotOpt = getBinEntries(fInts);
    if (entries != entriesNotOpt)
      printf("debug : mybhep::HCL::Base3D::binEntries : anomaly %d %d\n", entries, entriesNotOpt);
  }
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binHeight(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double height = 0;
  if (fProfile)
    height = (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
  else
    height = fBinSw[offset];
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double heightNotOpt = getBinHeight(fInts);
    if (!IS_EQUAL(height, heightNotOpt))
      printf("debug : mybhep::HCL::Base3D::binHeight : anomaly\n");
  }
#endif
  return height;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binError(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ((Base3D*)this)->fInts[0] = aI;
  ((Base3D*)this)->fInts[1] = aJ;
  ((Base3D*)this)->fInts[2] = aK;
  return getBinError(fInts);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binCenterX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0].binCenter(aI);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binCenterY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[1].binCenter(aJ);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binCenterZ(int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[2].binCenter(aK);
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binMeanX(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisX] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double meanNotOpt = getBinMean(fInts)[0];
    if (!IS_EQUAL(mean, meanNotOpt))
      printf("debug : mybhep::HCL::Base3D::binMeanX : anomaly %g %g\n", mean, meanNotOpt);
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binMeanY(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisY] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double meanNotOpt = getBinMean(fInts)[1];
    if (!IS_EQUAL(mean, meanNotOpt))
      printf("debug : mybhep::HCL::Base3D::binMeanY : anomaly %g %g\n", mean, meanNotOpt);
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binMeanZ(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double mean = fBinSxw[offset][AxisZ] / sw;
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double meanNotOpt = getBinMean(fInts)[2];
    if (!IS_EQUAL(mean, meanNotOpt))
      printf("debug : mybhep::HCL::Base3D::binMeanZ : anomaly %g %g\n", mean, meanNotOpt);
  }
#endif
  return mean;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binStdDevX(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisX];
  double sx2w = fBinSx2w[offset][AxisX];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double rmsNotOpt = getBinStdDev(fInts)[0];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base3D::binStdDevX : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binStdDevY(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisY];
  double sx2w = fBinSx2w[offset][AxisY];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double rmsNotOpt = getBinStdDev(fInts)[1];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base3D::binStdDevY : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binStdDevZ(int aI, int aJ, int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (fBinNumber == 0) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
  double sw = fBinSw[offset];
  if (sw == 0) return 0;
  double sxw = fBinSxw[offset][AxisZ];
  double sx2w = fBinSx2w[offset][AxisZ];
  double mean = sxw / sw;
  double rms = ::sqrt(::fabs((sx2w / sw) - mean * mean));
#ifdef HCL_CHECK_OPTIMIZE
  {
    ((Base3D*)this)->fInts[0] = aI;
    ((Base3D*)this)->fInts[1] = aJ;
    ((Base3D*)this)->fInts[2] = aK;
    double rmsNotOpt = getBinStdDev(fInts)[2];
    if (!IS_EQUAL(rms, rmsNotOpt)) printf("debug : mybhep::HCL::Base3D::binStdDevZ : anomaly\n");
  }
#endif
  return rms;
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base3D::xAxis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[0];
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base3D::yAxis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[1];
}
//////////////////////////////////////////////////////////////////////////////
const mybhep::HCL::Axis& mybhep::HCL::Base3D::zAxis() const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAxes[2];
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::coordToIndexX(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return xAxis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::coordToIndexY(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return yAxis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::coordToIndexZ(double aCoord) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return zAxis().coordToIndex(aCoord);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D* mybhep::HCL::Base3D::sliceXY(int aKbeg, int aKend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  mybhep::HCL::Base2D* sliceXY =
      new Base2D("SliceXY", xAxis().bins(), xAxis().lowerEdge(), xAxis().upperEdge(),
                 yAxis().bins(), yAxis().lowerEdge(), yAxis().upperEdge());
  if (!fillSliceXY(*sliceXY, aKbeg, aKend)) {
    delete sliceXY;
    return 0;
  }
  return sliceXY;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Base3D::fillSliceXY(mybhep::HCL::Base2D& aHisto, int aKbeg, int aKend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (!fAxes[2].inRangetoAbsoluteIndex(aKbeg)) return false;
  if (!fAxes[2].inRangetoAbsoluteIndex(aKend)) return false;
  if (aKbeg > aKend) return false;

  HistogramData& hdata = aHisto.getData();
  if (xAxis().bins() != aHisto.xAxis().bins()) return false;
  if (yAxis().bins() != aHisto.yAxis().bins()) return false;

  int aibin, ajbin, kbin, aoffset, offset;

  int ayoffset = hdata.fAxes[1].fOffset;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;

  int axbins = hdata.fAxes[0].bins() + 2;
  int aybins = hdata.fAxes[1].bins() + 2;
  // Fill also the outflow.
  for (aibin = 0; aibin < axbins; aibin++) {
    for (ajbin = 0; ajbin < aybins; ajbin++) {
      // offset2D = ibin + jbin * fAxes[1].fOffset
      aoffset = aibin + ajbin * ayoffset;
      for (kbin = aKbeg; kbin <= aKend; kbin++) {
        // offset3D = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
        // hdata booked with x-y then :
        offset = aibin + ajbin * yoffset + kbin * zoffset;

        // Bin :
        hdata.fBinEntries[aoffset] += fBinEntries[offset];
        hdata.fBinSw[aoffset] += fBinSw[offset];
        hdata.fBinSw2[aoffset] += fBinSw2[offset];
        hdata.fBinSxw[aoffset][0] += fBinSxw[offset][0];
        hdata.fBinSxw[aoffset][1] += fBinSxw[offset][1];
        hdata.fBinSx2w[aoffset][0] += fBinSx2w[offset][0];
        hdata.fBinSx2w[aoffset][1] += fBinSx2w[offset][1];
        // FIXME hdata.fBinSvw[aoffset] += fBinSvw[offset];
      }
    }
  }
  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D* mybhep::HCL::Base3D::sliceYZ(int aIbeg, int aIend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  mybhep::HCL::Base2D* sliceYZ =
      new Base2D("SliceYZ", yAxis().bins(), yAxis().lowerEdge(), yAxis().upperEdge(),
                 zAxis().bins(), zAxis().lowerEdge(), zAxis().upperEdge());
  if (!fillSliceYZ(*sliceYZ, aIbeg, aIend)) {
    delete sliceYZ;
    return 0;
  }
  return sliceYZ;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Base3D::fillSliceYZ(mybhep::HCL::Base2D& aHisto, int aIbeg, int aIend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (!fAxes[0].inRangetoAbsoluteIndex(aIbeg)) return false;
  if (!fAxes[0].inRangetoAbsoluteIndex(aIend)) return false;
  if (aIbeg > aIend) return false;

  HistogramData& hdata = aHisto.getData();
  if (yAxis().bins() != aHisto.xAxis().bins()) return false;
  if (zAxis().bins() != aHisto.yAxis().bins()) return false;

  int aibin, ajbin, ibin, aoffset, offset;

  int ayoffset = hdata.fAxes[1].fOffset;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;

  int axbins = hdata.fAxes[0].bins() + 2;
  int aybins = hdata.fAxes[1].bins() + 2;
  // Fill also the outflow.
  for (aibin = 0; aibin < axbins; aibin++) {
    for (ajbin = 0; ajbin < aybins; ajbin++) {
      // offset2D = ibin + jbin * fAxes[1].fOffset
      aoffset = aibin + ajbin * ayoffset;
      for (ibin = aIbeg; ibin <= aIend; ibin++) {
        // offset3D = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
        // hdata booked with y-z then :
        offset = ibin + aibin * yoffset + ajbin * zoffset;

        // Bin :
        hdata.fBinEntries[aoffset] += fBinEntries[offset];
        hdata.fBinSw[aoffset] += fBinSw[offset];
        hdata.fBinSw2[aoffset] += fBinSw2[offset];
        hdata.fBinSxw[aoffset][0] += fBinSxw[offset][1];
        hdata.fBinSxw[aoffset][1] += fBinSxw[offset][2];
        hdata.fBinSx2w[aoffset][0] += fBinSx2w[offset][1];
        hdata.fBinSx2w[aoffset][1] += fBinSx2w[offset][2];
        // FIXME hdata.fBinSvw[aoffset] += fBinSvw[offset];
      }
    }
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Base2D* mybhep::HCL::Base3D::sliceXZ(int aJbeg, int aJend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  mybhep::HCL::Base2D* sliceXZ =
      new Base2D("SliceXZ", xAxis().bins(), xAxis().lowerEdge(), xAxis().upperEdge(),
                 zAxis().bins(), zAxis().lowerEdge(), zAxis().upperEdge());
  if (!fillSliceXZ(*sliceXZ, aJbeg, aJend)) {
    delete sliceXZ;
    return 0;
  }
  return sliceXZ;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Base3D::fillSliceXZ(mybhep::HCL::Base2D& aHisto, int aJbeg, int aJend) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return false;
  if (!fAxes[1].inRangetoAbsoluteIndex(aJbeg)) return false;
  if (!fAxes[1].inRangetoAbsoluteIndex(aJend)) return false;
  if (aJbeg > aJend) return false;

  HistogramData& hdata = aHisto.getData();
  if (xAxis().bins() != aHisto.xAxis().bins()) return false;
  if (zAxis().bins() != aHisto.yAxis().bins()) return false;

  int aibin, ajbin, jbin, aoffset, offset;

  int ayoffset = hdata.fAxes[1].fOffset;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;

  int axbins = hdata.fAxes[0].bins() + 2;
  int aybins = hdata.fAxes[1].bins() + 2;
  // Fill also the outflow.
  for (aibin = 0; aibin < axbins; aibin++) {
    for (ajbin = 0; ajbin < aybins; ajbin++) {
      // offset2D = ibin + jbin * fAxes[1].fOffset
      aoffset = aibin + ajbin * ayoffset;
      for (jbin = aJbeg; jbin <= aJend; jbin++) {
        // offset3D = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;
        // hdata booked with x-z then :
        offset = aibin + jbin * yoffset + ajbin * zoffset;

        // Bin :
        hdata.fBinEntries[aoffset] += fBinEntries[offset];
        hdata.fBinSw[aoffset] += fBinSw[offset];
        hdata.fBinSw2[aoffset] += fBinSw2[offset];
        hdata.fBinSxw[aoffset][0] += fBinSxw[offset][0];
        hdata.fBinSxw[aoffset][1] += fBinSxw[offset][2];
        hdata.fBinSx2w[aoffset][0] += fBinSx2w[offset][0];
        hdata.fBinSx2w[aoffset][1] += fBinSx2w[offset][2];
        // FIXME hdata.fBinSvw[aoffset] += fBinSvw[offset];
      }
    }
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::binEntriesX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  int entries = 0;
  int jbin, kbin, joffset, offset;
  int ybins = fAxes[1].bins() + 2;
  int zbins = fAxes[2].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  joffset = ibin;
  for (jbin = 0; jbin < ybins; jbin++) {
    // joffset = ibin + jbin * fAxes[1].fOffset;
    offset = joffset;
    for (kbin = 0; kbin < zbins; kbin++) {
      // offset = joffset + kbin * fAxes[2].fOffset;
      entries += fBinEntries[offset];
      offset += zoffset;
    }
    joffset += yoffset;
  }
#ifdef HCL_CHECK_OPTIMIZE
  if (entries != getIthAxisBinEntries(0, aI))
    printf("debug : binEntriesX : anomaly at %d : %d\n", aI, entries);
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binHeightX(int aI) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // to slow : return getIthAxisBinHeight(0,aI);
  if (!fDimension) return 0;
  int ibin = aI;
  if (!fAxes[0].inRangetoAbsoluteIndex(ibin)) return 0;
  double sw = 0;
  int jbin, kbin, joffset, offset;
  int ybins = fAxes[1].bins() + 2;
  int zbins = fAxes[2].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  if (fProfile) {
    joffset = ibin;
    for (jbin = 0; jbin < ybins; jbin++) {
      // joffset = ibin + jbin * fAxes[1].fOffset;
      offset = joffset;
      for (kbin = 0; kbin < zbins; kbin++) {
        // offset = joffset + kbin * fAxes[2].fOffset;
        sw += (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
        offset += zoffset;
      }
      joffset += yoffset;
    }
  } else {
    joffset = ibin;
    for (jbin = 0; jbin < ybins; jbin++) {
      // joffset = ibin + jbin * fAxes[1].fOffset;
      offset = joffset;
      for (kbin = 0; kbin < zbins; kbin++) {
        // offset = joffset + kbin * fAxes[2].fOffset;
        sw += fBinSw[offset];
        offset += zoffset;
      }
      joffset += yoffset;
    }
  }
#ifdef HCL_CHECK_OPTIMIZE
  double swNotOpt = getIthAxisBinHeight(0, aI);
  if (!IS_EQUAL(sw, swNotOpt))
    printf("debug : mybhep::HCL::Base3D::binHeightX : anomaly at %d : %g %g\n", aI, sw, swNotOpt);
#endif
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::binEntriesY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  int entries = 0;
  int ibin, kbin, joffset, offset;
  int xbins = fAxes[0].bins() + 2;
  int zbins = fAxes[2].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  joffset = jbin * yoffset;
  for (ibin = 0; ibin < xbins; ibin++) {
    // joffset = ibin + jbin * fAxes[1].fOffset;
    offset = joffset;
    for (kbin = 0; kbin < zbins; kbin++) {
      // offset = joffset + kbin * fAxes[2].fOffset;
      entries += fBinEntries[offset];
      offset += zoffset;
    }
    joffset++;
  }
#ifdef HCL_CHECK_OPTIMIZE
  if (entries != getIthAxisBinEntries(1, aJ))
    printf("debug : mybhep::HCL::Base3D::binEntriesY : anomaly at %d : %d\n", aJ, entries);
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binHeightY(int aJ) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int jbin = aJ;
  if (!fAxes[1].inRangetoAbsoluteIndex(jbin)) return 0;
  double sw = 0;
  int ibin, kbin, joffset, offset;
  int xbins = fAxes[0].bins() + 2;
  int zbins = fAxes[2].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  if (fProfile) {
    joffset = jbin * yoffset;
    for (ibin = 0; ibin < xbins; ibin++) {
      // joffset = ibin + jbin * fAxes[1].fOffset;
      offset = joffset;
      for (kbin = 0; kbin < zbins; kbin++) {
        // offset = joffset + kbin * fAxes[2].fOffset;
        sw += (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
        offset += zoffset;
      }
      joffset++;
    }
  } else {
    joffset = jbin * yoffset;
    for (ibin = 0; ibin < xbins; ibin++) {
      // joffset = ibin + jbin * fAxes[1].fOffset;
      offset = joffset;
      for (kbin = 0; kbin < zbins; kbin++) {
        // offset = joffset + kbin * fAxes[2].fOffset;
        sw += fBinSw[offset];
        offset += zoffset;
      }
      joffset++;
    }
  }
#ifdef HCL_CHECK_OPTIMIZE
  double swNotOpt = getIthAxisBinHeight(1, aJ);
  if (!IS_EQUAL(sw, swNotOpt))
    printf("debug : mybhep::HCL::Base3D::binHeightY : anomaly at %d : %g %g\n", aJ, sw, swNotOpt);
#endif
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Base3D::binEntriesZ(int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  int entries = 0;
  int ibin, jbin, koffset, offset;
  int xbins = fAxes[0].bins() + 2;
  int ybins = fAxes[1].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  koffset = kbin * zoffset;
  for (ibin = 0; ibin < xbins; ibin++) {
    // koffset = ibin + kbin * fAxes[2].fOffset;
    offset = koffset;
    for (jbin = 0; jbin < ybins; jbin++) {
      // offset = koffset + jbin * fAxes[1].fOffset;
      entries += fBinEntries[offset];
      offset += yoffset;
    }
    koffset++;
  }
#ifdef HCL_CHECK_OPTIMIZE
  if (entries != getIthAxisBinEntries(2, aK))
    printf("debug : mybhep::HCL::Base3D::binEntriesZ : anomaly at %d : %d\n", aK, entries);
#endif
  return entries;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Base3D::binHeightZ(int aK) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (!fDimension) return 0;
  int kbin = aK;
  if (!fAxes[2].inRangetoAbsoluteIndex(kbin)) return 0;
  double sw = 0;
  int ibin, jbin, koffset, offset;
  int xbins = fAxes[0].bins() + 2;
  int ybins = fAxes[1].bins() + 2;
  int yoffset = fAxes[1].fOffset;
  int zoffset = fAxes[2].fOffset;
  if (fProfile) {
    koffset = kbin * zoffset;
    for (ibin = 0; ibin < xbins; ibin++) {
      // koffset = ibin + kbin * fAxes[2].fOffset;
      offset = koffset;
      for (jbin = 0; jbin < ybins; jbin++) {
        // offset = koffset + jbin * fAxes[1].fOffset;
        sw += (fBinSw[offset] ? (fBinSvw[offset] / fBinSw[offset]) : 0);
        offset += yoffset;
      }
      koffset++;
    }
  } else {
    koffset = kbin * zoffset;
    for (ibin = 0; ibin < xbins; ibin++) {
      // koffset = ibin + kbin * fAxes[2].fOffset;
      offset = koffset;
      for (jbin = 0; jbin < ybins; jbin++) {
        // offset = koffset + jbin * fAxes[1].fOffset;
        sw += fBinSw[offset];
        offset += yoffset;
      }
      koffset++;
    }
  }
#ifdef HCL_CHECK_OPTIMIZE
  double swNotOpt = getIthAxisBinHeight(2, aK);
  if (!IS_EQUAL(sw, swNotOpt))
    printf("debug : mybhep::HCL::Base3D::binHeightZ : anomaly at %d : %g %g\n", aK, sw, swNotOpt);
#endif
  return sw;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram3D::Histogram3D(const std::string& aString, int aXnumber, double aXmin,
                                      double aXmax, int aYnumber, double aYmin, double aYmax,
                                      int aZnumber, double aZmin, double aZmax)
    : Base3D(aString, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax, aZnumber, aZmin, aZmax)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram3D::Histogram3D(const std::string& aString,
                                      const std::vector<double>& aEdgesX,
                                      const std::vector<double>& aEdgesY,
                                      const std::vector<double>& aEdgesZ)
    : Base3D(aString, aEdgesX, aEdgesY, aEdgesZ)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram3D::Histogram3D(const Histogram3D& aFrom)
    //:IBase(aFrom)
    //,IBase3D(aFrom)
    //,IHistogram3D(aFrom)
    : Base3D(aFrom)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Histogram3D::~Histogram3D()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Histogram3D::fill(double aX, double aY, double aZ, double aWeight)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // fDoubles[0] = aX;
  // fDoubles[1] = aY;
  // fDoubles[2] = aZ;
  // return fillBin(fDoubles,aWeight);

  if (fDimension <= 0) return false;

  int ibin = fAxes[0].coordToAbsoluteIndex(aX);
  if (ibin == NotFound) return false;
  int jbin = fAxes[1].coordToAbsoluteIndex(aY);
  if (jbin == NotFound) return false;
  int kbin = fAxes[2].coordToAbsoluteIndex(aZ);
  if (kbin == NotFound) return false;

  int offset = ibin + jbin * fAxes[1].fOffset + kbin * fAxes[2].fOffset;

  fBinEntries[offset]++;
  fBinSw[offset] += aWeight;
  fBinSw2[offset] += aWeight * aWeight;

  double xw = aX * aWeight;
  double x2w = aX * xw;
  fBinSxw[offset][0] += xw;
  fBinSx2w[offset][0] += x2w;

  double yw = aY * aWeight;
  double y2w = aY * yw;
  fBinSxw[offset][1] += yw;
  fBinSx2w[offset][1] += y2w;

  double zw = aZ * aWeight;
  double z2w = aZ * zw;
  fBinSxw[offset][2] += zw;
  fBinSx2w[offset][2] += z2w;

  bool inRange = true;
  if (ibin == 0)
    inRange = false;
  else if (ibin == (fAxes[0].fNumberOfBins + 1))
    inRange = false;

  if (jbin == 0)
    inRange = false;
  else if (jbin == (fAxes[1].fNumberOfBins + 1))
    inRange = false;

  if (kbin == 0)
    inRange = false;
  else if (kbin == (fAxes[2].fNumberOfBins + 1))
    inRange = false;

  if (inRange) {
    fInRangeEntries++;
    fInRangeSw += aWeight;

    fInRangeSxw += xw;
    fInRangeSx2w += x2w;

    fInRangeSyw += yw;
    fInRangeSy2w += y2w;

    fInRangeSzw += zw;
    fInRangeSz2w += z2w;
  }

  return true;
}
