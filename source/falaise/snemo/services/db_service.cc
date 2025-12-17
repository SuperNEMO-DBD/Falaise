// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/db_service.cc */

// Ourselves:
#include <falaise/snemo/services/db_service.h>

// Standard library:
#include <iomanip>
#include <filesystem>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// This project:
#include <falaise/snemo/services/services.h>

// - Bayeux:
#include <bayeux/datatools/utils.h>

// MariaDB C++:
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
#include <mariadb/conncpp.hpp>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// Ourselves:
#include <falaise/snemo/db/database_description.hpp>
#include <falaise/snemo/db/sndb.hpp>
#include <falaise/snemo/db/mariadb_utils.hpp>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::db_service, "snemo::db_service")

namespace snemo {

  struct sndb_connection_info
  {
    std::string host;
    int port{0};
    std::string database{"nemo_rundb"};
    std::string user{"nemo_ro"};
    std::string password{""};
  };

  struct db_service::pimpl_type
  {
    pimpl_type(db_service & master_);
    ~pimpl_type();
    void connect();
    void disconnect();
    bool is_connected() const;

    // Working data structures:
    db_service & master;
    sndb_connection_info connInfo;
    snemo::db::sndb sndbDesc;
    const snemo::db::database_description * dbDesc = nullptr;
    sql::Driver * driver = nullptr;
    std::unique_ptr<sql::Connection> conn;
    std::set<std::string> tablenames;
    
  };

  db_service::pimpl_type::pimpl_type(db_service & master_)
    : master(master_)
  {
    driver = sql::mariadb::get_driver_instance();
    return;
  }

  db_service::pimpl_type::~pimpl_type()
  {
    if (is_connected()) {
      disconnect();
    }
    driver = nullptr;
    return;
  }

  bool db_service::pimpl_type::is_connected() const
  {
    return conn.get() != nullptr;
  }
     
  void db_service::pimpl_type::connect()
  {
    if (is_connected()) {
      disconnect();
    }
    connInfo.host = master.config().db_host;
    connInfo.port = master.config().db_port;
    connInfo.database = master.config().db_database;
    connInfo.user = master.config().db_user;
    connInfo.password = master.config().db_password;
    DT_LOG_DEBUG(master.get_logging_priority(), "connInfo.host=" << connInfo.host);
    DT_LOG_DEBUG(master.get_logging_priority(), "connInfo.port=" << connInfo.port);
    DT_LOG_DEBUG(master.get_logging_priority(), "connInfo.database=" << connInfo.database);
    DT_LOG_DEBUG(master.get_logging_priority(), "connInfo.user=" << connInfo.user);
    DT_LOG_DEBUG(master.get_logging_priority(), "connInfo.password=" << "*********");

    std::ostringstream urlSs;
    urlSs << "jdbc:mariadb://" << connInfo.host << ':' << connInfo.port << '/' << connInfo.database;
    sql::SQLString url(urlSs.str().c_str());
    sql::ConnectOptionsMap connection_properties;
    connection_properties["hostName"] = url;
    connection_properties["userName"] = connInfo.user;
    connection_properties["password"] = connInfo.password;
    connection_properties["useTls"] = "false";
    connection_properties["useSSL"] = "false";
    // std::cerr << "Connection properties:\n";
    // for (auto prop : connection_properties) {
    //   std::cerr << "- Property '" << prop.first
    // 		<< "' : '" << prop.second << "'" << std::endl;
    // }
    try {
      // conn.reset(driver->connect(url, properties));
      conn.reset(driver->connect(connection_properties));
    } catch (sql::SQLException & e) {
      std::cerr << "Error at connection: " << e.what() << std::endl;
      throw e;
    }

    // Check tables:
    std::unique_ptr<sql::PreparedStatement> statement(conn->prepareStatement("SHOW TABLES"));
    try {
      statement->execute();
      std::unique_ptr<sql::ResultSet> result(statement->getResultSet());
      std::unique_ptr<sql::ResultSetMetaData> resultMeta(result->getMetaData());
      int columnCount = resultMeta->getColumnCount();
      DT_LOG_DEBUG(master.get_logging_priority(), "Number of columns : " << columnCount);
      auto rowCount = 0u;
      while (result->next()) {
	for (int i = 0; i < columnCount; i++) {
	  auto colNum = i + 1;
	  sql::SQLString datatypeName = resultMeta->getColumnTypeName(colNum);
	  int32_t datatype = resultMeta->getColumnType(colNum);
	  sql::SQLString dataname = resultMeta->getColumnName(colNum);
	  sql::SQLString datalabel = resultMeta->getColumnLabel(colNum);
	  DT_LOG_DEBUG(master.get_logging_priority(), "Row #" << rowCount);
	  DT_LOG_DEBUG(master.get_logging_priority(), " - column #" << colNum << ' ' << "(name=" << std::quoted(dataname.c_str()) << ", label=" << std::quoted(datalabel.c_str()) << ')');
	  DT_LOG_DEBUG(master.get_logging_priority(), " - @datatype  : " << datatype << " (as " << std::quoted(datatypeName.c_str()) << ')');
	  if (datatype == sql::VARCHAR) {
	    sql::SQLString valueStr = result->getString(colNum);
	    DT_LOG_DEBUG(master.get_logging_priority(), " - @value = " << std::quoted(valueStr.c_str()));
	    this->tablenames.insert(valueStr.c_str());
	  }
	}
	rowCount++;
      }
    } catch (sql::SQLException & e) {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
      throw e;
    }
    
    return;
  }

  void db_service::pimpl_type::disconnect()
  {
    if (is_connected()) {
      return;
    }
    if (conn) {
      conn->close();
    }
    return;
  }

  /**********************************************************/
  
  const db_service::config_type & db_service::config() const
  {
    return _config_;
  }
 
  db_service::db_service()
    : datatools::base_service("db",
                              "Database service",
                              "Service accessing the SuperNEMO database")
  {
    _pimpl_ = std::make_unique<pimpl_type>(*this);    
    return;
  }

  db_service::~db_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool db_service::is_initialized() const
  {
    return _initialized_;
  }
   
  void db_service::_parse_db_access_config_(const std::string & path_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Parsing DB access config file " << std::quoted(path_) << "...");
    std::map<std::string, std::string> configValues;
    // Parsing:
    std::ifstream infile(path_);
    std::string line;
    while (getline(infile, line)) {
      std::istringstream is_line(line);
      std::string key;
      if (std::getline(is_line, key, '=')) {
	boost::trim(key);
	std::string value;
	if (key[0] == '#') {
	  continue;
	}	  
	if (std::getline(is_line, value)) {
	  boost::trim(value);
	  configValues[key] = value;
	}
      }
    }
    // Extraction:
    if (configValues.count("host")) {
      _config_.db_host = configValues["host"];
      DT_LOG_DEBUG(get_logging_priority(), "Parsed db_host=" << std::quoted(_config_.db_host));
    }
    if (configValues.count("port")) {
      _config_.db_port = std::stoi(configValues["port"]);
      DT_LOG_DEBUG(get_logging_priority(), "Parsed db_port=" << _config_.db_port);
    }
    if (configValues.count("database")) {
      _config_.db_database = configValues["database"];
      DT_LOG_DEBUG(get_logging_priority(), "Parsed db_database=" << std::quoted(_config_.db_database));
    }
    if (configValues.count("user")) {
      _config_.db_user = configValues["user"];
      DT_LOG_DEBUG(get_logging_priority(), "Parsed db_user=" << std::quoted(_config_.db_user));
    }
    if (configValues.count("password")) {
      _config_.db_password = configValues["password"];
      DT_LOG_DEBUG(get_logging_priority(), "Parsed db_password=" << std::quoted("*********"));
    }
    return;
  }

  int db_service::initialize(const datatools::properties & config_,
                             datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");

    DT_LOG_DEBUG(get_logging_priority(), "Initializing DB service...");
      
    this->datatools::base_service::_common_initialize(config_);
   
    // Attempt to set a config file for database access (address and authentication)
    std::string dbAccessConfigFile;
    if (config_.has_key("db_access")) {
      dbAccessConfigFile = config_.fetch_path("db_access");
    } else {
      dbAccessConfigFile = "~/.sndb.conf";
    }
    DT_LOG_DEBUG(get_logging_priority(), "DB service access config file : " << std::quoted(dbAccessConfigFile));
    static const char * env_variable = "SNDB_ACCESS_PATH";
    {
      DT_LOG_DEBUG(get_logging_priority(), "Searching DB service access config file from env " << std::quoted(env_variable) << "...");
      const char * envPathStr = getenv(env_variable);
      if (envPathStr != nullptr) {
	dbAccessConfigFile = std::string(envPathStr);
	DT_LOG_DEBUG(get_logging_priority(), "DB service access config file has been superseded from env : " << std::quoted(dbAccessConfigFile));
      } else {
	DT_LOG_DEBUG(get_logging_priority(), "No DB service access config file from env was found.");
      }
    }

    if (not dbAccessConfigFile.empty()) {
      datatools::fetch_path_with_env(dbAccessConfigFile);
      std::filesystem::path dbAccessConfigFilePath(dbAccessConfigFile);
      if (not std::filesystem::exists(dbAccessConfigFilePath)) {
	DT_LOG_FATAL(get_logging_priority(), "No DB service access config file " << std::quoted(dbAccessConfigFile) << " exists. Please configure one (example: '~/.sndb.conf') in your environment!");
	DT_THROW(std::logic_error, "DB service access config file " << std::quoted(dbAccessConfigFile) << " does not exist");
      }
      _parse_db_access_config_(dbAccessConfigFile);      
    } else {
      if (config_.has_key("host")) {
	_config_.db_host = config_.fetch_string("host");
      }
      if (config_.has_key("port")) {
	_config_.db_port = config_.fetch_positive_integer("port");
      }
      if (config_.has_key("version")) {
	_config_.db_version = config_.fetch_string("version");
      }
      if (config_.has_key("user")) {
	_config_.db_user = config_.fetch_string("user");
      }
      if (config_.has_key("password")) {
	_config_.db_password = config_.fetch_string("password");
      }
    }
    DT_THROW_IF(_config_.db_host.empty(), std::logic_error, "Missing DB hostname!");
    DT_THROW_IF(_config_.db_port == 0, std::logic_error, "Missing DB port!");
    DT_THROW_IF(_config_.db_user.empty(), std::logic_error, "Missing DB user!");
    DT_THROW_IF(_config_.db_password.empty(), std::logic_error, "Missing DB password!");
    DT_LOG_DEBUG(get_logging_priority(), "db_host = " << std::quoted(_config_.db_host));
    DT_LOG_DEBUG(get_logging_priority(), "db_port = " << _config_.db_port);
    DT_LOG_DEBUG(get_logging_priority(), "db_database = " << std::quoted(_config_.db_database));
    DT_LOG_DEBUG(get_logging_priority(), "db_user = " << std::quoted(_config_.db_user));
    DT_LOG_DEBUG(get_logging_priority(), "db_password = " << std::quoted("*********"));
    
    std::string sndbDescConfigFile;
    if (config_.has_key("sndb_config")) {
      sndbDescConfigFile = config_.fetch_path("sndb_config");
    }
    if (sndbDescConfigFile.empty()) {
      sndbDescConfigFile = "@falaise:snemo/demonstrator/db/sndb/1.0/main.conf";
      DT_LOG_DEBUG(get_logging_priority(),
		   "Setting default sndb main config file = "
		   << std::quoted(sndbDescConfigFile));
      datatools::fetch_path_with_env(sndbDescConfigFile);
    }
    
    datatools::properties sndbDescConfig;
    datatools::properties::read_config(sndbDescConfigFile, sndbDescConfig);
    _pimpl_->sndbDesc.initialize(sndbDescConfig);
    if (datatools::logger::is_debug(get_logging_priority())) {
      DT_LOG_DEBUG(get_logging_priority(), "SNDB descriptions : ");
      _pimpl_->sndbDesc.print_tree(std::cerr);
    }
    DT_LOG_DEBUG(get_logging_priority(), "Database name : " << std::quoted(_config_.db_database));
    DT_THROW_IF(not _pimpl_->sndbDesc.has_database(_config_.db_database), std::logic_error,
		"SNDB driver has no database named " << std::quoted(_config_.db_database));
    const snemo::db::database_entry & dbEntry = _pimpl_->sndbDesc.database(_config_.db_database);
    if (datatools::logger::is_debug(get_logging_priority())) {
      DT_LOG_DEBUG(get_logging_priority(), "Database entry : ");
      dbEntry.print_tree(std::cerr);
    }
    _pimpl_->dbDesc = &dbEntry.description();
    if (datatools::logger::is_debug(get_logging_priority())) {
      DT_LOG_DEBUG(get_logging_priority(), "Database description : ");
      _pimpl_->dbDesc->print_tree(std::cerr);
    }

    DT_LOG_DEBUG(get_logging_priority(), "Attempt to connect the remote server...");
    _connect_();
    std::set<std::string> descTablenames = _pimpl_->dbDesc->table_names();
    DT_LOG_DEBUG(get_logging_priority(), "Connection is established.");

    for (auto descTablename : descTablenames) {
      if (not _pimpl_->tablenames.count(descTablename)) {
	DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
		       "Described table " << std::quoted(descTablename) << " is not available in the database " << std::quoted(_pimpl_->connInfo.database));
      }
    }

    _initialized_ = true;
    return 0;
  }

  int db_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    _initialized_ = false;
    _disconnect_();
    _pimpl_.reset();
    return 0;
  }
    
  void db_service::_connect_()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Connecting the SuperNEMO database...");
    _pimpl_->connect();
    return;
  }

  void db_service::_disconnect_()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Disconnecting the SuperNEMO database...");
    _pimpl_->disconnect();
    DT_LOG_DEBUG(get_logging_priority(), "Connection is lost.");
   return;
  }
 
  void db_service::print_tree(std::ostream & out_,
                                const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);

    this->datatools::base_service::print_tree(out_,
					      base_print_options::force_inheritance(options_));
                                    
    out_ << popts.indent << i_tree_dumpable::tag
          << "Database  : "
          << std::boolalpha << this->_config_.db_database << std::endl;
                                     
    out_ << popts.indent << i_tree_dumpable::tag
          << "User  : "
          << std::boolalpha << this->_config_.db_user << std::endl;
                                   
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
          << "Database connected  : "
          << std::boolalpha << this->_pimpl_->is_connected() << std::endl;
   
    return;
  }

  bool db_service::has_table(const std::string & tablename_) const
  {
    return _pimpl_->tablenames.count(tablename_);
  }


  void db_service::process_select_statement(const std::string & tablename_,
					    const std::vector<std::string> & fieldnames_,
					    snemo::db::table_selection_type & selection_) const
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    DT_THROW_IF(not has_table(tablename_), std::logic_error,
		"DB has not table with name " << std::quoted(tablename_));
    datatools::logger::priority dbDriverVerbosity = datatools::logger::PRIO_FATAL;
    snemo::db::mariadb_driver dbDriver(*_pimpl_->conn, *_pimpl_->dbDesc, dbDriverVerbosity);
    dbDriver.process_select_statement(tablename_, fieldnames_, selection_);
    return;
  }

  void db_service::process_select_all_statement(const std::string & tablename_,
						snemo::db::table_selection_type & selection_) const
  {
    static const std::vector<std::string> _fields =  {"*"};
    return process_select_statement(tablename_, _fields, selection_);
  }

} // namespace snemo
