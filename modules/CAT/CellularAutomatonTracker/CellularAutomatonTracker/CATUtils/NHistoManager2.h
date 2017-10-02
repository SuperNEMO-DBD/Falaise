#ifndef SHMAN__
#define SHMAN__

#include <mybhep/gstore.h>
#include <CATUtils/ROOTbase.h>

class NHistoManager2 {
 public:
  NHistoManager2(){};
  NHistoManager2(mybhep::gstore);
  NHistoManager2(std::string);
  ~NHistoManager2(){};

  // general manager functions
  bool find(std::string name) { return hstore.find(name); };
  TH1* fetch(std::string name) { return hstore[name]; };

  // book functions
  void h1(std::string name, std::string title, size_t nbin, double fb, double lb);
  void h2(std::string name, std::string title, size_t nxbin, double fbx, double lbx, size_t nybin,
          double fby, double lby);

  // fill functions
  void fill(std::string name, double value);
  void fill(std::string name, double value1, double value2);
  void fillw(std::string name, double value1, double weight);

  // opertations

  void divide1D(std::string name1, std::string name2, std::string name3);
  void divide2D(std::string name1, std::string name2, std::string name3);

  // IO functions
  void save();

  // operators
  TH1* operator[](std::string name);

 private:
  // attributes

  mybhep::tstore<TH1*> hstore;

  TObjArray hlist;

  // messenger mhisto;

  // functions
  void readHistoParam(mybhep::gstore st);

  // parameters

  std::string hfile;
};

#endif
