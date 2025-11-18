// Ourselves:
#include <falaise/snemo/db/factory.hpp>

// Standard library:
#include <iomanip>

// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

namespace snemo {

  namespace db {

    std::unique_ptr<field_description>
    factory::make_field_description(const std::string & name_,
				    const datatools::properties & config_)
    {
      data_type type = data_type::UNDEFINED;
      uint16_t max_length = 1;
      bool funsigned = false;
      
      DT_THROW_IF(not config_.has_key("type"),
		  std::logic_error,
		  "Missing 'type' properties in field named " << std::quoted(name_));
      std::string typeRepr = config_.fetch_string("type");
      type = data_type_from_string(typeRepr);
      DT_THROW_IF(type == data_type::UNDEFINED, std::logic_error,
		  "Undefined field data type " << std::quoted(typeRepr) << " in field named " << std::quoted(name_));
      
      if (config_.has_key("max_length")) {
	max_length = (uint16_t) config_.fetch_positive_integer("max_length");
      }

      if (config_.has_flag("unsigned")) {
	funsigned = true;
      }
      
      return std::make_unique<field_description>(name_,
						 type,
						 max_length,
						 funsigned);
    }

    std::unique_ptr<table_description>
    factory::make_table_description(const std::string & name_,
				    const datatools::properties & config_)
    {
      auto uPtr = std::make_unique<table_description>(name_);

      std::vector<std::string> fieldNames;
      DT_THROW_IF(not config_.has_key("fields"),
		  std::logic_error,
		  "Missing 'fields' property");
      config_.fetch("fields", fieldNames);

      for (auto i = 0u; i < fieldNames.size(); i++) {
	std::string fieldDescLabel;
	std::string fieldDescModel;
	bool primaryKey = false;
	bool nullAllowed = false; 
	std::string key = "fields." + fieldNames[i] + ".text";
	if (config_.has_key(key)) {
	  fieldDescLabel = config_.fetch_string(key);
	}
	
	key = "fields." + fieldNames[i] + ".primary_key";
	if (config_.has_key(key)) {
	  primaryKey = config_.fetch_boolean(key);
	}
	
	key = "fields." + fieldNames[i] + ".null_allowed";
	if (config_.has_key(key)) {
	  nullAllowed = config_.fetch_boolean(key);
	}
	
	key = "fields." + fieldNames[i] + ".model";
	DT_THROW_IF(not config_.has_key(key),
		    std::logic_error,
		    "Missing " << std::quoted(key) << " property in table named " << std::quoted(name_));
	fieldDescModel = config_.fetch_string(key);

	DT_THROW_IF(not has_field_description(fieldDescModel),
		    std::logic_error,
		    "Unknown field description " << std::quoted(fieldDescModel) << " in table named " << std::quoted(name_));
	const auto & fieldDesc = get_field_description(fieldDescModel);
	uPtr->add_field(fieldNames[i],
			fieldDescLabel,
			fieldDesc,
			primaryKey,
			nullAllowed);
      }

      return uPtr;
    }

    std::unique_ptr<database_description>
    factory::make_database_description(const std::string & name_,
				       const datatools::properties & config_)
    {
      auto uPtr = std::make_unique<database_description>(name_);

      std::vector<std::string> tableNames;
      std::string key("tables");
      DT_THROW_IF(not config_.has_key(key),
		  std::logic_error,
		  "Missing " << std::quoted(key) << " property in database named " << std::quoted(name_));
      config_.fetch(key, tableNames);

      for (auto i = 0u; i < tableNames.size(); i++) {
	std::string tableDescComment;
	std::string tableDescModel;
	
	key = "tables." + tableNames[i] + ".model";
	DT_THROW_IF(not config_.has_key(key),
		    std::logic_error,
		    "Missing " << std::quoted(key) << " property in database named " << std::quoted(name_));
	tableDescModel = config_.fetch_string(key);

	key = "tables." + tableNames[i] + ".comment";
	if (config_.has_key(key)) {
	  tableDescComment = config_.fetch_string(key);
	}

	DT_THROW_IF(not has_table_description(tableDescModel),
		    std::logic_error,
		    "Unknown table description " << std::quoted(tableDescModel) << " in database named " << std::quoted(name_));
	const auto & tableDesc = get_table_description(tableDescModel);
	uPtr->add_table(tableNames[i],
			tableDescComment,
			tableDesc);
      }
     
      return uPtr;
    }

    std::set<std::string> factory::field_description_keys() const
    {
      std::set<std::string> keys;
      for (auto item : _field_descriptions_) {
	keys.insert(item.first);
      }
      return keys;
    }
 
    std::set<std::string> factory::table_description_keys() const
    {
      std::set<std::string> keys;
      for (auto item : _table_descriptions_) {
	keys.insert(item.first);
      }
      return keys;
    }
 
    std::set<std::string> factory::database_description_keys() const
    {
      std::set<std::string> keys;
      for (auto item : _database_descriptions_) {
	keys.insert(item.first);
      }
      return keys;
    }
 
    bool factory::has_field_description(const std::string & name_) const
    {
      return _field_descriptions_.count(name_);
    }

    bool factory::has_table_description(const std::string & name_) const
    {
      return _table_descriptions_.count(name_);
    }

    bool factory::has_database_description(const std::string & name_) const
    {
      return _database_descriptions_.count(name_);
    }

    const field_description &
    factory::get_field_description(const std::string & name_) const
    {
      return *(_field_descriptions_.find(name_)->second.get());
    }
 
    const table_description &
    factory::get_table_description(const std::string & name_) const
    {
      return *(_table_descriptions_.find(name_)->second.get());
    }
 
    const database_description &
    factory::get_database_description(const std::string & name_) const
    {
      return *(_database_descriptions_.find(name_)->second.get());
    }
 
    void factory::build(const datatools::multi_properties & mconfig_)
    {
      auto logging = datatools::logger::PRIO_FATAL;
      // logging = datatools::logger::PRIO_DEBUG;
      std::vector<std::string> orderedKeys = mconfig_.ordered_keys();
      for (auto i = 0u; i < orderedKeys.size(); i++) {
	const auto & descName = orderedKeys[i];
	const auto & descEntry = mconfig_.get(descName);
	const std::string & descMeta = descEntry.get_meta();
	DT_LOG_DEBUG(logging,
		     "descName = " << std::quoted(descName)
		     << "  descMeta = " << std::quoted(descMeta) );
        const datatools::properties & descConfig = descEntry.get_properties();
	if (descMeta == "field_description") {
	  DT_THROW_IF(_field_descriptions_.count(descName),
		      std::logic_error,
		      "Field description name " << std::quoted(descName) << " already exists");
	  std::shared_ptr<field_description> fieldDesc = make_field_description(descName, descConfig);
	  _field_descriptions_.emplace(descName, fieldDesc);
	} else if (descMeta == "table_description") {
	  DT_THROW_IF(_table_descriptions_.count(descName),
		      std::logic_error,
		      "Table description name " << std::quoted(descName) << " already exists");
	  std::shared_ptr<table_description> tableDesc = make_table_description(descName, descConfig);
	  _table_descriptions_.emplace(descName, tableDesc);
	} else if (descMeta == "database_description") {
	  DT_THROW_IF(_database_descriptions_.count(descName),
		      std::logic_error,
		      "Database description name " << std::quoted(descName) << " already exists");
	  std::shared_ptr<database_description> databaseDesc = make_database_description(descName, descConfig);
	  _database_descriptions_.emplace(descName, databaseDesc);
	} else {
	  DT_THROW(std::logic_error, "Invalid meta label ");
	}
      }
      return;
    }

    /// Smart print
    void factory::print_tree(std::ostream & out_,
			     const boost::property_tree::ptree & options_) const
    {
      i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      std::ostringstream outs;
      if (! popts.title.empty ()) {
	outs << popts.indent << popts.title << std::endl;
      }

      outs << popts.indent << i_tree_dumpable::tag
	   << "Number of field descriptions  : " << _field_descriptions_.size() << std::endl;
      {
	auto count = 0u;
	for (const auto & fieldDescItem : _field_descriptions_) {
	  std::string item_tag = i_tree_dumpable::tags::item();
	  std::string item_tag2 = i_tree_dumpable::tags::skip_item();
	  if (count + 1 == _field_descriptions_.size() ) {
	    item_tag = i_tree_dumpable::tags::last_item();
	    item_tag2 = i_tree_dumpable::tags::last_skip_item();
	  }
	  const auto & fieldDescPtr = fieldDescItem.second;
	  outs << popts.indent << i_tree_dumpable::skip_tag
	       << item_tag
	       << "Field description  : " << std::quoted(fieldDescItem.first) 
	       << " (addr=" << fieldDescPtr.get() << ')'
	       << std::endl;
  	  {
	    boost::property_tree::ptree fpopts;
	    fpopts.put("indent",
		       popts.indent
		       + i_tree_dumpable::tags::skip_item()
		       + item_tag2);
	    fieldDescPtr->print_tree(outs, fpopts);
	  }

	  count++;
	}
      }

      outs << popts.indent << i_tree_dumpable::tag
	   << "Number of table descriptions  : " << _table_descriptions_.size() << std::endl;
      {
	auto count = 0u;
	for (const auto & tableDescItem : _table_descriptions_) {
	  std::string item_tag = i_tree_dumpable::tags::item();
	  std::string item_tag2 = i_tree_dumpable::tags::skip_item();
	  if (count + 1 == _table_descriptions_.size() ) {
	    item_tag = i_tree_dumpable::tags::last_item();
	    item_tag2 = i_tree_dumpable::tags::last_skip_item();
	  }
	  const auto & tableDescPtr = tableDescItem.second;
	  outs << popts.indent << i_tree_dumpable::skip_tag
	       << item_tag
	       << "Table description  : " << std::quoted(tableDescItem.first) 
	       << " (addr=" << tableDescPtr.get() << ')'
	       << std::endl;
 	  {
	    boost::property_tree::ptree fpopts;
	    fpopts.put("indent",
		       popts.indent
		       + i_tree_dumpable::tags::skip_item()
		       + item_tag2);
	    tableDescPtr->print_tree(outs, fpopts);
	  }

	  count++;
	}
      }

      outs << popts.indent << i_tree_dumpable::last_tag
	   << "Number of database descriptions  : " << _database_descriptions_.size() << std::endl;
      {
	auto count = 0u;
	for (const auto & dbDescItem : _database_descriptions_) {
	  std::string item_tag = i_tree_dumpable::tags::item();
	  std::string item_tag2 = i_tree_dumpable::tags::skip_item();
	  if (count + 1 == _database_descriptions_.size() ) {
	    item_tag = i_tree_dumpable::tags::last_item();
	    item_tag2 = i_tree_dumpable::tags::last_skip_item();
	  }
	  const auto & dbDescPtr = dbDescItem.second;
	  outs << popts.indent << i_tree_dumpable::last_skip_tag
	       << item_tag
	       << "Database description  : " << std::quoted(dbDescItem.first)
	       << " (addr=" << dbDescPtr.get() << ')'
	       << std::endl;
 	  {
	    boost::property_tree::ptree fpopts;
	    fpopts.put("indent",
		       popts.indent
		       + i_tree_dumpable::tags::last_skip_item()
		       + item_tag2);
	    dbDescPtr->print_tree(outs, fpopts);
	  }

	  count++;
	}
      }
 
      out_ << outs.str();
      return;
    }

  } // end of namespace db

} // end of namespace snemo
