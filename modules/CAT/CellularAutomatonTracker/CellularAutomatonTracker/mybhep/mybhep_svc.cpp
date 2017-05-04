
#include <mybhep/mybhep_svc.h>
#include <mybhep/error.h>
#include <mybhep/container_algorithm.h>
#include <iostream>
#include <fstream>
#include <mybhep/converter_svc.h>

namespace mybhep{

  mybhep_svc* mybhep_svc::m_instance = 0;
  
  mybhep_svc::mybhep_svc() 
  {
    Assert(m_instance == 0,__FILE__,__LINE__,
	   internal_logic
	   ("mybhep_svc already created"));

   m_instance = this;
    csvc_ = new converter_svc;
    m_instance->rt_ = new reader_txt;
    m_instance->rz_ = new reader_gz;
    
    
    m_instance->wt_ = new writer_txt;
    m_instance->wz_ = new writer_gz;
    //#ifdef HDF5
    m_instance->rh_ = new reader_hdf5;
    m_instance->wh_ = new writer_hdf5;
    //#endif
  }

  mybhep_svc::~mybhep_svc() 
  {
    delete csvc_ ;
    delete rt_;
    delete rz_;
    delete wt_;
    delete wz_;
    //#ifdef HDf5
    delete wh_;
    delete rh_;
    //#endif
  }
  
  void mybhep_svc::clear_event()
  {
    event_.clear();
    csvc_->clear();
  }
}
