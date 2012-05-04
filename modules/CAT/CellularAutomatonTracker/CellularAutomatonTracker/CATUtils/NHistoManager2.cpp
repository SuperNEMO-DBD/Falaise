#include <CATUtils/NHistoManager2.h>
#include <CATUtils/NHistogramFunctions2.h>

namespace CAT{
  namespace utils{

    using namespace std;

    NHistoManager2::NHistoManager2(mybhep::gstore st){
  
      readHistoParam(st);
  
      hlist=TObjArray(0);

    }

    NHistoManager2::NHistoManager2(string histo_file){
  
      hfile=histo_file;
  
      hlist=TObjArray(0);
  
    }

    void NHistoManager2::readHistoParam(mybhep::gstore st){

      hfile=st.fetch_sstore("histo_file");  
 
    }


    TH1* NHistoManager2::operator[](string name){ 

      return fetch(name); 

    }


    void NHistoManager2::h1(string name,string title,size_t nbin,double fb,double lb)
    {

      TH1F* h = new TH1F(name.c_str(),title.c_str(),nbin,fb,lb);
  
      hstore.store(name,h);
  
      hlist.Add(h);

    }

    void NHistoManager2::h2(string name,string title,size_t nxbin,
                            double fbx,double lbx,size_t nybin,double fby,double lby)
    {

      TH2F* h = new TH2F(name.c_str(),title.c_str(),nxbin,fbx,lbx,
                         nybin,fby,lby);
  
      hstore.store(name,h);

      hlist.Add(h);
    }

    void NHistoManager2::fill(string name, double value1){
  

      TH1F* h = dynamic_cast<TH1F*>(fetch(name));

      h->Fill(value1);
    
    }

    void NHistoManager2::fill(string name, double value1, double value2){

      TH2F* h = dynamic_cast<TH2F*>(fetch(name));
 
      h->Fill(value1,value2);
    
    }

    void NHistoManager2::fillw(string name, double value1, double weight){
  

      TH1F* h = dynamic_cast<TH1F*>(fetch(name));

      h->Fill(value1, weight);
    
    }

    void NHistoManager2::divide1D(string name1,string name2,string name3){

      TH1F* h1 = dynamic_cast<TH1F*>(fetch(name1));
      TH1F* h2 = dynamic_cast<TH1F*>(fetch(name2));
   
      TH1F* h3 = Divide1D(h1,h2);
      h3->SetName(name3.c_str());h3->SetTitle(name3.c_str());

      hstore.store(name3,h3);

      hlist.Add(h3);

    }

    void NHistoManager2::divide2D(string name1,string name2,string name3){

      TH2F* h1 = dynamic_cast<TH2F*>(fetch(name1));
      TH2F* h2 = dynamic_cast<TH2F*>(fetch(name2));
  
      TH2F* h3 = Divide2D(h1,h2);
      h3->SetName(name3.c_str());h3->SetTitle(name3.c_str());
 
      hstore.store(name3,h3);
      hlist.Add(h3);
    }

    void NHistoManager2::save(){
 
      TFile f(hfile.c_str(),"recreate");

      hlist.Write(0);
  
      f.Close();
  
    }

  } // end of namespace utils
} // end of namespace CAT

