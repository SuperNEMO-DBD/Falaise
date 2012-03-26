#ifndef SHMAN__
#define SHMAN__

#include<mybhep/gstore.h>
#include<CATUtils/ROOTbase.h>

 class NHistoManager2{
  
 public:

  NHistoManager2(){};
  NHistoManager2(mybhep::gstore);
  NHistoManager2(string);
  ~NHistoManager2(){};
 
  //general manager functions
  bool find(string name){return hstore.find(name);};
  TH1* fetch(string name){ return hstore[name];};
  
  //book functions
  void h1(string name,string title,size_t nbin,double fb,double lb);
  void h2(string name,string title,size_t nxbin,
	  double fbx,double lbx,size_t nybin,double fby,double lby);
  
  //fill functions
  void fill(string name,double value);
  void fill(string name,double value1,double value2);
  void fillw(string name, double value1, double weight);
  
  //opertations
  
  void divide1D(string name1,string name2,string name3);
  void divide2D(string name1,string name2,string name3);

  //IO functions
  void save();
  
  //operators
  TH1* operator[](string name);

 private:
  
  //attributes
  
  mybhep::tstore<TH1*> hstore;
  
  TObjArray hlist;

  //messenger mhisto;

  //functions
  void readHistoParam(mybhep::gstore st);
  
  //parameters
  
  string hfile;
    

};

#endif
