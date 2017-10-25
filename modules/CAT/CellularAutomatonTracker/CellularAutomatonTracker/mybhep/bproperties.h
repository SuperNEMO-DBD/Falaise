/* -*- mode: c++ -*- */
/*
 *
 * Copyright (C) 2004 J.J. Gomez-Cadenas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef __IPROPERTIES_H__
#define __IPROPERTIES_H__

#include <string>
#include <vector>
#include <mybhep/store.h>

namespace mybhep {

//! class bproperties
/*!
  Class bproperties is a base class used by all objects with properties
  in mybhep
*/

class bproperties {
 protected:
  sstore prop_;

 protected:
  //! do not allow copy constructor
  bproperties(const bproperties&);

  //! do not allow assign operator
  bproperties& operator=(const bproperties&);

  //! do not allow constructor, class is not directly invoked
  bproperties(){};

 public:
  virtual ~bproperties(){};

  //! add a properties as a sstore
  void add_properties_store(sstore st) {
    std::vector<std::string> pnames = st.names();
    std::vector<std::string> pvalues = st.items();

    for (size_t i = 0; i < pnames.size(); i++) {
      add_property(pnames[i], pvalues[i]);
    }
  }

  //! add a property
  void add_property(std::string name, std::string value) { prop_.store(name, value); }
  //! find a property (true if property found)
  bool find_property(std::string name) const { return prop_.find(name); }

  //! fetch a property
  std::string fetch_property(std::string name) const {
    std::string pr = prop_.fetch(name);
    return pr;
  }

  //! change a property
  void change_property(std::string name, std::string prop) { prop_[name] = prop; }

  //! return properties as a sstore
  const sstore properties_store() const { return prop_; }

  //! return properties as a map
  const std::map<std::string, std::string>& properties_map() const { return prop_.store_map(); }

  //! return properties name
  const std::vector<std::string> properties() const { return prop_.names(); }

  //! return properties name
  const std::vector<std::string> properties_values() { return prop_.items(); }

  //! reset properties
  void clear() { prop_.clear(); }
};
}  // namespace mybhep
#endif
