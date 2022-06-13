/// \file falaise/snemo/datamodels/event_header.cc

// Ourselves:
#include <falaise/snemo/datamodels/event_header.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(event_header, "snemo::datamodel::event_header")

    const datatools::event_id& event_header::get_id() const { return id_; }

    datatools::event_id& event_header::get_id() { return id_; }

    void event_header::set_id(const datatools::event_id& id) { id_ = id; }

    const datatools::properties& event_header::get_properties() const { return properties_; }

    datatools::properties& event_header::get_properties() { return properties_; }

    void event_header::set_properties(const datatools::properties& pset) { properties_ = pset; }

    /// Check the MC timestamp validity
    bool event_header::has_timestamp() const
    {
      return timestamp_.is_valid();
    }

    const snemo::datamodel::timestamp& event_header::get_timestamp() const { return timestamp_; }

    snemo::datamodel::timestamp& event_header::get_timestamp() { return timestamp_; }

    void event_header::set_timestamp(const snemo::datamodel::timestamp& ts) { timestamp_ = ts; }

    bool event_header::has_mc_run_id() const
    {
      return mc_run_id_ >= 0;
    }

    std::int32_t event_header::get_mc_run_id() const
    {
      return mc_run_id_;
    }

    void event_header::set_mc_run_id(std::int32_t run_id_)
    {
      mc_run_id_ = run_id_;
      return;
    }

    bool event_header::has_mc_timestamp() const
    {
      return time::is_valid(mc_timestamp_);
    }

    const snemo::time::time_point & event_header::get_mc_timestamp() const
    {
      return mc_timestamp_;
    }

    void event_header::set_mc_timestamp(const snemo::time::time_point & t_)
    {
      DT_THROW_IF(not is_simulated(), std::logic_error,
                  "Cannot set MC event timestamp on non-simulated event!");
      mc_timestamp_ = t_;
      return;
    }

    event_header::generation_type event_header::get_generation() const { return generation_; }

    void event_header::set_generation(generation_type gen) { generation_ = gen; }

    bool event_header::is_real() const { return generation_ == GENERATION_REAL; }

    bool event_header::is_simulated() const { return generation_ == GENERATION_SIMULATED; }

    void event_header::clear() {
      properties_.clear();
      timestamp_ = snemo::datamodel::timestamp{};
      generation_ = GENERATION_INVALID;
      id_.clear();
    }

    void event_header::tree_dump(std::ostream& out, const std::string& title, const std::string& indent,
                                 bool is_last) const {
      if (!title.empty()) {
        out << indent << title << std::endl;
      }

      out << indent << datatools::i_tree_dumpable::tag << "Id : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        id_.tree_dump(out, "", indent_oss.str());
      }

      out << indent << datatools::i_tree_dumpable::tag << "Timestamp : ";
      if (has_timestamp()) {
        out << timestamp_;
      } else {
        out << "none";
      }
      out << std::endl;

      out << indent << datatools::i_tree_dumpable::tag << "MC run ID : ";
      if (has_mc_run_id()) {
        out << mc_run_id_;
      } else {
        out << "none";
      }
      out << std::endl;

      out << indent << datatools::i_tree_dumpable::tag << "MC timestamp : ";
      if (has_mc_timestamp()) {
        out << mc_timestamp_;
      } else {
        out << "none";
      }
      out << std::endl;

      out << indent << datatools::i_tree_dumpable::tag << "Properties : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        properties_.tree_dump(out, "", indent_oss.str());
      }

      out << indent << datatools::i_tree_dumpable::inherit_tag(is_last) << "Generation : ";
      if (is_simulated()) {
        out << "simulated";
      }
      if (is_real()) {
        out << "real";
      }
      out << std::endl;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
