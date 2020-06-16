//! \file testFhiclProperties.cc
//! \brief Tests for making Multi/Properties hierarchical
//! \description Use Boost.PropertyTree as first step
//               because it provides an easy way to automatically
//               make a flat key set hierarchical
//               That will later be used to migrate to FHiCL's
//               pset implementation which requires hierarchical
//               construction.
//
// Copyright (c) 2016 by Ben Morgan <Ben.Morgan@warwick.ac.uk>
// Copyright (c) 2016 by The University of Warwick
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard Library

// Third Party
// - Boost
#include "boost/property_tree/exceptions.hpp"
#include "boost/property_tree/ptree.hpp"

// - Bayeux
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/properties.h"

// This Project

namespace falaise {
namespace metadata {
//! Properties
typedef boost::property_tree::basic_ptree<std::string, datatools::properties::data> properties;

//! Traits for types that can be stored in properties
struct traits {
  // TODO: Support for Paths, Quantities and Arrays
  typedef boost::mpl::vector<
      datatools::properties::data::vbool, datatools::properties::data::vbool::value_type,
      datatools::properties::data::vint, datatools::properties::data::vint::value_type,
      datatools::properties::data::vdouble::value_type, datatools::properties::data::vstring,
      datatools::properties::data::vstring::value_type>
      types;
  template <typename T>
  using is_storable = typename boost::mpl::contains<types, T>::type;
};

//! Exceptions - try to be compatible with other ptree errors
class type_error : public boost::property_tree::ptree_error {
  using boost::property_tree::ptree_error::ptree_error;
};
typedef boost::property_tree::ptree_bad_path key_error;

//! Translator for boost::property_tree.
//! This allows user to put/get data to/from the tree
//! It does not yet support persisting or reading data from file
template <typename Internal, typename External>
struct property_translator {
  //! Typedef for stored type required by boost::property_tree
  typedef Internal internal_type;
  //! Typedef for input/output type required by boost::property_tree
  typedef External external_type;

  //! Extract value from property, converting to external_type
  //! Conversion can only happen if external_type is supported
  boost::optional<external_type> get_value(const internal_type& internal) {
    static_assert(traits::is_storable<external_type>::value, "Cannot get value, invalid type");
    return this->get_value_impl(internal, external_type{});
  }

  //! Store value, converting to internal_type
  //! Conversion can only happen if incoming type is storable in internal_type
  boost::optional<internal_type> put_value(const external_type& external) {
    return internal_type(external);
  }

 private:
  //! Tag-dispatched implementations for each type storable
  //! Has to be done this way because underlying type
  //! (datatools::properties::data) has awkward conventions for checking
  //! held type and extrating value
  boost::optional<external_type> get_value_impl(const internal_type& internal, bool) {
    if (internal.is_boolean() && internal.is_scalar()) {
      return internal.get_boolean_value();
    } else {
      throw type_error("cannot convert value to bool");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal, int) {
    if (internal.is_integer() && internal.is_scalar()) {
      return internal.get_integer_value();
    } else {
      throw type_error("cannot convert value to int");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal, double) {
    if (internal.is_real() && internal.is_scalar()) {
      return internal.get_real_value();
    } else {
      throw type_error("cannot convert value to double");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal, std::string) {
    if (internal.is_string() && internal.is_scalar()) {
      return internal.get_string_value();
    } else {
      throw type_error("cannot convert value to string");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal, std::vector<bool>) {
    if (internal.is_boolean() && internal.is_vector()) {
      size_t size = static_cast<size_t>(internal.size());
      std::vector<bool> tmp;
      tmp.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        tmp.push_back(internal.get_boolean_value(i));
      }
      return tmp;
    } else {
      throw type_error("cannot convert value to vector<bool>");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal, std::vector<int>) {
    if (internal.is_integer() && internal.is_vector()) {
      size_t size = static_cast<size_t>(internal.size());
      std::vector<int> tmp;
      tmp.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        tmp.push_back(internal.get_integer_value(i));
      }
      return tmp;
    } else {
      throw type_error("cannot convert value to vector<int>");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal,
                                                std::vector<double>) {
    if (internal.is_real() && internal.is_vector()) {
      size_t size = static_cast<size_t>(internal.size());
      std::vector<double> tmp;
      tmp.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        tmp.push_back(internal.get_real_value(i));
      }
      return tmp;
    } else {
      throw type_error("cannot convert value to vector<double>");
    }
  }

  boost::optional<external_type> get_value_impl(const internal_type& internal,
                                                std::vector<std::string>) {
    if (internal.is_string() && internal.is_vector()) {
      size_t size = static_cast<size_t>(internal.size());
      std::vector<std::string> tmp;
      tmp.reserve(size);
      for (size_t i = 0; i < size; ++i) {
        tmp.push_back(internal.get_string_value(i));
      }
      return tmp;
    } else {
      throw type_error("cannot convert value to vector<string>");
    }
  }
};

//! Print contents of properties
void tree_dump(std::ostream& oput, const properties& p, int depth = 0) {
  for (const auto& node : p.get_child("")) {
    // Process current node
    oput << std::string("").assign(depth * 2, ' ') << "* ";
    oput << node.first;
    oput << " : ";
    // any data associated with this node
    auto data = node.second.data();
    data.to_string(oput);
    oput << "\n";

    // recurse to subtree
    tree_dump(oput, node.second, depth + 1);
  }
}

//! Convert datatools::properties to falaise::metadata::properties
properties read_datatools(const datatools::properties& input) {
  properties output;

  for (const auto& k : input.keys()) {
    // Use put rather than add, because properties always(??) has unique keys
    output.put(k, input.get(k));
  }

  return output;
}

//! Convert falaise::metadata::properties to datatools::properties
//! This makes one significant assumption in that we *never* have
//! keys in the input tree of the form
//! a : 1
//! a.b : 2
//! i.e. nodes are either leaves or trees, there is a never a node
//! that has data and subtrees (pure hierarchical form)
datatools::properties write_datatools(const properties& input) {
  datatools::properties output;
  for (const auto& node : input) {
    // Handle leaf node
    if (node.second.empty()) {
      output.store(node.first, node.second.data());
    }
    // Get child node(s)
    datatools::properties tmp = write_datatools(node.second);
    tmp.export_all_adding_prefix(output, node.first + ".");
  }

  return output;
}

//! Convert datatools::multi_properties to falaise::metadata::properties
// Multiproperties effectively adds one layer of hierarchy,
// but with explicit attributes (key, meta). So to reproduce, have
// to treat it as a formated tree:
// multiproperties {
//   key_label : "name"
//   meta_label : "meta"
//   sections {
//     key1 {
//       meta : "meta"
//       properties {
//         key1 = foo
//         key2 = bar
//         ,,,
//         }
//     }
//     key2 {
//     ...
//     }
//}
// }
// i.e. it's actually a schema rather than a structure...
// Does mean that we can't trivially translate between representations
// though (but *can* represent multiproperties within properties, at least
// when converted to ptree!)
properties read_datatools(const datatools::multi_properties& input) {
  // Maint tree is single "multiproperties" node
  properties output;
  auto& mpTree = output.put_child("multiproperties", properties{});
  mpTree.put("key_label", input.get_key_label());
  mpTree.put("meta_label", input.get_meta_label());
  auto& sections = mpTree.put_child("sections", properties{});

  for (const auto& entry : input.ordered_entries()) {
    properties opEntry;
    opEntry.put("meta", entry->get_meta());
    opEntry.put_child("properties", falaise::metadata::read_datatools(entry->get_properties()));

    // Section children must be unique, so add to sections tree using put
    sections.put_child(entry->get_key(), opEntry);
  }

  return output;
}

//! Convert falaise::metadata::properties to datatools::multiproperties
// Note that as above, this requires the supply properties to have
// a specific format and key names.
// It will throw path/key errors if any element is missing
// At present, this cannot handle a tree with multiple "multiproperties" chidren
datatools::multi_properties write_datatools_multiproperties(const properties& input) {
  // To restore, must have a ptree with a particular layout.
  // 1. A node named "multiproperties"
  // 2. string leaves "key_label" and "meta_label"
  auto mpTree = input.get_child("multiproperties");
  auto opKeyLabel = mpTree.get<std::string>("key_label");
  auto opMetaLabel = mpTree.get<std::string>("meta_label");

  // Make the MP
  datatools::multi_properties output(opKeyLabel, opMetaLabel);

  // Iterate over sections
  auto sectionTree = mpTree.get_child("sections");

  for (const auto& section : sectionTree) {
    // Out of each subnode, get key, meta value and properties
    auto key = section.first;
    auto meta = section.second.get<std::string>("meta");
    auto props = section.second.get_child("properties");
    datatools::properties tmp = falaise::metadata::write_datatools(props);
    output.add(key, meta, tmp);
  }

  return output;
}

}  // namespace metadata
}  // namespace falaise

// Don't care (yet) about serializing to/from XML/JSON etc,
// but do need to think about get/put for data...
// This is outside above struct as must go into boost:property_tree
// namespace
namespace boost {
namespace property_tree {
template <typename T>
struct translator_between<falaise::metadata::properties::data_type, T> {
  typedef falaise::metadata::property_translator<falaise::metadata::properties::data_type, T> type;
};

// Specialize for self translation of stored type
template <>
struct translator_between<falaise::metadata::properties::data_type,
                          falaise::metadata::properties::data_type> {
  struct type {
    typedef falaise::metadata::properties::data_type internal_type;
    typedef falaise::metadata::properties::data_type external_type;

    boost::optional<falaise::metadata::properties::data_type> get_value(
        const falaise::metadata::properties::data_type& value) {
      return value;
    }

    boost::optional<falaise::metadata::properties::data_type> put_value(
        const falaise::metadata::properties::data_type& value) {
      return falaise::metadata::properties::data_type(value);
    }
  };
};
}  // namespace property_tree
}  // namespace boost

//----------------------------------------------------------------------
// Catch tests
//----------------------------------------------------------------------
#include "catch.hpp"

datatools::properties makeProperties() {
  datatools::properties input;
  input.store("foo", 42);
  input.store("bar", true);
  input.store("baz", "lorem ipsum");
  input.store("bob", 3.14);
  input.store("config", std::vector<std::string>{"a", "b"});
  input.store("a.parameter", 3.14);
  input.store("a.directory", "/a/b/c/");
  input.store("b.parameter", 4.13);
  input.store("b.random.id", "mt19937");
  input.store("b.random.seed", "auto");
  return input;
}

datatools::multi_properties makeMultiProperties() {
  datatools::multi_properties input;
  datatools::properties& server_section = input.add_section("server", "server_type");
  server_section.store_flag("verbose", "Verbose mode");
  server_section.store("max_number_of_connexions", 5, "Maximum number of simultaneous connections");
  server_section.store("port", 3456, "Port number");
  server_section.store("protocol", "UDP", "Current protocol");

  // Add a section for parameters dedicated to an embedded pseuso-random number generator :
  datatools::properties& prng_section = input.add_section("prng", "prng_type");
  prng_section.store("seed", 314159, "initial PRNG seed");
  prng_section.store("buffer_size", 1000, "Size of the buffer for pre-computed random numbers");

  return input;
}

TEST_CASE("properties conversion works", "") {
  auto fixture = makeProperties();
  auto metadata = falaise::metadata::read_datatools(fixture);

  SECTION("Failure conditions") {
    REQUIRE_THROWS_AS(metadata.get<bool>("a.bad.path"), falaise::metadata::key_error);
  }

  SECTION("Integer Extraction") {
    REQUIRE(metadata.get<int>("foo") == 42);
    REQUIRE_THROWS_AS(metadata.get<double>("foo"), falaise::metadata::type_error);
    REQUIRE(metadata.get("foo_opt", 24) == 24);
    metadata.put("foo_opt", 234);
    REQUIRE(metadata.get("foo_opt", 4321) == 234);
  }

  SECTION("Vector Extraction") {
    auto vecString = metadata.get<std::vector<std::string> >("config");
    std::vector<std::string> expected{"a", "b"};
    REQUIRE(vecString == expected);
    std::vector<int> def{1, 2, 3, 4};
    REQUIRE(metadata.get("nonexistant", std::vector<int>{1, 2, 3, 4}) == def);
  }
}

TEST_CASE("properties conversion is reversible", "") {
  auto input = makeProperties();
  auto tmp = falaise::metadata::read_datatools(input);

  SECTION("Back conversion works") { REQUIRE_NOTHROW(falaise::metadata::write_datatools(tmp)); }

  SECTION("Read/Write datatools::properties are Identical", "") {
    auto output = falaise::metadata::write_datatools(tmp);
    // Keys
    REQUIRE(output.keys() == input.keys());

    // Types held at each key
    std::vector<int> inTypes(input.size());
    auto inKeys = input.keys();
    std::transform(inKeys.begin(), inKeys.end(), inTypes.begin(),
                   [&input](std::string& k) { return input.get(k).get_type(); });

    std::vector<int> outTypes(output.size());
    auto outKeys = output.keys();
    std::transform(outKeys.begin(), outKeys.end(), outTypes.begin(),
                   [&output](std::string& k) { return output.get(k).get_type(); });
    REQUIRE(outTypes == inTypes);
  }
}

TEST_CASE("multi_properties conversion works", "") {
  auto fixture = makeMultiProperties();
  falaise::metadata::properties rep = falaise::metadata::read_datatools(fixture);
  falaise::metadata::tree_dump(std::cout, rep);
  auto output = falaise::metadata::write_datatools_multiproperties(rep);
  output.tree_dump(std::cout);
}
