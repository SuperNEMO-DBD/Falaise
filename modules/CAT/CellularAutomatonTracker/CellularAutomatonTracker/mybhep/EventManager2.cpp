#include <mybhep/EventManager2.h>

namespace mybhep {

using namespace std;

//*************************************************************
void EventManager2::writeGlobalProperties() {
  //*************************************************************

  m.message("+++ writeGlobalPorperties function +++", mybhep::VVERBOSE);

  // reset previous header

  header.clear();

  // copy run and dst properties to global_store

  header.add_properties_store(dst_properties);

  header.add_properties_store(run_properties);

  // write dst header as event 0

  if (oext == "gz") wgz.write(header, oevent);
#ifdef HDF5
  else if (oext == "h5")
    wh5.write(header, oevent);
#endif
  else
    wtxt.write(header, oevent);

  // header already saved

  header_saved = true;

  // run properties also saved,
  // so do not allow to change them anymore

  run_properties_saved = true;

  m.message("++ Header saved with properties:", header.properties_store(), mybhep::VERBOSE);

  oevent += 1;
  toevents += 1;
}

//*************************************************************
mybhep::sstore EventManager2::get_global_properties() {
  //*************************************************************

  mybhep::event global_store;

  global_store.add_properties_store(dst_properties);

  global_store.add_properties_store(run_properties);

  return global_store.properties_store();
}

//*************************************************************
bool EventManager2::readGlobalProperties() {
  //*************************************************************

  /*

   */

  m.message("+++ readGlobalPorperties function +++", mybhep::VVERBOSE);

  // read event zero with global parameters

  mybhep::sstore new_header = read(0).properties_store();

  // new properties not saved

  header_saved = false;

  m.message("++ Event 0 read", mybhep::VVERBOSE);

  // check if properties in dst have changed

  bool ok = checkNewProperties(new_header);

  if (ok) userException();

  // reset previous dst properties

  dst_properties.clear();

  // copy new dst properties

  getDstProperties(new_header);

  m.message("++ Dst global properties:", dst_properties, mybhep::VERBOSE);

  return true;
}

//*******************************************************************
bool EventManager2::checkNewProperties(mybhep::sstore newstore) {
  //*******************************************************************

  /*
    Check if new dst have same properties as previous one
   */

  bool ok = false;

  for (size_t ikey = 0; ikey < dst_properties.size(); ikey++) {
    std::string okey = dst_properties.names()[ikey];
    std::string ovalue = dst_properties.fetch(okey);

    if (!newstore.find(okey)) {  // property not in new dst

      ok = true;
      m.message("\n+++ WARNING from EventManager!!! +++", mybhep::MUTE);
      m.message("++ Missing property in new file: ", okey, mybhep::MUTE);
      m.message("\n", mybhep::MUTE);

    } else {
      std::string nvalue = newstore[okey];

      if (nvalue == ovalue) continue;

      // property has different value

      ok = true;
      m.message("\n+++ WARNING from EventManager!!! +++", mybhep::MUTE);
      m.message("++ DST property changed its value ++", mybhep::MUTE);
      m.message(okey, " = ", ovalue, " --> ", okey, " = ", nvalue, mybhep::MUTE);
      m.message("\n", mybhep::MUTE);
    }
  }  // end loop over old properties

  return ok;
}

//*******************************************************************
void EventManager2::getDstProperties(mybhep::sstore storage) {
  //*******************************************************************

  for (size_t ikey = 0; ikey < storage.size(); ikey++) {
    std::string key = storage.names()[ikey];

    add_dst_property(key, storage.fetch(key));
  }
}

//*******************************************************************
bool EventManager2::add_dst_property(std::string name, std::string value) {
  //*******************************************************************

  bool ok;

  if (!header_saved) {
    if (find_run_property(name)) usedProperty(name);

    ok = true;
    dst_properties.store(name, value);

    m.message("++ Property added to DST:", name, "=", value, mybhep::VERBOSE);

  }

  else {
    ok = false;

    m.message("\n+++ WARNING from EventManager!!! +++", mybhep::MUTE);

    m.message("++ Could not add dst property: ", name, mybhep::MUTE);

    m.message("+ header already written in file!\n", mybhep::MUTE);

    userException();
  }

  return ok;
}

//*******************************************************************
bool EventManager2::add_run_property(std::string name, std::string value) {
  //*******************************************************************

  bool ok;

  if (!run_properties_saved) {
    if (find_dst_property(name)) usedProperty(name);

    ok = true;
    run_properties.store(name, value);

    m.message("++ Run property added:", name, "=", value, mybhep::VERBOSE);

  }

  else {
    ok = false;

    m.message("\n+++ WARNING from EventManager!!! +++", mybhep::MUTE);

    m.message("++ Could not add run property: ", name, mybhep::MUTE);

    m.message("+ Run has already started \n", mybhep::MUTE);

    userException();
  }

  return ok;
}

//*******************************************************************
bool EventManager2::add_run_properties(mybhep::sstore storage, std::string str) {
  //*******************************************************************

  bool ok = true, ok2;

  for (size_t ikey = 0; ikey < storage.size(); ikey++) {
    std::string key = storage.names()[ikey];
    std::string value = storage.fetch(key);

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  return ok;
}

//*******************************************************************
bool EventManager2::add_run_properties(mybhep::gstore storage, std::string str) {
  //*******************************************************************

  bool ok = true, ok2;

  for (size_t i = 0; i < storage.names_istore().size(); i++) {
    std::string key = storage.names_istore()[i];

    std::string value = mybhep::to_string(storage.fetch_istore(key));

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  for (size_t i = 0; i < storage.names_dstore().size(); i++) {
    std::string key = storage.names_dstore()[i];

    std::string value = mybhep::to_string(storage.fetch_dstore(key));

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  for (size_t i = 0; i < storage.names_sstore().size(); i++) {
    std::string key = storage.names_sstore()[i];

    std::string value = storage.fetch_sstore(key);

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  for (size_t i = 0; i < storage.names_ivstore().size(); i++) {
    std::string key = storage.names_ivstore()[i];

    std::string value = mybhep::vector_to_string(storage.fetch_ivstore(key));

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  for (size_t i = 0; i < storage.names_vstore().size(); i++) {
    std::string key = storage.names_vstore()[i];

    std::string value = mybhep::vector_to_string(storage.fetch_vstore(key));

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  for (size_t i = 0; i < storage.names_svstore().size(); i++) {
    std::string key = storage.names_svstore()[i];

    std::string value = mybhep::vector_to_string(storage.fetch_svstore(key));

    ok2 = add_run_property(str + key, value);

    ok = (ok && ok2);
  }

  return ok;
}

//*******************************************************************
void EventManager2::usedProperty(std::string name) {
  //*******************************************************************

  /*
    Abort program if a property is found (added)
    in (to) both run and dst store.
   */

  m.message("\n+++ ERROR from EventManager!!! +++", mybhep::MUTE);
  m.message("++ Property ", name, " not added", mybhep::MUTE);
  m.message("+ Property ", name, " already in store\n", mybhep::MUTE);

  exit(1);
}

//*******************************************************************
bool EventManager2::change_dst_property(std::string name, std::string prop) {
  //*******************************************************************

  bool ok;

  if (!header_saved) {  // first event not written yet

    ok = true;
    dst_properties.sstore(name, prop);

    m.message("++ Property changed in DST:", name, "=", prop, mybhep::VERBOSE);
  } else {
    ok = false;

    m.message("\n+++ WARNING from EventManager!!! +++", mybhep::MUTE);

    m.message("++ Could not change dst property: ", name, mybhep::MUTE);

    m.message("+ header already written in file!\n", mybhep::MUTE);

    userException();
  }

  return ok;
}

//*******************************************************************
std::string EventManager2::fetch_global_property(std::string name) {
  //*******************************************************************

  if (find_dst_property(name))
    return fetch_dst_property(name);

  else
    return fetch_run_property(name);
}

//*******************************************************************
bool EventManager2::find_global_property(std::string name) {
  //*******************************************************************

  bool inrun = find_run_property(name);

  bool indst = find_dst_property(name);

  if (inrun && indst) usedProperty(name);

  return (inrun || indst);
}

//*******************************************************************
bool EventManager2::find_dst_property(std::string name) {
  //*******************************************************************
  return dst_properties.find(name);
}

//*******************************************************************
std::string EventManager2::fetch_dst_property(std::string name) {
  //*******************************************************************
  return dst_properties.fetch(name);
}

//*******************************************************************
bool EventManager2::find_run_property(std::string name) {
  //*******************************************************************
  return run_properties.find(name);
}

//*******************************************************************
std::string EventManager2::fetch_run_property(std::string name) {
  //*******************************************************************
  return run_properties.fetch(name);
}

//*******************************************************************
void EventManager2::userException() {
  //*******************************************************************

  if (!warnings) {
    m.message("++ Warnings are not allowed", mybhep::MUTE);
    m.message("++ Abort as user requests ++\n", mybhep::MUTE);
    exit(1);
  }
}

//*************************************************************
bool EventManager2::isNewFile() {
  //*************************************************************
  return (ievent == 1);
}

}  // namespace mybhep
