/* brio_access.cc
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
 */

// Ourselves:
#include <falaise/snemo/io/brio_access.h>

// Third party:
// - Boost
#include <boost/filesystem.hpp>
// - Bayeux/brio
#include <brio/reader.h>
// - Bayeux/dpp
#include <dpp/brio_common.h>

// This project:
#include <falaise/snemo/io/data_model.h>
#include <falaise/snemo/view/options_manager.h>

namespace snemo {

  namespace visualization {

    namespace io {

      // ctor:
      brio_access::brio_access() :
        i_data_access()
      {
        _number_of_entries_   = 0;
        _current_file_number_ = 0;
        _reader_              = 0;
        return;
      }

      // dtor:
      brio_access::~brio_access()
      {
        this->reset();
        return;
      }

      size_t brio_access::get_number_of_entries() const
      {
        return _number_of_entries_;
      }

      std::string brio_access::get_file_type_as_string() const
      {
        return "BRIO";
      }

      const std::string & brio_access::get_current_filename() const
      {
        return _file_list_.at(_current_file_number_);
      }

      bool brio_access::open(const std::vector<std::string> & filenames_)
      {
        if (_reader_ == 0) {
          _reader_ = new brio::reader;
        }

        for (std::vector<std::string>::const_iterator it_file = filenames_.begin();
             it_file != filenames_.end(); ++it_file) {
          const std::string & a_file = *it_file;
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "Opening file " << a_file << "...");
          _reader_->open(a_file);
          if (!is_readable()) return false;
          build_list();
          _reader_->close();
        }

        _file_list_ = filenames_;

        // open the first one
        _reader_->open(_file_list_.at(_current_file_number_ = 0));
        return true;
      }

      bool brio_access::is_valid(const std::vector<std::string> & filenames_) const
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

          // Check file extension
          int mode;
          int status = brio::store_info::guess_mode_from_filename(a_file, mode);
          if (status == brio::store_info::ERROR) {
            DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                         a_file << " is not a brio archive !");
            return false;
          }
        }

        std::ostringstream message;
        message << filenames_.front();
        filenames_.size() > 1 ?
          message << " (and others) are brio archives" :
          message << " is a brio archive";
        DT_LOG_INFORMATION(view::options_manager::get_instance().get_logging_priority(),
                           message.str());
        return true;
      }

      bool brio_access::is_readable()
      {
        // Check different mode:
        std::list<std::string> modes;
        modes.push_back(dpp::brio_common::event_record_store_label());
        modes.push_back(dpp::brio_common::general_info_store_label());
        //modes.push_back(dpp::brio_common::PLAIN_SIMULATED_DATA_STORE_LABEL);

        for (std::list<std::string>::const_iterator imode = modes.begin();
             imode != modes.end(); ++imode) {
          const std::string & mode = *imode;
          if (_reader_->has_store(mode)) {
            DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                         "Store mode is '" << mode << "' mode...");
            _mode_ = mode;
            break;
          }
        }

        if (_mode_.empty()) {
          DT_LOG_DEBUG(view::options_manager::get_instance().get_logging_priority(),
                       "Do not find any known mode");
          return false;
        }
        return true;
      }

      bool brio_access::is_opened() const
      {
        return (_reader_ != 0) && _reader_->is_opened();
      }

      bool brio_access::rewind()
      {
        this->close();

        // Reset should "reset" everything, so it implies removing
        // _file_list_ and ask user to reconstruct all the chain
        // This is needed by event_display::show_all_vertices:
        // I don't really know where to put that for the time being
        if (_file_list_.empty())
          return false;

        if (_reader_ == 0) {
          _reader_ = new brio::reader;
        }
        _reader_->open(_file_list_.at(_current_file_number_ = 0));

        return true;
      }

      bool brio_access::reset()
      {
        _current_file_number_ = 0;
        _number_of_entries_   = 0;

        _file_list_.clear();
        _entry_list_.clear();
        _mode_.clear();

        close();
        return true;
      }

      bool brio_access::close()
      {
        if (this->is_opened()) {
          _reader_->close();
          delete _reader_;
          _reader_ = 0;
        }
        return true;
      }

      bool brio_access::build_list()
      {
        DT_LOG_NOTICE(view::options_manager::get_instance().get_logging_priority(),
                      "Loading set of events... please wait...");

        for (size_t i = _number_of_entries_;
             i < _number_of_entries_ + _reader_->get_number_of_entries(_mode_);
             ++i) {
          _entry_list_[i] = std::make_pair(_current_file_number_,
                                           i - _number_of_entries_);
        }
        _number_of_entries_ += _reader_->get_number_of_entries(_mode_);
        _current_file_number_++;

        DT_LOG_INFORMATION(view::options_manager::get_instance().get_logging_priority(),
                           "Total number of record = " << _number_of_entries_);
        return true;
      }

      bool brio_access::retrieve_event(event_record & event_,
                                       const size_t event_number_)
      {
        if (event_number_ >= _number_of_entries_) {
          DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                         "No more record tag");
          return false;
        }

        // Get corresponding event file
        const size_t file_idx = _entry_list_[event_number_].first;
        const size_t event_number = _entry_list_[event_number_].second;

        if (file_idx != _current_file_number_) {
          _current_file_number_ = file_idx;
          _reader_->close();
          _reader_->open(_file_list_.at(_current_file_number_));
        }

        // Clear event
        event_.clear();

        // if (_mode_ == dpp::brio_common::PLAIN_SIMULATED_DATA_STORE_LABEL)
        //   {
        //     sim_data_bank & sim_data = event_.add<sim_data_bank>(SD_LABEL);
        //     _reader_->load(sim_data, _mode_, event_number);
        //   }
        // else
        if (_mode_ == dpp::brio_common::event_record_store_label()) {
          _reader_->load(event_, _mode_, event_number);
        } else {
          DT_LOG_WARNING(view::options_manager::get_instance().get_logging_priority(),
                         "Record mode '" << _mode_ << "' is not supported");
          return false;
        }
        return true;
      }

      void brio_access::tree_dump(std::ostream      & out_,
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
               << distance(_file_list_.begin(), ifile)
               << "]: '" << *ifile << "'" << std::endl;
        }
        return;
      }

      void brio_access::dump() const
      {
        this->tree_dump(std::clog, "snemo::visualization::io::brio_access");
        return;
      }

    } // end of namespace io

  } // end of namespace visualization

} // end of namespace snemo

// end of brio_access.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
