// falaise/snemo/cuts/fake_trigger_cut.cc

// Ourselves:
#include <falaise/snemo/digitization/fake_trigger_cut.h>

/// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>
// - Bayeux/mctools:
#include <bayeux/mctools/simulated_data.h>

// This project :
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

  namespace digitization {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(fake_trigger_cut, "snemo::digitization::fake_trigger_cut");

    fake_trigger_cut::fake_trigger_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults();
      return;
    }

    fake_trigger_cut::~fake_trigger_cut()
    {
      if (is_initialized()) fake_trigger_cut::reset();
      return;
    }

    void fake_trigger_cut::_set_defaults()
    {
      _SD_label_ = snemo::datamodel::data_info::default_simulated_data_label();
    }

    void fake_trigger_cut::set_SD_label(const std::string & SD_label_)
    {
      _SD_label_ = SD_label_;
      return;
    }

    const std::string & fake_trigger_cut::get_SD_label() const
    {
      return _SD_label_;
    }

    void fake_trigger_cut::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Cut '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      if (_algo_.get() != 0) {
	if (_algo_->is_initialized()) {
	  _algo_->reset();
	}
	_algo_.reset();
      }
      _set_defaults();
      return;
    }

    void fake_trigger_cut::initialize(const datatools::properties & configuration_,
				      datatools::service_manager  & /* service_manager_ */,
				      cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      i_cut::_common_initialize(configuration_);

      if (_SD_label_.empty()) {
        if (configuration_.has_key("SD_label")) {
          set_SD_label(configuration_.fetch_string("SD_label"));
        } else {
          set_SD_label(snemo::datamodel::data_info::default_simulated_data_label());
        }
      }
      // Algo :
      _algo_.reset(new snemo::digitization::fake_trigger_algo);
      _algo_->initialize();
      // Tag the cut as initialized :
      _set_initialized(true);
      return;
    }

    int fake_trigger_cut::_accept()
    {
      int cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has(_SD_label_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _SD_label_ << "' bank !");
        return cut_returned;
      }

      // Get simulated data bank
      const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(_SD_label_);

      bool result_for_a_SD = _algo_->process(SD);
      if (!result_for_a_SD) cut_returned = cuts::SELECTION_REJECTED;
      else cut_returned = cuts::SELECTION_ACCEPTED;
            
      return cut_returned;
    }

  } // end of namespace digitization

} // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::digitization::fake_trigger_cut, ocd_)
{
  ocd_.set_class_name("snemo::digitization::fake_trigger_cut");
  ocd_.set_class_description("Cut based on criteria applied to the simulated data bank stored in the event record");
  ocd_.set_class_library("falaise");
  ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
      .set_terse_description("The label/name of the 'simulated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used  \n"
                            "after a simulation. \n")
      .set_default_value_string(snemo::datamodel::data_info::default_simulated_data_label())
      .add_example("Use an alternative name for the \n"
                   "'simulated data' bank::        \n"
                   "                                \n"
                   "  SD_label : string = \"SD2\"   \n"
                   "                                \n"
                   )
      ;
  }

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::digitization::fake_trigger_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::digitization::fake_trigger_cut, "snemo::digitization::fake_trigger_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
