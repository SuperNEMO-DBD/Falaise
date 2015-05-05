// snemo/digitization/memory.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MEMORY_H

// Standard library :
#include <map>
#include <bitset>

namespace snemo {
  
  namespace digitization {

		/// \brief Template memory class
		template <unsigned int AddressSize, unsigned int DataSize>
		class memory
		{
		public :
			
			/// Default constructor
			memory();
		
			/// Constructor
			memory(const std::bitset<DataSize> & default_data_);

			void push (const std::bitset<AddressSize> & address_bitset_,
								 std::bitset<DataSize> & data_bitset_);
  
			void fetch (const std::bitset<AddressSize> & address_bitset_,
									std::bitset<DataSize> & data_bitset_);
  
			const std::bitset<DataSize> & fetch (const std::bitset<AddressSize> & address_bitset_);

			void load_from_file(const std::string & filename_);

			void load_from_file(const std::string & filename_, std::string & description_);
  
		private :
			std::map<std::bitset<AddressSize>, std::bitset<DataSize> > _memory_;
			std::bitset<DataSize> _default_data_;

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
