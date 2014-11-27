/* boost_access.cc
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 */

// Ourselves:
#include <falaise/snemo/io/boost_access.h>

// Third party:
// - Boost
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>

// This project:
#include <falaise/snemo/io/data_model.h>
#include <falaise/snemo/view/options_manager.h>

namespace snemo {

  namespace visualization {

    namespace io {

      void boost_access::set_sequential(const bool sequential_)
      {
        _sequential_ = sequential_;
        return;
      }

      bool boost_access::is_sequential() const
      {
        return _sequential_;
      }

      // ctor:
      boost_access::boost_access() :
        i_data_access()
      {
        _number_of_entries_   = 0;
        _current_file_number_ = 0;
        _reader_              = 0;
        return;
      }

      // dtor:
      boost_access::~boost_access()
      {
        this->reset();
        return;
      }

      size_t boost_access::get_number_of_entries() const
      {
        return _number_of_entries_;
      }

      std::string boost_access::get_file_type_as_string() const
      {
        return "BOOST/Serialization";
      }

      const std::string & boost_access::get_current_filename() const
      {
        return _file_list_.at(_current_file_number_);
      }

      bool boost_access::open(const std::vector<std::string> & filenames_)
      {
        if (_reader_ == 0) {
          _reader_ = new datatools::data_reader;
        }

        for (std::vector<std::string>::const_iterator it_file = filenames_.begin();
             it_file != filenames_.end(); ++it_file) {
          const std::string & a_file = *it_file;
          _reader_->init_multi(a_file);

          if (!is_readable())
            return false;

          if (!is_sequential())
            build_list();

          _reader_->reset();
        }

        _file_list_ = filenames_;

        // Open the first one
        _reader_->init_multi(_file_list_.at(_current_file_number_ = 0));

        return true;
      }

      bool boost_access::is_valid(const std::vector<std::string> & filenames_) const
      {
        for (std::vector<std::string>::const_iterator it_file = filenames_.begin();
             it_file != filenames_.end(); ++it_file) {
          const std::string & a_file = *it_file;

          // Check file existence
          if (!boost::filesystem::exists(a_file)) {
            DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                           "File '" << a_file << "' does not exist !");
            return false;
          }

          // Check file extension: quite complicated for such file
          // Note 1: probably one can use guess_mode_from_filename
          // static method from datatools::serialization::io_factory
          // class
          // Note 2: boost extension keep a dot which is not the
          // case for datatools: this function can probably be
          // optimized
          bool ok = false;
          const std::string gz_ext    = "." + datatools::io_factory::format::gzip_extension();
          const std::string bzip2_ext = "." + datatools::io_factory::format::bzip2_extension();
          const std::string txt_ext   = "." + datatools::io_factory::format::text_extension();
          const std::string xml_ext   = "." + datatools::io_factory::format::xml_extension();
          const std::string bin_ext   = "." + datatools::io_factory::format::binary_extension();

          if (boost::filesystem::extension(a_file) == gz_ext ||
              boost::filesystem::extension(a_file) == bzip2_ext) {
            const std::string stem = a_file.substr(0, a_file.find_last_of("."));

            if (boost::filesystem::extension(stem) == txt_ext)      ok = true;
            else if (boost::filesystem::extension(stem) == xml_ext) ok = true;
            else if (boost::filesystem::extension(stem) == bin_ext) ok = true;
          }
          else if (boost::filesystem::extension(a_file) == txt_ext) ok = true;
          else if (boost::filesystem::extension(a_file) == xml_ext) ok = true;
          else if (boost::filesystem::extension(a_file) == bin_ext) ok = true;

          if (!ok) {
            DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                         a_file << " is not a boost archive !");
            return false;
          }
        }

        std::ostringstream message;
        message << filenames_.front();
        filenames_.size() > 1 ?
          message << " (and others) are boost archives" :
          message << " is a boost archive";
        DT_LOG_INFORMATION(view::options_manager::get_instance().get_logging_priority(),
                           message.str());
        return true;
      }

      bool boost_access::is_readable()
      {
        // Check if it has record tag
        if (!_reader_->has_record_tag()) {
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "File has no record tag");
          return false;
        }

        // Check serial tag
        if (_reader_->record_tag_is(event_record::SERIAL_TAG)) {
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "Record tag is '" << event_record::SERIAL_TAG << "'");
        } else if (_reader_->record_tag_is(mctools::simulated_data::SERIAL_TAG)) {
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "Record tag is '" << mctools::simulated_data::SERIAL_TAG << "'");
        } else if (_reader_->record_tag_is(datatools::properties::SERIAL_TAG)) {
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "Record tag is '" << datatools::properties::SERIAL_TAG << "'");
        } else {
          DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                       "Serial tag '" << _reader_->get_record_tag() << "' "
                       << "not yet implemented");
          return false;
        }

        return true;
      }

      bool boost_access::is_opened() const
      {
        return (_reader_ != 0 ) && (_reader_->is_initialized() || !is_sequential());
      }

      bool boost_access::rewind()
      {
        // This is needed by event_display::show_all_vertices:
        // I don't really know where to put that for the time being
        if (is_sequential()) {
          close();

          if (_file_list_.empty())
            return false;

          if (_reader_ == 0) {
            _reader_ = new datatools::data_reader;
          }
          _reader_->init_multi(_file_list_.at(_current_file_number_ = 0));
        }

        return true;
      }

      bool boost_access::reset()
      {
        _number_of_entries_   = 0;
        _current_file_number_ = 0;

        _file_list_.clear();
        _sim_data_list_.clear();

        close();

        return true;
      }

      bool boost_access::close()
      {
        if (is_opened()) {
          _reader_->reset();
          delete _reader_;
          _reader_ = 0;
        }

        return true;
      }

      bool boost_access::build_list()
      {
        DT_LOG_NOTICE(view::options_manager::get_instance().get_logging_priority(),
                      "Loading the set of simulated data entries... please wait...");

        while (_reader_->has_record_tag()) {
          if (_reader_->record_tag_is(mctools::simulated_data::SERIAL_TAG)) {
            // Memory allocation optimization :
            if (_sim_data_list_.capacity() < 10) {
              _sim_data_list_.reserve(1000);
            }
            mctools::simulated_data sim_data;
            // Store a blank sim_data :
            _sim_data_list_.push_back(sim_data);
            // Work on the last entry :
            _reader_->load(_sim_data_list_.back());
          } else // [_reader_->record_tag_is(event_record::SERIAL_TAG)]
            {
              DT_THROW_IF(true, std::logic_error,
                          "The 'preload' mode is not supported for archived 'event_record' objects !");
            }
          _number_of_entries_++;
        }

        DT_LOG_INFORMATION(view::options_manager::get_instance().get_logging_priority(),
                           "Total number of record = " << _number_of_entries_);
        return true;
      }

      bool boost_access::retrieve_event(event_record & event_,
                                        const size_t event_number_)
      {
        if (is_sequential()) {
          if (!_reader_->has_record_tag()) {
            _current_file_number_++;

            if (_current_file_number_ >= _file_list_.size()) {
              DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                             "No more record tag");
              return true;
            }
            _reader_->reset();
            _reader_->init_multi(_file_list_.at(_current_file_number_));
            _number_of_entries_++;
          }
        } else {
          if (event_number_ >= _number_of_entries_) {
            DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                           "No more record tag");
            return true;
          }
          event_.clear();
          mctools::simulated_data & sim_data = event_.add<mctools::simulated_data>(SD_LABEL);
          sim_data = _sim_data_list_.at(event_number_);
          return true;
        }

        // Clear event
        event_.clear();

        // Loop over metadata
        bool record_found = false;
        while (! record_found) {
          if (_reader_->record_tag_is(event_record::SERIAL_TAG)) {
            _reader_->load(event_);
            record_found = true;
          } else if (_reader_->record_tag_is(mctools::simulated_data::SERIAL_TAG)) {
            mctools::simulated_data & sim_data = event_.add<mctools::simulated_data>(SD_LABEL);
            _reader_->load(sim_data);
            record_found = true;
          } else if (_reader_->record_tag_is(datatools::properties::SERIAL_TAG)) {
            DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Metadata found !");
            datatools::properties metadata;
            _reader_->load(metadata);
            record_found = false;
          } else {
            DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                           "Record tag '" << _reader_->get_record_tag() << "' is not supported");
            return false;
          }
        }
        return true;
      }

      void boost_access::tree_dump(std::ostream      & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool /*inherit_*/) const
      {
        std::string indent;
        if (! indent_.empty()) indent = indent_;
        if (! title_.empty()) {
          out_ << indent << title_ << std::endl;
        }

        out_ << indent
             << datatools::i_tree_dumpable::tag
             << "File type : " << get_file_type_as_string()
             << std::endl;

        // File list
        out_ << indent << datatools::i_tree_dumpable::last_tag
             << "Attached files : " << _file_list_.size() << std::endl;

        for (std::vector<std::string>::const_iterator ifile = _file_list_.begin();
             ifile != _file_list_.end(); ++ifile) {
          out_ << indent << datatools::i_tree_dumpable::last_skip_tag;
          std::vector<std::string>::const_iterator jfile = ifile;
          if (++jfile == _file_list_.end()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "File["
               << std::distance(_file_list_.begin(), ifile)
               << "]: '" << *ifile << "'" << std::endl;
        }
        return;
      }

      void boost_access::dump() const
      {
        this->tree_dump(std::clog, "snemo::visualization::io::boost_access");
        return;
      }

    } // end of namespace io

  } // end of namespace visualization

} // end of namespace snemo

// end of boost_access.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
