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

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(fake_trigger_cut, "snemo::cut::fake_trigger_cut");

    void fake_trigger_cut::_set_defaults()
    {
      _SD_label_ = "";
      return;
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

    fake_trigger_cut::fake_trigger_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults();
      return;
    }

    fake_trigger_cut::~fake_trigger_cut()
    {
      if (is_initialized()) this->fake_trigger_cut::reset();
      return;
    }

    void fake_trigger_cut::reset()
    {
      _set_defaults();
      this->i_cut::_reset();
      this->i_cut::_set_initialized(false);
      return;
    }

    void fake_trigger_cut::initialize(const datatools::properties & configuration_,
				      datatools::service_manager  & /* service_manager_ */,
				      cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      this->i_cut::_common_initialize(configuration_);

      if (_SD_label_.empty()) {
        if (configuration_.has_key("SD_label")) {
          set_SD_label(configuration_.fetch_string("SD_label"));
        } else {
          set_SD_label(snemo::datamodel::data_info::default_simulated_data_label());
        }
      }
      
      this->i_cut::_set_initialized(true);
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
      // cut_returned = cuts::SELECTION_REJECTED;
      // cut_returned = cuts::SELECTION_ACCEPTED;
      }

      return cut_returned;
    }

  } // end of namespace cut

} // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::fake_trigger_cut, ocd_)
{
  ocd_.set_class_name("snemo::cut::fake_trigger_cut");
  ocd_.set_class_description("Cut based on criteria applied to the simulated data bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::cut::fake_trigger_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::fake_trigger_cut, "snemo::cut::fake_trigger_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
