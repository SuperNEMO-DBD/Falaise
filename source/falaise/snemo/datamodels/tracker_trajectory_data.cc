// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/tracker_trajectory_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

namespace snemo {

  namespace datamodel {

    bool tracker_trajectory_data::has_solutions () const
    {
      return get_number_of_solutions () > 0;
    }

    size_t tracker_trajectory_data::get_number_of_solutions () const
    {
      return _solutions_.size ();
    }

    /*
      tracker_trajectory_solution & tracker_trajectory_data::grab_solution (int i_)
      {
      return _solutions_.at (i_).get ();
      }
    */

    const tracker_trajectory_solution & tracker_trajectory_data::get_solution (int i_) const
    {
      return _solutions_.at (i_).get ();
    }

    void tracker_trajectory_data::add_solution (const tracker_trajectory_solution::handle_type & solution_handle_, bool default_solution_)
    {
      DT_THROW_IF (!solution_handle_, std::logic_error, "Cannot store a null handle !");
      for (size_t i = 0; i < _solutions_.size (); i++) {
        tracker_trajectory_solution::handle_type h = _solutions_.at (i);
        const tracker_trajectory_solution * addr = &(h.get ());
        DT_THROW_IF (addr == &(solution_handle_.get ()), std::logic_error,
                     "Duplicated solutions is not allowed!");
      }
      _solutions_.push_back (solution_handle_);
      if (default_solution_ || _solutions_.size () == 1) {
        _default_solution_ = solution_handle_;
      }
      return;
    }

    void tracker_trajectory_data::invalidate_solutions ()
    {
      _solutions_.clear ();
      return;
    }

    bool tracker_trajectory_data::has_default_solution () const
    {
      return _default_solution_.has_data ();
    }

    const tracker_trajectory_solution & tracker_trajectory_data::get_default_solution () const
    {
      DT_THROW_IF (! has_default_solution (), std::logic_error, "No default solution is available !");
      return _default_solution_.get ();
    }

    /// Set the default trajectory solution
    void tracker_trajectory_data::set_default_solution (int index_)
    {
      _default_solution_ = _solutions_.at (index_);
      return;
    }

    void tracker_trajectory_data::invalidate_default_solution ()
    {
      _default_solution_.reset();
      return;
    }

    datatools::properties & tracker_trajectory_data::grab_auxiliaries ()
    {
      return _auxiliaries_;
    }

    const datatools::properties & tracker_trajectory_data::get_auxiliaries () const
    {
      return _auxiliaries_;
    }

    tracker_trajectory_data::solution_col_type & tracker_trajectory_data::grab_solutions ()
    {
      return _solutions_;
    }

    const tracker_trajectory_data::solution_col_type & tracker_trajectory_data::get_solutions () const
    {
      return _solutions_;
    }

    void tracker_trajectory_data::reset ()
    {
      this->clear ();
      return;
    }

    void tracker_trajectory_data::clear ()
    {
      invalidate_solutions ();
      invalidate_default_solution ();
      _auxiliaries_.clear ();
      return;
    }

    tracker_trajectory_data::tracker_trajectory_data ()
    {
      return;
    }

    tracker_trajectory_data::~tracker_trajectory_data ()
    {
      this->reset ();
      return;
    }

    void tracker_trajectory_data::tree_dump (std::ostream      & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) {
        indent = indent_;
      }
      if ( ! title_.empty () ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solutions : " << _solutions_.size () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Default solutions : " << (_default_solution_  ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Auxiliaries : ";
      if (_auxiliaries_.empty ()) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_) ;
        _auxiliaries_.tree_dump (out_, "", indent_oss.str ());
      }

      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory_data, "snemo::datamodel::tracker_trajectory_data")

  } // end of namespace datamodel

} // end of namespace snemo
