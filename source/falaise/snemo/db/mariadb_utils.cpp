// Ourselves:
#include <falaise/snemo/db/mariadb_utils.hpp>

// Standard library:
#include <iomanip>
#include <bitset>

// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

namespace snemo {

  namespace db {

    mariadb_driver::mariadb_driver(sql::Connection & conn_,
				   const database_description & dbdesc_,
				   const datatools::logger::priority verbosity_)
      : _conn_(conn_)
      , _dbdesc_(dbdesc_)
    {
      _verbosity_ = verbosity_;
      return;
    }

    // const database_description & dbdesc() const
    // {
    //   return _dbdesc_;
    // }
    
    void mariadb_driver::process_select_statement(const std::string & tablename_,
						  const std::vector<std::string> & fieldnames_,
						  table_selection_type & selection_)
      
    {
      datatools::logger::priority logging = _verbosity_;
      // logging = datatools::logger::PRIO_DEBUG;
      selection_.clear();
      std::ostringstream fieldtags_iss;
      if (fieldnames_.empty()
	  or (fieldnames_.size() == 1 and fieldnames_.front() == "*")) {
	fieldtags_iss << '*';
      } else if (fieldnames_.size() > 1) {
	for (auto iField = 0u; iField < fieldnames_.size(); iField++) {
	  if (iField+1 == fieldnames_.size()) fieldtags_iss << ',';
	  fieldtags_iss << fieldnames_[iField];
	}
      }
      std::ostringstream statement_iss;
      statement_iss << "SELECT " << fieldtags_iss.str() << " FROM " << tablename_ << ';';
      std::unique_ptr<sql::PreparedStatement> 
	statement(_conn_.prepareStatement(statement_iss.str().c_str()));
      statement->execute();
      std::unique_ptr<sql::ResultSet> result(statement->getResultSet());
      std::unique_ptr<sql::ResultSetMetaData> resultMeta(result->getMetaData());
      auto columnCount = resultMeta->getColumnCount();
      auto rowCount = 0u;
      while (result->next()) {
	DT_LOG_DEBUG(logging, "Next row with " << columnCount << " columns");
	// std::clog << "  @first : " << std::boolalpha << result->isFirst() << '\n';
	// std::clog << "  @last  : " << std::boolalpha << result->isLast() << '\n';
	record_type raw_record;
	for (auto iColumn = 0u; iColumn < columnCount; iColumn++) {
	  auto colNum = iColumn + 1;
	  DT_LOG_DEBUG(logging, "  Field " << iColumn << " type=" << resultMeta->getColumnType(colNum) << " as "
		       << std::quoted(resultMeta->getColumnTypeName(colNum).c_str()));
	  int32_t datatype = resultMeta->getColumnType(colNum);
	  // See include/mariadb/conncpp/ResultSet.hpp
	  // See include/mariadb/conncpp/jdbccompat.hpp
	  // See https://mariadb.com/kb/en/data-types-numeric-data-types/
	  record_field_type field;
	  if (datatype == sql::BOOLEAN) {
	    bool value = result->getBoolean(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << value);
	  } else if (datatype == sql::Types::INTEGER) { // type=15 
	    int value = result->getInt(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << value);
	  } else if (datatype == 38) { // type=38 (sql::Types::BIT or sql::Types::BINARY?)
	    int value = (int) result->getUInt(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << std::bitset<8>(value));
	  } else if (datatype == sql::Types::REAL) { // or DOUBLE type=13
	    double value = result->getDouble(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << value);
	  } else if (datatype == 13) { // sql::Types::DOUBLE 
	    double value = result->getDouble(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << value);
	  } else if (datatype == sql::Types::FLOAT) {
	    double value = (double) result->getFloat(colNum);
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << value);
	  } else if (datatype == sql::Types::VARCHAR) { // type=39 
	    std::string value = result->getString(colNum).c_str();
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << std::quoted(value));
	  } else if (datatype == 19 ) { // sql::Types::LONGTEXT for JSON
	    std::string value = result->getString(colNum).c_str();
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << std::quoted(value));
	  } else if (datatype == sql::Types::TIMESTAMP) { // type=35
	    std::string value = result->getString(colNum).c_str();
	    field = value;
	    DT_LOG_DEBUG(logging, "    @value : " << std::quoted(value));
	  }
	  raw_record.push_back(field);
	}
	selection_.push_back(raw_record);
	rowCount++;
      }
      DT_LOG_DEBUG(logging, "Number of rows : " << rowCount);
      return;
    }
 

  } // end of namespace db

} // end of namespace snemo
