#ifndef HCL_Axis_h
#define HCL_Axis_h

#include <string>
#include <vector>

namespace mybhep{
  namespace HCL {

    class Axis {
    public:
      enum { UNDERFLOW_BIN = -2, OVERFLOW_BIN = -1 };
    public: //IAxis
      virtual bool isFixedBinning() const;
      virtual double lowerEdge() const;
      virtual double upperEdge() const;
      virtual int bins() const;
      virtual double binLowerEdge(int) const;
      virtual double binUpperEdge(int) const;
      virtual double binWidth(int) const;
      virtual double binCenter(int) const;
      virtual int coordToIndex(double) const;
    public:
      // Partition :
      virtual bool configure(const std::vector<double>&);
      virtual bool configure(int,double,double);
      virtual bool isCompatible(const Axis&) const;
      // Partition and bin :
      virtual const std::vector<double>& edges() const;
    public:
      Axis();
      Axis(const Axis&);
      virtual ~Axis();
      Axis& operator=(const Axis&);
      int coordToAbsoluteIndex(double) const;
      bool inRangetoAbsoluteIndex(int&) const;
    public:
      int fOffset;
      int fNumberOfBins;
      double fMinimumValue;
      double fMaximumValue;
      bool fFixed;
      // Fixed size bins :
      double fBinWidth;
      // Variable size bins :
      std::vector<double> fEdges;
    };
    
  }
}

#endif




