/// \file falaise/snemo/db/field_description.hpp
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2025-04-29
 * Last modified : 2025-04-29
 *
 * Copyright (C) 2025 Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef FALAISE_SNEMO_DB_FIELD_DESCRIPTION_HPP
#define FALAISE_SNEMO_DB_FIELD_DESCRIPTION_HPP 1

// Standard library:
#include <string>
#include <map>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

namespace snemo {

  namespace db {

    enum data_type : uint32_t
			{
				UNDEFINED = 0,
				BOOLEAN,
				INT,
				INTEGER = INT,
				REAL,
				BIT,
				FIXED_STR,
				VAR_STR,
				DATE,
				TIME,
				DATETIME,
				JSON
			};

		std::string to_string(const data_type);
		
		data_type data_type_from_string(const std::string & label_);

    /// \brief Description/model of a field
    class field_description
      : datatools::i_tree_dumpable
    {
    public:
      
      field_description(const std::string & name_,
												const data_type type_,
												const uint16_t max_length_,
												const bool unsigned_ = false);

      ~field_description() = default;

      const std::string & name() const;

      data_type type() const;

      bool is_signed() const;

      bool is_unsigned() const;
  
      uint16_t max_length() const;
  
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;
    
    private:

      std::string _name_;
      data_type _type_ = data_type::UNDEFINED;
      uint16_t _max_length_;
      bool _unsigned_ = false; ///< Only for integer type
    
    };
 
    /// \brief Field instance
    struct field_entry
      : datatools::i_tree_dumpable
    {
    public:
      
      field_entry() = delete;
      
      field_entry(const std::string & name_,
									const std::string & text_,
									const field_description & description_,
									const bool primary_key_ = false,
									const bool null_allowed_ = false);

      const std::string & name() const;

      const std::string & text() const;

			bool is_primary_key() const;

			bool is_null_allowed() const;

			bool has_default() const;

      const field_description & description() const;
 
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ = empty_options()) const override;
 
    private:
      
      std::string _name_;
      std::string _text_;
      const field_description & _description_;
			bool _primary_key_ = false;
 			bool _null_allowed_ = false;
      
    };
     
    typedef std::map<std::string, field_entry> field_map_type;
   		
  } // end of namespace db

} // end of namespace snemo

#endif // FALAISE_SNEMO_DB_FIELD_DESCRIPTION_HPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
