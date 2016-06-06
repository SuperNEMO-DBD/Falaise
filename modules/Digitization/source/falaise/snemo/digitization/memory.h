// snemo/digitization/memory.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_H

// Standard library :
#include <map>
#include <bitset>
#include <string>

namespace snemo {
  
  namespace digitization {

		/// Comparison operator between two bitsets
		template<unsigned int Size>
		struct bitset_compare {
			bool operator()(const std::bitset<Size> & bs1_, const std::bitset<Size> & bs2_) const
			{
				return bs1_.to_ulong() < bs2_.to_ulong();
			}
		};

		template <unsigned int AddressSize, unsigned int DataSize>

		/// \brief Template memory class
		class memory
		{
		public :

      typedef std::bitset<AddressSize> address_type;
      typedef std::bitset<DataSize>    data_type;

			/// Default constructor
			memory();
		
			/// Constructor
			memory(const std::bitset<DataSize> & default_data_);
			
			/// Add a couple of address bitset & data bitset into the memory map
			void push(const std::bitset<AddressSize> & address_bitset_,
								const std::bitset<DataSize> & data_bitset_);
  
			/// Fetch the data bitset corresponding to an address bitset
			void fetch(const std::bitset<AddressSize> & address_bitset_,
								 std::bitset<DataSize> & data_bitset_);
  
			/// Return the data bitset corresponding to an address bitset
			const std::bitset<DataSize> & fetch(const std::bitset<AddressSize> & address_bitset_);

			/// Display the key and value of the memory map
			void memory_map_display();

			/// Store the memory map into a file
			void store_to_file(const std::string & filename_, const std::string & description_ = "");

			/// Load from a file a memory and store it in the memory map
			void load_from_file(const std::string & filename_);

			/// Load from a file a memory and store it in the memory map with description
			void load_from_file(const std::string & filename_, std::string & description_);
			
			/// Reset
			void reset();
  
			/// Return the number of addresses
      inline std::size_t get_number_of_addresses() const;

		private :
			
			typedef std::map<std::bitset<AddressSize>, std::bitset<DataSize>, bitset_compare<AddressSize> > memory_dict_type;
			memory_dict_type _memory_; //!< Memory map of bitset, key = address bitset, value = data bitset
			std::bitset<DataSize> _default_data_; //!< Default bitset data

		};

  } // end of namespace digitization

} // end of namespace snemo

#include <snemo/digitization/memory-inl.h>

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_H
/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
