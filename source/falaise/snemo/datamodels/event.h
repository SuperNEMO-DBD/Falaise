/// \file falaise/snemo/datamodels/event.h
/// \brief Event type and free functions

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_H

#include <bayeux/datatools/things.h>

#include <stdexcept>
#include <string>
#include <type_traits>

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
  static_assert(std::is_base_of<datatools::i_serializable, T>::value,
                "snedm::event_record can only store types derived from datatools::i_serializable");
  if (event.has(key)) {
    return event.grab<T>(key);
  }
  return event.add<T>(key);
}

//! Add instance of T at supplied key in event record unless key exists
/*!
 * \tparam T type of value to be added
 * \param[in] key key for value
 * \param[in] event snedm::event_record to add the value to
 * \returns reference to created value held at key
 * \throws snedm::existing_product_key if key already exists
 */
template <typename T>
T& addToEvent(std::string const& key, event_record& event) {
  static_assert(std::is_base_of<datatools::i_serializable, T>::value,
                "snedm::event_record can only store types derived from datatools::i_serializable");
  if (event.has(key)) {
    throw existing_product_key{"snedm:event_record '" + event.get_name() +
                               "' already holds product '" + key + "'"};
  }
  // add will throw if the product exists, but check
  // explicitly to raise a clearer exception
  return event.add<T>(key);
}

}  // namespace snedm

#endif