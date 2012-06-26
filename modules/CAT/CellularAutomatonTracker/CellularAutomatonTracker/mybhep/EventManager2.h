/* -*- mode: c++ -*- */
#ifndef _event_man2___
#define _event_man2___

#include <mybhep/EventManager.h>

namespace mybhep{
class EventManager2: public EventManager{

public:

  EventManager2(const mybhep::gstore& gs,
		mybhep::prlevel vl=mybhep::NORMAL):
    EventManager(gs,vl){
    header_saved=false;
    run_properties_saved=false;
  }

  EventManager2(const mybhep::gstore& data_store,
		const mybhep::gstore& param_store,
		mybhep::prlevel vl=mybhep::NORMAL):
    EventManager(data_store,vl){
    header_saved=false;
    run_properties_saved=false;
    add_run_properties(param_store);
  }

  ~EventManager2(){}

  bool isNewFile();

  //functions for run properties
  bool add_run_properties (mybhep::sstore,std::string str="");
  bool add_run_properties (mybhep::gstore,std::string str="");
  bool add_run_property (std::string name, std::string value);
  bool find_run_property (std::string name);
  std::string fetch_run_property (std::string name);

  //functions for dst properties
  bool add_dst_property (std::string name, std::string value);
  bool change_dst_property (std::string name, std::string prop);
  bool find_dst_property (std::string name);
  std::string fetch_dst_property (std::string name);

  // functions for both run and dst properties
  bool find_global_property (std::string name);
  std::string fetch_global_property (std::string name);

  // return run properties as a store
  mybhep::sstore get_run_properties(){
    return run_properties;
  };

  // return dst properties as a store
  mybhep::sstore get_dst_properties(){
    return dst_properties;
  };

  // return global properties as a store
  mybhep::sstore get_global_properties();


protected:

  void userException();
  bool readGlobalProperties();
  void writeGlobalProperties();
  bool checkNewProperties(mybhep::sstore);
  void getDstProperties(mybhep::sstore);
  void usedProperty(std::string);

protected:

  //store for the current idst properties
  mybhep::sstore dst_properties;

  //store for the run properties.
  mybhep::sstore run_properties;

  //event to save global properties (idst+run)
  mybhep::event header;

  //bools

  bool header_saved;
  bool run_properties_saved;

};
}

#endif
