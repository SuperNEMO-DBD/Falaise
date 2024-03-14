/// \file falaise/snemo/datamodels/event.h
/// \brief Event type and free functions

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_H

#include <bayeux/datatools/things.h>

#include <stdexcept>
#include <string>
#include <type_traits>

namespace snedm {
  
  //! Class modelling Falaise's top level event data model
  /*!
   * A key-value store with keys being `std::string` and values being
   * any type inheriting from Bayeux's datatools:i_serializable base class.
   *
   * It is just a clarity typedef to Bayeux's underlying datatools::things class,
   * which should be consulted for the full interface.
   * \sa datatools::things
   */
  using event_record = datatools::things;

  class existing_product_key
    : public std::logic_error
  {
    using std::logic_error::logic_error;
  };

  //! Get or add instance of T at supplied key in input event_record
  /*!
   * A simple convenience wrapper around a typical use case in Falaise
   * modules: retrieve a non-const reference to the value of type T stored
   * at "key" in the event_record, inserting a new value of that type at "key"
   * if "key is not present. In code this results in the boilerplate:
   *
   * ```cpp
   * // Usually in the "process" member function of a Falaise module
   * SomeDataType* x{nullptr};
   *
   * if(event.has(key)) {
   *   x = event.grab<SomeDataType>(key); // throws if key does not hold a value of type T
   * }
   * else {
   *   x = event.add<SomeDataType>(key);
   * }
   *
   * // do something with x
   * ```
   *
   * This wrapper function reduces the above to:
   *
   * ```
   * auto x = getOrAddToEvent<SomeDataType>(key, event);
   * ```
   *
   * It additionally checks at compile time that the supplied type (`SomeDataType` here)
   * meets the requirements to be stored in the event.
   *
   * \tparam T type of value to be retrieved or added
   * \param[in] key key for value to find
   * \param[in] event event_record to search in
   * \returns reference to value held at key
   * \throws datatools::bad_things_cast if value at key is not of type T
   */
  template <typename T>
  T & getOrAddToEvent(std::string const & key, event_record & event)
  {
    static_assert(std::is_base_of<datatools::i_serializable, T>::value,
		  "snedm::event_record can only store types derived from datatools::i_serializable");
    if (event.has(key)) {
      return event.grab<T>(key);
    }
    return event.add<T>(key);
  }

  //! Add instance of T at supplied key in event_record unless key exists
  /*!
   * A simple convenience wrapper around a typical use case in Falaise modules:
   * add a new value to the event at "key" only if the "key" is not already in use
   *
   * ```cpp
   * // Usually in the "process" member function of a Falaise module
   * auto x = addToEvent<SomeDataType>("mydata",event);
   *
   * // use x
   * ```
   *
   * Whilst the event_record type has an `add` member function of its own, it is wrapped
   * here to allow:
   *
   * - A clearer existing_product_key exception to be thrown if the key already exists
   * - A compile time check that the type of data being stored meets the requirements to
   *   be stored in the event
   *
   * \tparam T type of value to be added
   * \param[in] key key for value
   * \param[in] event snedm::event_record to add the value to
   * \returns reference to created value held at key
   * \throws snedm::existing_product_key if key already exists
   */
  template <typename T>
  T & addToEvent(std::string const & key, event_record & event)
  {
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

} // namespace snedm

#endif // FALAISE_SNEMO_DATAMODELS_EVENT_H
