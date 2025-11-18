// Ourselves:
#include <falaise/snemo/db/table_description.hpp>

// Standard library:
#include <iomanip>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace db {

    table_description::table_description(const std::string & name_)
      : _name_(name_)
    {
      return;
    }

    const std::string & table_description::name() const
    {
      return _name_;
    }

    std::set<std::string> table_description::field_names() const
    {
      std::set<std::string> names;
      for(auto it = _fields_.begin(); it != _fields_.end(); ++it) {
	names.insert(it->first);
      }
      return names;
    }

    bool table_description::has_field(const std::string & name_) const
    {
      return _fields_.count(name_);
    }
  
       
    void table_description::add_field(const std::string & name_,
				      const std::string & text_,
				      const field_description & description_,
				      const bool primary_key_,
				      const bool null_allowed_)
    {
      DT_THROW_IF(has_field(name_), std::logic_error,
		  "Already has a field named " << std::quoted(name_));
      _fields_.emplace(std::make_pair(name_, field_entry(name_, text_, description_, primary_key_, null_allowed_)));
      return;
    }
    
    const field_entry & table_description::field(const std::string & name_) const
    {
      return _fields_.find(name_)->second;
    }
 
    const field_map_type & table_description::fields() const
    {
      return _fields_;
    }
    
    /**********************************************************/
    
    table_entry::table_entry(const std::string & name_,
			     const std::string & text_,
			     const table_description & description_)
      : _name_(name_)
      , _text_(text_)
      , _description_(description_)
    {
      return;
    }
   
    const std::string & table_entry::name() const
    {
      return _name_;
    }
   
    const std::string & table_entry::text() const
    {
      return _text_;
    }

    const table_description & table_entry::description() const
    {
      return _description_;
    }
    
    /**********************************************************/

    /// Smart print
    void table_description::print_tree(std::ostream & out_,
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
   
      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Number of fields  : " << _fields_.size() << std::endl;
      {
	auto count = 0u;
	for (const auto & fieldEntryItem : _fields_) {
	  std::string item_tag = i_tree_dumpable::tags::item();
	  std::string item_tag2 = i_tree_dumpable::tags::skip_item();
	  if (count + 1 == _fields_.size() ) {
	    item_tag = i_tree_dumpable::tags::last_item();
	    item_tag2 = i_tree_dumpable::tags::last_skip_item();
	  }
	  outs << popts.indent << i_tree_dumpable::last_skip_tag
	       << item_tag
	       << "Field  : " << std::quoted(fieldEntryItem.first) 
	       << std::endl;
	  {
	    boost::property_tree::ptree fpopts;
	    fpopts.put("indent",
		       popts.indent
		       + i_tree_dumpable::tags::last_skip_item()
		       + item_tag2);
	    fieldEntryItem.second.print_tree(outs, fpopts);
	  }
	  count++;
	}
      }
 
      out_ << outs.str();
      return;
    }

    /**********************************************************/

    /// Smart print
    void table_entry::print_tree(std::ostream & out_,
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
    
      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Description : " << std::quoted(_description_.name()) << std::endl;
      
      out_ << outs.str();
      return;
    }
 
  } // end of namespace db

} // end of namespace snemo
