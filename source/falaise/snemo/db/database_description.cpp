// Ourselves:
#include <falaise/snemo/db/database_description.hpp>

// Standard library:
#include <iomanip>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace db {

 
    database_description::database_description(const std::string & name_)
      : _name_(name_)
    {
      return;
    }

    const std::string & database_description::name() const
    {
      return _name_;
    }

    std::set<std::string> database_description::table_names() const
    {
      std::set<std::string> names;
      for(auto it = _tables_.begin(); it != _tables_.end(); ++it) {
	names.insert(it->first);
      }
      return names;
    }

    bool database_description::has_table(const std::string & name_) const
    {
      return _tables_.count(name_);
    }
    
    void database_description::add_table(const std::string & name_,
					 const std::string & text_,
					 const table_description & description_)
    {
      DT_THROW_IF(has_table(name_), std::logic_error,
		  "Already has a table named " << std::quoted(name_));
      _tables_.emplace(std::make_pair(name_, table_entry(name_, text_, description_)));
      return;
    }
    
    const table_entry & database_description::table(const std::string & name_) const
    {
      return _tables_.find(name_)->second;
    }
 
    const table_map_type & database_description::tables() const
    {
      return _tables_;
    }

    database_entry::database_entry(const std::string & name_,
				   const std::string & text_,
				   const database_description & description_)
      : _name_(name_)
      , _text_(text_)
      , _description_(description_)
    {
      return;
    }
    
    const std::string & database_entry::name() const
    {
      return _name_;
    }
   
    const std::string & database_entry::text() const
    {
      return _text_;
    }

    const database_description & database_entry::description() const
    {
      return _description_;
    }
  
    /// Smart print
    void database_description::print_tree(std::ostream & out_,
					  const boost::property_tree::ptree & options_) const
    {
      i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      std::ostringstream outs;
      if (! popts.title.empty ()) {
	outs << popts.indent << popts.title << std::endl;
      }
   
      outs << popts.indent << i_tree_dumpable::tag
	   << "Name  : " << std::quoted(_name_) << std::endl;
   
      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Number of tables  : " << _tables_.size() << std::endl;
      {
	auto count = 0u;
	for (const auto & tableEntryItem : _tables_) {
	  std::string item_tag = i_tree_dumpable::tags::item();
	  std::string item_tag2 = i_tree_dumpable::tags::skip_item();
	  if (count + 1 == _tables_.size() ) {
	    item_tag = i_tree_dumpable::tags::last_item();
	    item_tag2 = i_tree_dumpable::tags::last_skip_item();
	  }
	  outs << popts.indent << i_tree_dumpable::last_skip_tag
	       << item_tag
	       << "Table  : " << std::quoted(tableEntryItem.first) 
	       << std::endl;
	  {
	    boost::property_tree::ptree fpopts;
	    fpopts.put("indent",
		       popts.indent
		       + i_tree_dumpable::tags::last_skip_item()
		       + item_tag2);
	    tableEntryItem.second.print_tree(outs, fpopts);
	  }
	  count++;
	}
      }

      out_ << outs.str();
      return;
    }
    
    /**********************************************************/

    /// Smart print
    void database_entry::print_tree(std::ostream & out_,
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
	   << "Description : "
	   << std::quoted(_description_.name())
	// << " (addr=" << &_description_ << ')'
	   << std::endl;
      
      out_ << outs.str();
      return;
    }
  
  } // end of namespace db

} // end of namespace snemo
