/// \file mctools/g4/track_history.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-18
 * Last modified : 2013-06-03
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * Description:
 *
 *   Track history.
 *
 * History:
 *
 */

#ifndef SNSIM_TRACK_HISTORY_H
#define SNSIM_TRACK_HISTORY_H 1

// Standard library:
#include <map>
#include <string>

namespace snsim {

/// \brief Recording of tracks history
class track_history {
 public:
  /// \brief Recording of informations about a single track
  class track_info {
   public:
    /// Undefined Id for a track (parent track Id is 0 for a primary track)
    static const int TRACK_ID_UNSET = 0;

   public:
    int get_id() const;

    void set_id(const int id_);

    int get_parent_id() const;

    void set_parent_id(const int id_);

    bool is_primary() const;

    const std::string& get_particle_name() const;

    void set_particle_name(const std::string& name_);

    const std::string& get_creator_process_name() const;

    void set_creator_process_name(const std::string& name_);

    /// \deprecated
    const std::string& get_creator_sensitive_category() const;

    /// \deprecated
    void set_creator_sensitive_category(const std::string& category_);

    /// Constructor
    track_info();

    /// Destructor
    ~track_info();

    /// Reset
    void reset();

   private:
    int _id_;                                  //!< G4 particle id
    int _parent_id_;                           //!< G4 parent id (if any)
    std::string _particle_name_;               //!< G4 particle name
    std::string _creator_process_name_;        //!< G4 creation process name
    std::string _creator_sensitive_category_;  //!< SNG4 sensitive category (obsolete)
  };

 public:
  /// Dictionary type of track info
  typedef std::map<int, track_info> track_info_dict_type;

  const track_info_dict_type& get_track_infos() const;

  track_info_dict_type& grab_track_infos();

  bool has_track_info(const int id_) const;

  const track_info& get_track_info(const int id_) const;

  track_info& grab_track_info(const int id_);

  void add_track_info(const int id_, const track_info& tinfo_);

  /// Constructor
  track_history();

  /// Destructor
  ~track_history();

  /// Reset the track history object
  void reset();

 private:
  track_info_dict_type _track_infos_;  //!< Dictionary of track informations
};

}  // namespace snsim

#endif  // SNSIM_TRACK_HISTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
