
// Ourselves:
#include <falaise/snemo/db/table.h>

namespace snemo {

  namespace db {

    table::table(const std::string & name_)
      : _name_(name_)
    {
      return;
    }

    const std::string & table::name() const
    {
      return _name_;
    }
    
  } // end of namespace db

} // end of namespace snemo
