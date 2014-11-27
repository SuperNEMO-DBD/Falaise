// -*- mode: c++ ; -*-
/* i_data_access.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-07-31
 * Last modified: 2010-07-31
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *   interface (abstract) class to data access
 *   methods are implemented in the daughter class
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_IO_I_DATA_ACCESS_H
#define FALAISE_SNEMO_VISUALIZATION_IO_I_DATA_ACCESS_H 1

#include <falaise/snemo/io/data_model.h>

#include <datatools/i_tree_dump.h>

#include <string>
#include <vector>

namespace snemo {

  namespace visualization {

    namespace io {

      class i_data_access : public datatools::i_tree_dumpable
      {
      public:

        i_data_access();

        virtual ~i_data_access();

        virtual size_t get_number_of_entries() const = 0;

        virtual std::string get_file_type_as_string() const = 0;

        virtual const std::string & get_current_filename() const = 0;

        virtual bool open(const std::vector<std::string> & filenames_) = 0;

        virtual bool is_valid(const std::vector<std::string> & filenames_) const = 0;

        virtual bool is_readable() = 0;

        virtual bool is_opened() const = 0;

        virtual bool rewind() = 0;

        virtual bool reset() = 0;

        virtual bool close() = 0;

        virtual void tree_dump(std::ostream      & out_    = std::clog,
                               const std::string & title_  = "",
                               const std::string & indent_ = "",
                               bool inherit_               = false) const = 0;

        virtual void dump() const = 0;

        // Need probably a better name
        virtual bool build_list() = 0;

        virtual bool retrieve_event(event_record & event_,
                                    const size_t event_number_) = 0;

      private:

        bool _sequential_;
      };

    } // end of namespace io

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_IO_I_DATA_ACCESS_H

// end of i_data_access.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
