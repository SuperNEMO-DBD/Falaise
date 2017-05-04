// this :
#include <mybhep/axis.h>

#define NotFound (-1)

//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Axis::Axis(
)
:fOffset(0)
,fNumberOfBins(0)
,fMinimumValue(0)
,fMaximumValue(0)
,fFixed(true)
,fBinWidth(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Axis::Axis(
 const Axis& aAxis
)
//:IAxis()
:fOffset(aAxis.fOffset)
,fNumberOfBins(aAxis.fNumberOfBins)
,fMinimumValue(aAxis.fMinimumValue)
,fMaximumValue(aAxis.fMaximumValue)
,fFixed(aAxis.fFixed)
,fBinWidth(aAxis.fBinWidth)
,fEdges(aAxis.fEdges)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Axis::~Axis(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
mybhep::HCL::Axis& mybhep::HCL::Axis::operator=(
 const Axis& aAxis
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOffset = aAxis.fOffset;
  fNumberOfBins = aAxis.fNumberOfBins;
  fMinimumValue = aAxis.fMinimumValue;
  fMaximumValue = aAxis.fMaximumValue;
  fFixed = aAxis.fFixed;
  fBinWidth = aAxis.fBinWidth;
  fEdges = aAxis.fEdges;
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Axis::configure(
 const std::vector<double>& aEdges
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // init :
  fNumberOfBins = 0;
  fMinimumValue = 0;
  fMaximumValue = 0;
  fFixed = true;
  fBinWidth = 0;
  fEdges.clear();
  // setup :
  if(aEdges.size()<=1) return false;
  int number = aEdges.size()-1;
  for(int index=0;index<number;index++) {
    if((aEdges[index]>=aEdges[index+1])) {
      return false;
    }
  }
  fEdges = aEdges;
  fNumberOfBins = number;
  fMinimumValue = aEdges[0];
  fMaximumValue = aEdges[fNumberOfBins];
  fFixed = false;
  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Axis::configure(
 int aNumber
,double aMin
,double aMax
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // init :
  fNumberOfBins = 0;
  fMinimumValue = 0;
  fMaximumValue = 0;
  fFixed = true;
  fBinWidth = 0;
  fEdges.clear();
  // setup :
  if(aNumber<=0) return false;
  if(aMax<=aMin) return false;
  fNumberOfBins = aNumber;
  fMinimumValue = aMin;
  fMaximumValue = aMax;
  fBinWidth = (aMax - aMin)/ aNumber;
  fFixed = true;
  return true;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Axis::isFixedBinning(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fFixed;
}
//////////////////////////////////////////////////////////////////////////////
const std::vector<double>& mybhep::HCL::Axis::edges(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fEdges;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::lowerEdge(
) const 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fMinimumValue;
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::upperEdge(
) const 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fMaximumValue;
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Axis::bins(
) const 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fNumberOfBins;
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Axis::inRangetoAbsoluteIndex(
 int& aIndex
) const
//////////////////////////////////////////////////////////////////////////////
// aIndex is given in in-range indexing :
//  - [0,n-1] for in-range bins
//  - UNDERFLOW_BIN for the iaxis underflow bin
//  - OVERFLOW_BIN for the iaxis overflow bin
// Return the absolute indexing in [0,n+1].
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aIndex==Axis::UNDERFLOW_BIN) {
    aIndex = 0;
    return true;
  } else if(aIndex==Axis::OVERFLOW_BIN) {
    aIndex = fNumberOfBins+1;
    return true;
  } else if((aIndex>=0)&&(aIndex<fNumberOfBins)){
    aIndex++;
    return true;
  } else {
    return false;
  }
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Axis::coordToIndex(
 double aValue
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if( aValue <  fMinimumValue) {
    return Axis::UNDERFLOW_BIN;
  } else if( aValue >= fMaximumValue) {
    return Axis::OVERFLOW_BIN;
  } else {
    if(fFixed) {
      return (int)((aValue - fMinimumValue)/fBinWidth);
    } else {
      for(int index=0;index<fNumberOfBins;index++) {
        if((fEdges[index]<=aValue)&&(aValue<fEdges[index+1])) {
          return index;
        }
      }
      // Should never pass here...
      return Axis::UNDERFLOW_BIN;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
int mybhep::HCL::Axis::coordToAbsoluteIndex(
 double aValue
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if( aValue <  fMinimumValue) {
    return 0;
  } else if( aValue >= fMaximumValue) {
    return fNumberOfBins+1;
  } else {
    if(fFixed) {
      return (int)((aValue - fMinimumValue)/fBinWidth)+1;
    } else {
      for(int index=0;index<fNumberOfBins;index++) {
        if((fEdges[index]<=aValue)&&(aValue<fEdges[index+1])) {
          return (index+1);
        }
      }
      // Should never pass here...
      return NotFound;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::binWidth(
 int aBin
) const 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aBin==Axis::UNDERFLOW_BIN) {
    return 0; //FIXME return DBL_MAX;
  } else if(aBin==Axis::OVERFLOW_BIN) {
    return 0; //FIXME return DBL_MAX;
  } else if((aBin<0) ||(aBin>=fNumberOfBins)) {
    return 0;
  } else {
    if(fFixed) {
      return fBinWidth;
    } else {
      return (fEdges[aBin+1]-fEdges[aBin]);
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::binLowerEdge(
 int aBin
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aBin==Axis::UNDERFLOW_BIN) {
    return 0; //FIXME return -DBL_MAX;
  } else if(aBin==Axis::OVERFLOW_BIN) {
    return 0; //FIXME return binUpperEdge(fNumberOfBins-1);
  } else if((aBin<0) ||(aBin>=fNumberOfBins)) {
    return 0;
  } else {
    if(fFixed) {
      return (fMinimumValue + aBin * fBinWidth);
    } else {
      return fEdges[aBin];
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::binUpperEdge(
 int aBin
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aBin==Axis::UNDERFLOW_BIN) {
    return 0; //FIXME binLowerEdge(0)
  } else if(aBin==Axis::OVERFLOW_BIN) {
    return 0; //FIXME return DBL_MAX;
  } else if((aBin<0) ||(aBin>=fNumberOfBins)) {
    return 0;
  } else {
    if(fFixed) {
      return (fMinimumValue + (aBin + 1) * fBinWidth);
    } else {
      return fEdges[aBin+1];
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
double mybhep::HCL::Axis::binCenter(
 int aBin
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aBin==Axis::UNDERFLOW_BIN) {
    return 0; //FIXME : -INF
  } else if(aBin==Axis::OVERFLOW_BIN) {
    return 0; //FIXME : +INF
  } else if(aBin<0) {
    return 0; //FIXME : -INF
  } else if(aBin>=fNumberOfBins) {
    return 0; //FIXME : +INF
  } else {
    if(fFixed) {
      return (fMinimumValue + (aBin + 0.5) * fBinWidth);
    } else {
      return (fEdges[aBin] + fEdges[aBin+1])/2.;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
bool mybhep::HCL::Axis::isCompatible(
 const Axis& aAxis
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fNumberOfBins!=aAxis.fNumberOfBins) return false;
  if(fMinimumValue!=aAxis.fMinimumValue) return false;
  if(fMaximumValue!=aAxis.fMaximumValue) return false;
  return true;
}
