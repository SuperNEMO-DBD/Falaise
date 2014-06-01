/// \file falaise/snemo/datamodels/tracker_clustering_data.cc

// Ourselves
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

  namespace datamodel {

    // static
    const std::string & tracker_clustering_data::prompt_key()
    {
      static const std::string _key("prompt");
      return _key;
    }

    // static
    const std::string & tracker_clustering_data::delayed_key()
    {
      static const std::string _key("delayed");
      return _key;
    }

    // static
    const std::string & tracker_clustering_data::delayed_id_key()
    {
      static const std::string _key("delayed.id");
      return _key;
    }

    // static
    const std::string & tracker_clustering_data::clusterizer_id_key()
    {
      static const std::string _key("clusterizer.id");
      return _key;
    }

    bool tracker_clustering_data::has_solutions() const
    {
      return get_number_of_solutions() > 0;
    }

    size_t tracker_clustering_data::get_number_of_solutions() const
    {
      return _solutions_.size();
    }

    const tracker_clustering_solution & tracker_clustering_data::get_solution(int i_) const
    {
      return _solutions_.at(i_).get();
    }

    void tracker_clustering_data::add_solution(const tracker_clustering_solution::handle_type & solution_handle_, bool default_solution_)
    {
      DT_THROW_IF(!solution_handle_, std::logic_error, "Cannot store a null handle !");
      for(size_t i = 0; i < _solutions_.size(); i++) {
        tracker_clustering_solution::handle_type h = _solutions_.at(i);
        tracker_clustering_solution * addr = &(h.grab());
        DT_THROW_IF(addr == &(solution_handle_.get()), std::logic_error,
                    "Duplicated solutions is not allowed!");
      }
      _solutions_.push_back(solution_handle_);
      if(default_solution_ || _solutions_.size() == 1) {
        _default_solution_ = solution_handle_;
      }
      return;
    }

    void tracker_clustering_data::invalidate_solutions()
    {
      _solutions_.clear();
      return;
    }

    bool tracker_clustering_data::has_default_solution() const
    {
      return _default_solution_.has_data();
    }

    tracker_clustering_solution & tracker_clustering_data::grab_default_solution()
    {
      DT_THROW_IF(! has_default_solution(), std::logic_error, "No default solution is available !");
      return _default_solution_.grab();
    }

    const tracker_clustering_solution & tracker_clustering_data::get_default_solution() const
    {
      DT_THROW_IF(! has_default_solution(), std::logic_error, "No default solution is available !");
      return _default_solution_.get();
    }

    void tracker_clustering_data::set_default_solution(int index_)
    {
      _default_solution_ = _solutions_.at(index_);
      return;
    }

    void tracker_clustering_data::invalidate_default_solution()
    {
      _default_solution_.reset();
      return;
    }

    datatools::properties & tracker_clustering_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & tracker_clustering_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    tracker_clustering_data::solution_col_type & tracker_clustering_data::grab_solutions()
    {
      return _solutions_;
    }

    const tracker_clustering_data::solution_col_type & tracker_clustering_data::get_solutions() const
    {
      return _solutions_;
    }

    void tracker_clustering_data::reset()
    {
      this->clear();
      return;
    }

    void tracker_clustering_data::clear()
    {
      invalidate_solutions();
      invalidate_default_solution();
      _auxiliaries_.clear();
      return;
    }

    tracker_clustering_data::tracker_clustering_data()
    {
      return;
    }

    tracker_clustering_data::~tracker_clustering_data()
    {
      this->reset();
      return;
    }

    void tracker_clustering_data::tree_dump(std::ostream      & out_,
                                            const std::string & title_,
                                            const std::string & indent_,
                                            bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if( ! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solution(s) : " << _solutions_.size() << std::endl;
      for (int i = 0; i < (int) get_number_of_solutions(); i++) {
        const tracker_clustering_solution & tcsol = get_solution(i);
        std::ostringstream indent2;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        indent2 << indent << datatools::i_tree_dumpable::skip_tag;
        if (i == (int) _solutions_.size() - 1) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent2 << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent2 << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Solution #" << i << " : " << std::endl;
        tcsol.tree_dump(out_, "", indent2.str());
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Default solution : " << (!_default_solution_  ? "No" : "Yes") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : ";
      if(_auxiliaries_.empty()) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
        _auxiliaries_.tree_dump(out_, "", indent_oss.str());
      }

      return;
    }

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_data, "snemo::datamodel::tracker_clustering_data")

  } // end of namespace datamodel

} // end of namespace snemo
