/// \file falaise/snemo/datamodels/event_header.cc

// Ourselves:
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(event_header,
						      "snemo::datamodel::event_header")

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

    bool event_header::is_event_builder() const
    {
      return properties_.has_flag(snedm::labels::event_builder_key());
    }

    // override
    void event_header::clear() {
      properties_.clear();
      timestamp_ = snemo::datamodel::timestamp{};
      generation_ = GENERATION_INVALID;
      id_.clear();
    }

    // override
    void event_header::print_tree(std::ostream & out_,
				  const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Id : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << popts.indent << skip_tag;
        id_.tree_dump(out_, "", indent_oss.str());
      }

      out_ << popts.indent << tag << "Timestamp : ";
      if (has_timestamp()) {
        out_ << timestamp_;
      } else {
        out_ << "none";
      }
      out_ << std::endl;

      out_ << popts.indent << tag << "Generation : ";
      if (is_simulated()) {
        out_ << std::quoted("simulated");
      }
      if (is_real()) {
        out_ << std::quoted("real");
      }
      out_ << std::endl;

      out_ << popts.indent << tag << "Event builder : ";
      out_ << std::boolalpha << is_event_builder();
      if (properties_.has_key("event_builder.model")) {
        out_ << " (model=" << std::quoted(properties_.fetch_string("event_builder.model")) << ')';
      }
      out_ << std::endl;

      if (is_simulated()) {
  
	out_ << popts.indent << skip_tag << tag << "MC run ID : ";
	if (has_mc_run_id()) {
	  out_ << mc_run_id_;
	} else {
	  out_ << "none";
	}
	out_ << std::endl;
	
	out_ << popts.indent << skip_tag << last_tag << "MC timestamp : ";
	if (has_mc_timestamp()) {
	  out_ << mc_timestamp_;
	} else {
	  out_ << "none";
	}
	out_ << std::endl;
	
      }
      
      out_ << popts.indent << inherit_tag(popts.inherit) << "Auxiliary properties : " << properties_.size() << std::endl;
      if (list_properties_opt) {
        boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + tags::item(not popts.inherit, true));
        properties_.print_tree(out_, auxOpts);
      }

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
