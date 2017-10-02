// snemo/digitization/memory.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H

// Standard library:
#include <fstream>
#include <sstream>
#include <vector>

// Third party :
// Boost :

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace snemo {

namespace digitization {

template <unsigned int AddressSize, unsigned int DataSize>
std::size_t memory<AddressSize, DataSize>::get_number_of_addresses() const {
  return 0x1 << AddressSize;
}

template <unsigned int AddressSize, unsigned int DataSize>
memory<AddressSize, DataSize>::memory() {
  _default_data_ = 0;
}

template <unsigned int AddressSize, unsigned int DataSize>
memory<AddressSize, DataSize>::memory(const std::bitset<DataSize>& default_data_) {
  _default_data_ = default_data_;
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::push(const std::bitset<AddressSize>& address_bitset_,
                                         const std::bitset<DataSize>& data_bitset_) {
  _memory_[address_bitset_] = data_bitset_;
  return;
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::fetch(const std::bitset<AddressSize>& address_bitset_,
                                          std::bitset<DataSize>& data_bitset_) {
  if (_memory_.find(address_bitset_) == _memory_.end()) {
    data_bitset_ = _default_data_;
  } else {
    data_bitset_ = _memory_[address_bitset_];
  }
  return;
}

template <unsigned int AddressSize, unsigned int DataSize>
const std::bitset<DataSize>& memory<AddressSize, DataSize>::fetch(
    const std::bitset<AddressSize>& address_bitset_) {
  if (_memory_.find(address_bitset_) == _memory_.end()) {
    return _default_data_;
  } else {
    return _memory_[address_bitset_];
  }
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::memory_map_display() {
  typename memory_dict_type::const_iterator itmem;
  std::clog << "DEBUG : Memory map contains :" << std::endl;
  for (itmem = _memory_.begin(); itmem != _memory_.end(); itmem++) {
    std::clog << itmem->first << " => " << itmem->second << std::endl;
  }
  return;
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::store_to_file(const std::string& filename_,
                                                  const std::string& description_) {
  std::ofstream file(filename_.c_str());
  if (file) {
    if (!description_.empty()) {
      file << "#@description = " << description_ << std::endl;
    }
    file << "#@address_size = " << AddressSize << std::endl;
    file << "#@data_size = " << DataSize << std::endl;
    file << "#@default_data = " << _default_data_ << std::endl;
    typename memory_dict_type::const_iterator itmem;
    for (itmem = _memory_.begin(); itmem != _memory_.end(); itmem++) {
      if (itmem->second != _default_data_) {
        file << itmem->first << ' ' << itmem->second << std::endl;
      }
    }
    file.close();
  }
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::load_from_file(const std::string& filename_) {
  std::string dummy_description;
  load_from_file(filename_, dummy_description);
  return;
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::load_from_file(const std::string& filename_,
                                                   std::string& description_) {
  reset();
  unsigned int address_size = 0;
  unsigned int data_size = 0;

  std::map<std::string, std::bitset<DataSize> > registered_values;
  typename std::map<std::string, std::bitset<DataSize> >::iterator it;

  std::bitset<DataSize> default_data = 0;
  std::string default_data_str;
  std::string description;

  std::ifstream fin(filename_.c_str());
  DT_THROW_IF(!fin, std::runtime_error, "Cannot open file '" << filename_ << "'!");
  bool header_done = false;
  while (fin && !fin.eof()) {
    std::string line;
    std::getline(fin, line);
    {
      std::istringstream tmp_iss(line);
      std::string word;
      tmp_iss >> word >> std::ws;
      if (word.empty()) {
        continue;
      } else if (word[0] == '#') {
        if (word.length() > 1) {
          if (word[1] == '@') {
            DT_THROW_IF(header_done, std::logic_error,
                        "Metadatas outside header at line '" << line << "' ! ");

            typedef std::vector<std::string> split_vector_type;
            split_vector_type key_value_vect;
            boost::split(key_value_vect, line, boost::is_any_of("="), boost::token_compress_off);
            DT_THROW_IF(key_value_vect.size() != 2, std::logic_error,
                        "Key = value format error at line '" << line << "' ! ");
            std::string key = key_value_vect[0];
            std::string data = key_value_vect[1];
            boost::trim(key);
            boost::trim(data);

            if (key == "#@address_size") {
              std::istringstream iss(data);
              iss >> address_size;
              DT_THROW_IF(!iss, std::logic_error, "Format error while reading address size ! ");
            } else if (key == "#@data_size") {
              std::istringstream iss(data);
              iss >> data_size;
              DT_THROW_IF(!iss, std::logic_error, "Format error while reading address size ! ");
            } else if (key == "#@description") {
              std::istringstream iss(data);
              std::getline(iss, description);
            } else if (key == "#@registered_value") {
              split_vector_type rv_vect;
              boost::split(rv_vect, data, boost::is_any_of(":"), boost::token_compress_off);
              DT_THROW_IF(rv_vect.size() != 2, std::logic_error,
                          "Key = value format error at line '" << line << "' ! ");
              std::string label = rv_vect[0];
              std::string rv = rv_vect[1];
              boost::trim(label);
              boost::trim(rv);
              std::bitset<DataSize> bitset_data(rv);
              registered_values.insert(
                  std::pair<std::string, std::bitset<DataSize> >(label, bitset_data));
            } else if (key == "#@default_data") {
              default_data_str = data;
            } else {
              DT_THROW(std::logic_error, "Unsupported meta data '" << key << "' ! ");
            }
          }
        } else {
          // Skip comment
        }
        continue;

      }  // end of else if

    }  // end of line

    header_done = true;
    std::istringstream line_iss(line);
    std::bitset<AddressSize> address_bitset;
    std::bitset<DataSize> data_bitset;
    std::string data_string;
    line_iss >> address_bitset >> data_string;
    if (registered_values.find(data_string) != registered_values.end()) {
      data_bitset = registered_values[data_string];
    } else {
      std::istringstream dd_iss(data_string);
      dd_iss >> data_bitset;
      DT_THROW_IF(!dd_iss, std::logic_error,
                  "Invalid format for data bitset '" << data_string << "' ! ");
    }
    if (!default_data_str.empty()) {
      if (registered_values.find(default_data_str) != registered_values.end()) {
        default_data = registered_values[default_data_str];
      } else {
        std::istringstream dd_iss(default_data_str);
        dd_iss >> default_data;
        DT_THROW_IF(!dd_iss, std::logic_error,
                    "Invalid format for default data '" << default_data_str << "' ! ");
      }
    }

    _default_data_ = default_data;
    push(address_bitset, data_bitset);

  }  // end of while

  if (!description.empty()) {
    description_ = description;
  }
  return;
}

template <unsigned int AddressSize, unsigned int DataSize>
void memory<AddressSize, DataSize>::reset() {
  _memory_.clear();
  _default_data_ = 0;
  return;
}

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
