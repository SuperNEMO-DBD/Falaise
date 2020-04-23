/// \file falaise/snemo/datatmodels/event.h
/// \brief Event type and free functions

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_H

#include <string>
#include <stdexcept>
#include <bayeux/datatools/things.h>

namespace snedm {
//! Event record type definition
using event_record = datatools::things;

class existing_product_key : public std::logic_error {
  using std::logic_error::logic_error;
};

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

//! Add instance of T at supplied key in event record unless key exists
/*!
 * \tparam T type of value to be added
 * \param[in] key key for value
 * \param[in] event event record to add the value to
 * \returns reference to created value held at key
 * \throws snedm::existing_product_key if key already exists
 */
template <typename T>
T& addToEvent(std::string const& key, event_record& event) {
  if (event.has(key)) {
    throw existing_product_key{"snedm:event_record '" + event.get_name() + "' already holds product '" + key + "'"};
  }
  return event.add<T>(key);
}

}  // namespace snedm

#endif