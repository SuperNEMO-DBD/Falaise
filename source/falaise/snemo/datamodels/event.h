/// \file falaise/snemo/datatmodels/event.h
/// \brief Event type and free functions

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_H

#include <string>

#include <bayeux/datatools/things.h>

namespace snemo {
namespace datamodel {
//! Event record type definition
using event_record = datatools::things;

//! Get or add instance of T at supplied key in event record
  /*!
   * \tparam T type of value to be retrieved or added
   * \param[in] key key for value to find
   * \param[in] event event record to search in
   * \returns reference to value held at key
   * \throws datatools::bad_things_cast if value at key is not of type T
   */
template <typename T>
T& getOrAddToEvent(std::string const& key, event_record& event) {
  if (event.has(key)) {
    return event.grab<T>(key);
  }
  return event.add<T>(key);
}

}  // namespace datamodels
}  // namespace snemo

#endif