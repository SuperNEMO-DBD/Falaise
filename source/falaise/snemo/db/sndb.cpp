// Ourselves:
#include <falaise/snemo/db/sndb.hpp>

// Standard library:
#include <iomanip>

// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace db {
  
    bool sndb::has_database(const std::string & name_) const
    {
      return _databases_.count(name_);
    }
    
    void sndb::add_database(const std::string & name_,
                            const std::string & text_,
                            const database_description & description_)
    {
      DT_THROW_IF(has_database(name_), std::logic_error,
                  "Already has a database named " << std::quoted(name_));
      _databases_.emplace(std::make_pair(name_, database_entry(name_, text_, description_)));
      return;
    }
    
    const database_entry & sndb::database(const std::string & name_) const
    {
      return _databases_.find(name_)->second;
    }
 
    const database_map_type & sndb::databases() const
    {
      return _databases_;
    }

    bool sndb::is_initialized() const
    {
      return _initialized_;
    }
     
    void sndb::initialize(const datatools::properties & config_)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      logging = datatools::logger::PRIO_DEBUG;
      std::vector<std::string> db_descfiles;

      std::string db_descfiles_key = "database_descriptions.files";
      if (config_.has_key(db_descfiles_key)) {
        config_.fetch(db_descfiles_key, db_descfiles);
      }
      // if (db_descfiles.empty()) {
      //        db_descfiles.push_back("@falaise:snemo/demonstrator/db/sndb/1.0/descriptions.conf");
      // }
      
      DT_THROW_IF(db_descfiles.size() == 0, std::logic_error,
                  "Missing " << std::quoted(db_descfiles_key) << " property");
      
      for (auto i = 0u; i < db_descfiles.size(); i++) {
        datatools::fetch_path_with_env(db_descfiles[i]);
        DT_LOG_DEBUG(logging, "Path = " << std::quoted(db_descfiles[i]));
        datatools::multi_properties db_desc;
        uint32_t options =                              \
          datatools::multi_properties::config::RESOLVE_PATH
          | datatools::multi_properties::config::DONT_CLEAR;
        db_desc.read(db_descfiles[i], options);
        _factory_.build(db_desc);
      }

      if (datatools::logger::is_debug(logging)) {
        DT_LOG_DEBUG(logging, "Embedded factory:");
        _factory_.print_tree(std::cerr);
      }
            

      std::vector<std::string> databaseNames;

      // databases : string[2] = "rundb" "calocalib"
      // databases.rundb.comment : string = ""
      // databases.rundb.model   : string = "RunDbDatabaseDesc"
      // databases.calocalib.comment : string = ""
      // databases.calocalib.model   : string = "CalocalibDatabaseDesc"
      DT_THROW_IF(not config_.has_key("databases"),
                  std::logic_error,
                  "Missing 'databases' property");
      config_.fetch("databases", databaseNames);
     
      DT_THROW_IF(databaseNames.size() == 0, std::logic_error,
                  "No database is defined");

      for (auto i = 0u; i < databaseNames.size(); i++) {
        std::string databaseDescText;
        std::string databaseDescName;

        std::string key = "databases." + databaseNames[i] + ".text";
        if (config_.has_key(key)) {
          databaseDescText = config_.fetch_string(key);
        }
        
        key = "databases." + databaseNames[i] + ".model";
        DT_THROW_IF(not config_.has_key(key),
                    std::logic_error,
                    "Missing " << std::quoted(key) << " property");
        databaseDescName = config_.fetch_string(key);
        DT_THROW_IF(not _factory_.has_database_description(databaseDescName),
                    std::logic_error,
                    "No registered database description named "
                    << std::quoted(databaseDescName)); 
        const auto & databaseDesc = _factory_.get_database_description(databaseDescName);
        DT_LOG_DEBUG(logging, "databaseDesc: addr=" << &databaseDesc);
        add_database(databaseNames[i],
                     databaseDescText,
                     databaseDesc);
      }
      DT_LOG_DEBUG(logging, "Number of databases = " << databases().size());
     
      _initialized_ = true;
      return;
    }

    void sndb::reset()
    {
      _initialized_ = false;
      
      _databases_.clear();
      return;
    }

    /// Smart print
    void sndb::print_tree(std::ostream & out_,
                          const boost::property_tree::ptree & options_) const
    {
      i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      std::ostringstream outs;
      if (! popts.title.empty ()) {
        outs << popts.indent << popts.title << std::endl;
      }

      outs << popts.indent << i_tree_dumpable::tag
           << "Embedded description factory  : " << std::endl;

      {
        boost::property_tree::ptree fpopts;
        fpopts.put("indent", popts.indent + i_tree_dumpable::tags::skip_item());
        _factory_.print_tree(outs, fpopts);
      }

      outs << popts.indent << i_tree_dumpable::last_tag
           << "Number of databases  : " << _databases_.size() << std::endl;
      {
        auto count = 0u;
        for (const auto & dbItem : _databases_) {
          std::string item_tag  = i_tree_dumpable::tags::item();
          std::string item_tag2 = i_tree_dumpable::tags::skip_item();
          if (count + 1 == _databases_.size() ) {
            item_tag = i_tree_dumpable::tags::last_item();
            item_tag2 = i_tree_dumpable::tags::last_skip_item();
          }
          
          outs << popts.indent << i_tree_dumpable::last_skip_tag
               << item_tag
               << "Database " << std::quoted(dbItem.first) << " :"
               << std::endl;
          {
            const auto & dbEntry = dbItem.second;
            boost::property_tree::ptree fpopts;
            fpopts.put("indent",
                       popts.indent
                       + i_tree_dumpable::tags::last_skip_item()
                       + item_tag2);
            dbEntry.print_tree(outs, fpopts);
          }
          
          count++;
        }
      }
 
      out_ << outs.str();
      return;
    }

  } // end of namespace db

} // end of namespace snemo
