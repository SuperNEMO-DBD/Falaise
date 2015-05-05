// snemo/digitization/memory.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H

namespace snemo {
  
  namespace digitization {
		
		template<unsigned int AddressSize, unsigned int DataSize>
		memory<AddressSize, DataSize>::memory()
		{
			_default_data_ = 0;
		}

		template<unsigned int AddressSize, unsigned int DataSize>
		memory<AddressSize, DataSize>::memory(const std::bitset<DataSize> & default_data_)
		{
			_default_data_ = default_data_;
		}

		template<unsigned int AddressSize, unsigned int DataSize>
		void memory<AddressSize, DataSize>::push (const std::bitset<AddressSize> & address_bitset_,
																							std::bitset<DataSize> & data_bitset_)
		{
			_memory_[address_bitset_] = data_bitset_;
			return;
		}
  
		template<unsigned int AddressSize, unsigned int DataSize>
		void memory<AddressSize, DataSize>::fetch (const std::bitset<AddressSize> & address_bitset_,
																							 std::bitset<DataSize> & data_bitset_)
		{
			if (_memory_.find(address_bitset_) == _memory_.end())
				{
					data_bitset_ =  _default_data_;
				}
			else
				{
					data_bitset_ = _memory_[address_bitset_];
				}
			return;
		}

		template<unsigned int AddressSize, unsigned int DataSize>
		const std::bitset<DataSize> & memory<AddressSize, DataSize>::fetch (const std::bitset<AddressSize> & address_bitset_)
		{	
			if (_memory_.find(address_bitset_) == _memory_.end())
				{
					return _default_data_;
				}
			else
				{
					return _memory_[address_bitset_];
				}
		}
  
		template<unsigned int AddressSize, unsigned int DataSize>
		void memory<AddressSize, DataSize>::load_from_file(const std::string & filename_)
		{
			std::string dummy_description;
			load_from_file(filename_, dummy_description);
			return;
		}	

		template<unsigned int AddressSize, unsigned int DataSize>
		void memory<AddressSize, DataSize>::load_from_file(const std::string & filename_, std::string & description_)
		{
			// unsigned int address_size = 0;
			// unsigned int data_size = 0;
			// std::map<std::string, std::bitset<data_size> > registered_values;
			// std::bitset<data_size> default_data = 0;
			// std::string description;
			
			
			

			
			// if (!description.empty())
			// 	{
			// 		description_ = description;
			// 	}
			return;
		}


  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_INL_H
/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
