#ifndef HCL_Histogram_h
#define HCL_Histogram_h

#include <vector>

#include <mybhep/axis.h> 

namespace mybhep{
  namespace HCL {

    class HistogramData {
    public:
      HistogramData();
      HistogramData(const HistogramData&);
      virtual ~HistogramData();
      void copy(const HistogramData&);
    public:
      // General :
      std::string fTitle;
      int fDimension;
      // Bins :
      int fBinNumber;
      std::vector<int> fBinEntries;
      std::vector<double> fBinSw;
      std::vector<double> fBinSw2;
      std::vector< std::vector<double> > fBinSxw;
      std::vector< std::vector<double> > fBinSx2w;
      // Axes :
      std::vector<Axis> fAxes;
      // Others :
      int fMode;
      // Profile :
      bool fProfile;
      std::vector<double> fBinSvw;
      std::vector<double> fBinSv2w;
      bool fCutV;
      double fMinV;
      double fMaxV;
      // Args :
      std::vector<double> fDoubles;
      std::vector<int> fInts;
    };

    class Histogram : protected HistogramData {
    public:
      enum {NotFound=-1};
    public:
      Histogram();
      Histogram(int,
		const std::vector<int>&,
		const std::vector<double>&,
		const std::vector<double>&,
		bool = false);
      Histogram(int,
		const std::vector< std::vector<double> >&,
		bool = false);
      Histogram(const Histogram&);
      Histogram(const HistogramData&);
      virtual ~Histogram();
      Histogram& operator=(const Histogram&);
      HistogramData& getData();
      const HistogramData& getData() const;
      void copy(const Histogram&);               
      bool add(const Histogram&);
      bool subtract(const Histogram&);
      bool multiply(const Histogram&);
      bool divide(const Histogram&);
      bool multiply(double); 
      bool isProfile() const;
      inline int getOffset(const std::vector<int>&) const;
      int getOffset(const std::vector<double>&) const;
      void getIndices(int,std::vector<int>&) const;
      inline bool isOut(int) const;
      const HCL::Axis& getAxis(int) const;
      int getDimension() const;
      int getEntries() const;
      int getAllEntries() const;
      double getSw() const;
      double getSw2() const;
      int getBins() const;
      double getIthAxisSxw(int) const;
      double getIthAxisSx2w(int) const;
      const std::string& getTitle() const;
      virtual void updateFastGetters();
      bool isValid() const;
    protected:
      bool configure(int,
		     const std::vector<int>&,
		     const std::vector<double>&,
		     const std::vector<double>&,
		     bool = false);
      bool configure(int,
		     const std::vector< std::vector<double> >&,
		     bool = false);
      bool isCompatible(const Histogram&) const;
      // Axes :
      int getIthAxisBinEntries(int,int) const;
      double getIthAxisBinHeight(int,int) const;
      double getIthAxisMean(int) const;
      double getIthAxisRMS(int) const;
      // Bins :
      bool fillBin(const std::vector<double>&,double);
      bool fillBin(const std::vector<double>&,double,double);
      int getBinEntries(const std::vector<int>&) const;
      double getBinHeight(const std::vector<int>&) const;
      double getBinError(const std::vector<int>&) const;
      std::vector<double> getBinMean(const std::vector<int>&) const;
      std::vector<double> getBinStdDev(const std::vector<int>&) const;
      double getBinRMS(const std::vector<int>&) const;
      // General :
      void clear();
      void baseReset();
    private:
      void allocate();
    };

#define HISTOGRAM_COMMON                       \
  virtual const std::string& title() const;    \
  virtual bool setTitle(const std::string&);   \
  virtual int dimension() const;               \
  virtual bool reset();                        \
  virtual int entries() const;                 \
  virtual int allEntries() const;              \
  virtual int extraEntries() const;            \
  virtual double equivalentBinEntries() const; \
  virtual double sumBinHeights() const;        \
  virtual double sumAllBinHeights() const;     \
  virtual double sumExtraBinHeights() const;   \
  virtual double minBinHeight() const;         \
  virtual double maxBinHeight() const;         \
  virtual bool scale(double);
    
    class Base1D : public Histogram {
    public: //IMethods :
      HISTOGRAM_COMMON
	// Partition :
	virtual int coordToIndex(double) const;
      virtual double mean() const;
      virtual double rms() const;
      // Bins :
      virtual int binEntries(int) const;
      virtual double binHeight(int) const;
      virtual double binError(int) const;
      virtual double binCenter(int) const;
      virtual double binMean(int) const;
      virtual double binStdDev(int) const;
      // Axis :
      virtual const Axis& axis() const;
    public:
      Base1D(const std::string&,int,double,double,bool = false);
      Base1D(const std::string&,const std::vector<double>&,bool = false);
      Base1D(const Base1D&);
      virtual ~Base1D();
    };
    
    class Histogram1D : public Base1D {
    public: //IMethods :
      virtual bool fill(double,double = 1.0);
    public:
      Histogram1D(const std::string&,int,double,double);
      Histogram1D(const std::string&,const std::vector<double>&);
      Histogram1D(const Histogram1D&);
      virtual ~Histogram1D();
    };
    
    class Profile1D : public Base1D {
    public: //IMethods :
      virtual bool fill(double,double,double = 1.0);
      virtual double binRms(int) const;
    public:
      Profile1D(const std::string&,int,double,double);
      Profile1D(const std::string&,int,double,double,double,double);
      Profile1D(const std::string&,const std::vector<double>&);
      Profile1D(const std::string&,const std::vector<double>&,double,double);
      Profile1D(const Profile1D&);
      virtual ~Profile1D();
    };

    class Base2D : public Histogram {
    public: //IMethods :
      HISTOGRAM_COMMON
	// Partition :
	virtual double meanX() const;
      virtual double rmsX() const;
      virtual int coordToIndexX(double) const;
      //
      virtual double meanY() const;
      virtual double rmsY() const;
      virtual int coordToIndexY(double) const;
      // ints :
      virtual int binEntries(int,int) const;
      virtual double binHeight(int,int) const;
      virtual double binError(int,int) const;
      virtual double binCenterX(int) const;
      virtual double binCenterY(int) const;
      virtual double binMeanX(int,int) const;
      virtual double binMeanY(int,int) const;
      virtual double binStdDevX(int,int) const;
      virtual double binStdDevY(int,int) const;
      // Axes :
      virtual const Axis& xAxis() const;
      virtual const Axis& yAxis() const;
      // Projection :
      virtual int binEntriesX(int) const;
      virtual double binHeightX(int) const;
      virtual int binEntriesY(int) const;
      virtual double binHeightY(int) const;
      // Slices :
      virtual Base1D* sliceX(int,int) const;
      virtual Base1D* sliceY(int,int) const;
      virtual Base1D* projectionX() const;
      virtual Base1D* projectionY() const;
    public:
      Base2D(const std::string&,
	     int,double,double,
	     int,double,double,
	     bool = false);
      Base2D(const std::string&,
	     const std::vector<double>&,
	     const std::vector<double>&,
	     bool = false);
      Base2D(const Base2D&);
      virtual ~Base2D();
    private:
      bool fillSliceX(Base1D&,int,int) const;
      bool fillSliceY(Base1D&,int,int) const;
      virtual void updateFastGetters();
    protected:
      int fInRangeEntries;
      double fInRangeSw;
      double fInRangeSxw;
      double fInRangeSyw;
      double fInRangeSx2w;
      double fInRangeSy2w;
    };
    
    class Histogram2D : public Base2D {
    public: //IMethods :
      virtual bool fill(double,double,double = 1.0);
    public:
      Histogram2D(const std::string&,
		  int,double,double,
		  int,double,double);
      Histogram2D(const std::string&,
		  const std::vector<double>&,
		  const std::vector<double>&);
      Histogram2D(const Histogram2D&);
      virtual ~Histogram2D();
    };
    
    class Profile2D : public Base2D {
    public: //IMethods :
      virtual bool fill(double,double,double,double = 1.0);
      virtual double binRms(int,int) const;
    public:
      Profile2D(const std::string&,
		int,double,double,
		int,double,double);
      Profile2D(const std::string&,
		int,double,double,
		int,double,double,
		double,double);
      Profile2D(const std::string&,
		const std::vector<double>&,
		const std::vector<double>&);
      Profile2D(const std::string&,
		const std::vector<double>&,
		const std::vector<double>&,
		double,double);
      Profile2D(const Profile2D&);
      virtual ~Profile2D();
    };
    
    class Base3D : public Histogram {
    public: //IMethods :
      HISTOGRAM_COMMON
	// Partition :
	virtual double meanX() const;
      virtual double rmsX() const;
      virtual int coordToIndexX(double) const;
      //
      virtual double meanY() const;
      virtual double rmsY() const;
      virtual int coordToIndexY(double) const;
      //
      virtual double meanZ() const;
      virtual double rmsZ() const;
      virtual int coordToIndexZ(double) const;
      // ints :
      virtual int binEntries(int,int,int) const;
      virtual double binHeight(int,int,int) const;
      virtual double binError(int,int,int) const;
      virtual double binCenterX(int) const;
      virtual double binCenterY(int) const;
      virtual double binCenterZ(int) const;
      virtual double binMeanX(int,int,int) const;
      virtual double binMeanY(int,int,int) const;
      virtual double binMeanZ(int,int,int) const;
      virtual double binStdDevX(int,int,int) const;
      virtual double binStdDevY(int,int,int) const;
      virtual double binStdDevZ(int,int,int) const;
      // Axes :
      virtual const Axis& xAxis() const;
      virtual const Axis& yAxis() const;
      virtual const Axis& zAxis() const;
      // Projection :
      virtual int binEntriesX(int) const;
      virtual int binEntriesY(int) const;
      virtual int binEntriesZ(int) const;
      virtual double binHeightX(int) const;
      virtual double binHeightY(int) const;
      virtual double binHeightZ(int) const;
    public:
      // Slices :
      virtual Base2D* sliceXY(int,int) const;
      virtual Base2D* sliceYZ(int,int) const;
      virtual Base2D* sliceXZ(int,int) const;
    public:
      Base3D(const std::string&,
	     int,double,double,
	     int,double,double,
	     int,double,double);
      Base3D(const std::string&,
	     const std::vector<double>&,
	     const std::vector<double>&,
	     const std::vector<double>&);
      Base3D(const Base3D&);
      virtual ~Base3D();
    private:
      bool fillSliceXY(Base2D&,int,int) const;
      bool fillSliceYZ(Base2D&,int,int) const;
      bool fillSliceXZ(Base2D&,int,int) const;
      virtual void updateFastGetters();
    protected:
      int fInRangeEntries;
      double fInRangeSw;
      double fInRangeSxw;
      double fInRangeSyw;
      double fInRangeSzw;
      double fInRangeSx2w;
      double fInRangeSy2w;
      double fInRangeSz2w;
    };
    
    class Histogram3D : public Base3D {
    public: //IMethods :
      virtual bool fill(double,double,double,double = 1.0);
    public:
      Histogram3D(const std::string&,
		  int,double,double,
		  int,double,double,
		  int,double,double);
      Histogram3D(const std::string&,
		  const std::vector<double>&,
		  const std::vector<double>&,
		  const std::vector<double>&);
      Histogram3D(const Histogram3D&);
      virtual ~Histogram3D();
    };
    
  }
  
  //////////////////////////////////////////////////////////////////////////////
  inline bool HCL::Histogram::isOut(
				    int aOffset
				    ) const
    //////////////////////////////////////////////////////////////////////////////
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    {
      int iaxis;
      int offset = aOffset;
      int index;
      for(iaxis=fDimension-1;iaxis>=0;iaxis--) { 
	index = offset/fAxes[iaxis].fOffset;
	if(index==0) return true;
	if(index==(fAxes[iaxis].fNumberOfBins+1)) return true;
	offset -= index * fAxes[iaxis].fOffset;
      }
      return false;
    }
  
  //////////////////////////////////////////////////////////////////////////////
  inline int HCL::Histogram::getOffset(
				       const std::vector<int>& aIs
				       ) const
    //////////////////////////////////////////////////////////////////////////////
    // aIs[iaxis] is given in in-range indexing :
    //  - [0,n[iaxis]-1] for in-range bins
    //  - UNDERFLOW_BIN for the iaxis underflow bin
    //  - OVERFLOW_BIN for the iaxis overflow bin
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    {
      if(fDimension<=0) return NotFound;
      int offset = 0;
      for(int iaxis=0;iaxis<fDimension;iaxis++) { 
	int ibin = aIs[iaxis];
    if(!fAxes[iaxis].inRangetoAbsoluteIndex(ibin)) return NotFound;
    offset += ibin * fAxes[iaxis].fOffset;
      }
      return offset;
    }
}
#endif




