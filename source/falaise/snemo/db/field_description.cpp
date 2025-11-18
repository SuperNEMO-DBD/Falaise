// Ourselves:
#include <falaise/snemo/db/field_description.hpp>

// Standard library:
#include <iomanip>

namespace snemo {

  namespace db {

    std::string to_string(const data_type type_)
    {
      if (type_ == data_type::BOOLEAN) return "boolean";
      if (type_ == data_type::INTEGER) return "integer";
      if (type_ == data_type::REAL) return "real";
      if (type_ == data_type::BIT) return "bitset";
      if (type_ == data_type::FIXED_STR) return "fixed_str";
      if (type_ == data_type::VAR_STR) return "var_str";
      if (type_ == data_type::DATE) return "date";
      if (type_ == data_type::TIME) return "time";
      if (type_ == data_type::DATETIME) return "datetime";
      if (type_ == data_type::JSON) return "json";
      return "undefined";
    }
	
    data_type data_type_from_string(const std::string & label_)
    {
      data_type t = data_type::UNDEFINED;
      if (label_ == "boolean") {
	t = data_type::BOOLEAN;
      } else if (label_ == "int" or label_ == "integer") {
	t = data_type::INTEGER;
      } else if (label_ == "real" or label_ == "double") {
	t = data_type::REAL;
      } else if (label_ == "bit" or label_ == "bitset") {
	t = data_type::BIT;
      } else if (label_ == "fixed_str") {
	t = data_type::FIXED_STR;
      } else if (label_ == "var_str") {
	t = data_type::VAR_STR;
      } else if (label_ == "date") {
	t = data_type::DATE;
      } else if (label_ == "time") {
	t = data_type::TIME;
      } else if (label_ == "datetime") {
	t = data_type::DATETIME;
      } else if (label_ == "json") {
	t = data_type::JSON;
      }
      return t;
    }

    field_description::field_description(const std::string & name_,
					 const data_type type_,
					 const uint16_t max_length_,
					 const bool unsigned_)
      : _name_(name_)
      , _type_(type_)
      , _max_length_(max_length_)
      , _unsigned_(unsigned_)
     {
      return;
    }

    const std::string & field_description::name() const
    {
      return _name_;
    }

    data_type field_description::type() const
    {
      return _type_;
    }
   
    uint16_t field_description::max_length() const
    {
      return _max_length_;
    }
    
    bool field_description::is_signed() const
    {
      return not _unsigned_;
    }
    
    bool field_description::is_unsigned() const
    {
      return _unsigned_;
    }
 
    field_entry::field_entry(const std::string & name_,
			     const std::string & text_,
			     const field_description & description_,
			     const bool primary_key_,
			     const bool null_allowed_)
      : _name_(name_)
      , _text_(text_)
      , _description_(description_)
      , _primary_key_(primary_key_)
      , _null_allowed_(null_allowed_)
    {
      return;
    }
   
    const std::string & field_entry::name() const
    {
      return _name_;
    }
   
    const std::string & field_entry::text() const
    {
      return _text_;
    }

    bool field_entry::is_null_allowed() const
    {
      return _null_allowed_;
    }

    bool field_entry::is_primary_key() const
    {
      return _primary_key_;
    }

    bool field_entry::has_default() const
    {
      return false;
    }

    const field_description & field_entry::description() const
    {
      return _description_;
    }
     
    /**********************************************************/

    /// Smart print
    void field_description::print_tree(std::ostream & out_,
				       const boost::property_tree::ptree & options_) const
    {
      i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      std::ostringstream outs;
      if (! popts.title.empty ()) {
	outs << popts.indent << popts.title << std::endl;
      }
  
      outs << popts.indent << i_tree_dumpable::tag
	   << "Name : " << std::quoted(_name_) << std::endl;
  
      outs << popts.indent << i_tree_dumpable::tag
	   << "Type : " << std::quoted(to_string(_type_)) << std::endl;

      outs << popts.indent << i_tree_dumpable::tag
	   << "Max length : " << _max_length_ << std::endl;
 
      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Unsigned : " << std::boolalpha << _unsigned_ << std::endl;

      out_ << outs.str();
      return;
    }
 
    /**********************************************************/

    /// Smart print
    void field_entry::print_tree(std::ostream & out_,
				 const boost::property_tree::ptree & options_) const
    {
      i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      std::ostringstream outs;
      if (! popts.title.empty ()) {
	outs << popts.indent << popts.title << std::endl;
      }
   
      outs << popts.indent << i_tree_dumpable::tag
	   << "Name : " << std::quoted(_name_) << std::endl;
   
      outs << popts.indent << i_tree_dumpable::tag
	   << "Text : " << std::quoted(_text_) << std::endl;
   
      outs << popts.indent << i_tree_dumpable::tag
	   << "Null allowed : " << std::boolalpha << _null_allowed_ << std::endl;
  
      outs << popts.indent << i_tree_dumpable::tag
	   << "Primary key : " << std::boolalpha << _primary_key_ << std::endl;
   
      outs << popts.indent << i_tree_dumpable::tag
	   << "Has default : " << std::boolalpha << has_default() << std::endl;
    
      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Description : " << std::quoted(_description_.name()) << std::endl;
      
      out_ << outs.str();
      return;
    }
 
  } // end of namespace db

} // end of namespace snemo
