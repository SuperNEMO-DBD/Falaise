// track_history.cc

// Ourselves:
#include <mctools/g4/track_history.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - bayeux/datatools:
#include <datatools/exception.h>

namespace mctools {

  namespace g4 {

    // static
    const int track_history::track_info::TRACK_ID_UNSET;

    auto track_history::track_info::get_id () const -> int
    {
      return _id_;
    }

    void track_history::track_info::set_id (const int id_)
    {
      _id_ = id_;
   }

    auto track_history::track_info::get_parent_id () const -> int
    {
      return _parent_id_;
    }

    void track_history::track_info::set_parent_id (const int id_)
    {
      _parent_id_ = id_;
   }

    auto track_history::track_info::is_primary () const -> bool
    {
      return _parent_id_ == TRACK_ID_UNSET;
    }

    auto track_history::track_info::get_particle_name () const -> const std::string &
    {
      return _particle_name_;
    }

    void track_history::track_info::set_particle_name (const std::string & name_)
    {
      _particle_name_ = name_;
    }

    auto track_history::track_info::get_creator_process_name () const -> const std::string &
    {
      return _creator_process_name_;
    }

    void track_history::track_info::set_creator_process_name (const std::string & name_)
    {
      _creator_process_name_ = name_;
    }

    auto track_history::track_info::get_creator_sensitive_category () const -> const std::string &
    {
      return _creator_sensitive_category_;
    }

    void track_history::track_info::set_creator_sensitive_category (const std::string & category_)
    {
      _creator_sensitive_category_ = category_;
    }

    void track_history::track_info::reset ()
    {
      _id_        = TRACK_ID_UNSET;
      _parent_id_ = TRACK_ID_UNSET;
    }

    track_history::track_info::track_info ()
    {
      reset ();
   }

    track_history::track_info::~track_info ()
    {
      reset ();
   }

    auto track_history::get_track_infos () const -> const track_history::track_info_dict_type &
    {
      return _track_infos_;
    }

    auto track_history::grab_track_infos () -> track_history::track_info_dict_type &
    {
      return _track_infos_;
    }

    auto track_history::has_track_info (const int id_) const -> bool
    {
      return _track_infos_.find (id_) != _track_infos_.end ();
    }

    auto track_history::get_track_info (const int id_) const -> const track_history::track_info &
    {
      DT_THROW_IF (! has_track_info(id_), std::logic_error,
                   "No track with id " << id_ << " has been stored!");
      return _track_infos_.at (id_);
    }

    auto track_history::grab_track_info (const int id_) -> track_history::track_info &
    {
      DT_THROW_IF (! has_track_info(id_), std::logic_error,
                   "No track with id " << id_ << " has been stored!");
      return _track_infos_[id_];
    }

    void track_history::add_track_info (const int id_, const track_info & tinfo_)
    {
      DT_THROW_IF (has_track_info(id_), std::logic_error,
                   "A track with id '" << id_ << "' already exist!");
      _track_infos_[id_] = tinfo_;
    }

    track_history::track_history ()
    = default;

    track_history::~track_history ()
    {
      reset();
   }

    void track_history::reset()
    {
      _track_infos_.clear();
   }

  }  // end of namespace g4

}  // end of namespace mctools

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
