// electromagnetic_field.cc

// Ourselves:
#include <mctools/g4/electromagnetic_field.h>

// Standard library:
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>
#include <geomtools/manager.h>
// - Bayeux/emfield:
#include <emfield/base_electromagnetic_field.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/emfield_geom_plugin.h>

// This project:
#include <mctools/g4/em_field_g4_utils.h>

namespace mctools {

  namespace g4 {

    auto electromagnetic_field::has_name() const -> bool
    {
      return ! _name_.empty();
    }

    void electromagnetic_field::set_name(const std::string & name_)
    {
      _name_ = name_;
   }

    auto electromagnetic_field::get_name() const -> const std::string &
    {
      return _name_;
    }

    void electromagnetic_field::set_field_check_pos_time(bool c_)
    {
      _field_check_pos_time_ = c_;
   }

    auto electromagnetic_field::is_field_check_pos_time() const -> bool
    {
      return _field_check_pos_time_;
    }

    auto electromagnetic_field::is_initialized() const -> bool
    {
      return _initialized_;
    }

    auto electromagnetic_field::has_field() const -> bool
    {
      return _field_ != nullptr;
    }

    void electromagnetic_field::set_field(const emfield::base_electromagnetic_field & f_)
    {
      _field_ = &f_;
   }

    auto
    electromagnetic_field::get_field() const -> const emfield::base_electromagnetic_field &
    {
      DT_THROW_IF (! has_field(), std::logic_error, "No electromagnetic field !");
      return *_field_;
    }

    auto electromagnetic_field::is_active() const -> bool
    {
      return has_field();
    }

    void electromagnetic_field::_set_defaults()
    {
      _field_check_pos_time_ = true;
      geomtools::invalidate(_standalone_constant_mag_field_);
      geomtools::invalidate(_standalone_constant_electric_field_);
   }

    void electromagnetic_field::reset()
    {
      DT_THROW_IF (!_initialized_,std::logic_error,
                   "Geant4 electromagnetic field '" << get_name() << "' is not initialized !");

      _initialized_ = false;
      _name_.clear();
      _field_ = nullptr;
      _set_defaults();
   }

    electromagnetic_field::electromagnetic_field()
    {
      _initialized_ = false;
      _field_ = nullptr;
      _set_defaults();
   }

    electromagnetic_field::~electromagnetic_field()
    {
      if (_initialized_) {
        reset();
      }
         }

    void electromagnetic_field::initialize()
    {
      datatools::properties empty;
      initialize(empty);
   }

    void electromagnetic_field::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF (_initialized_,std::logic_error,
                   "Geant4 electromagnetic field '" << get_name() << "' is already initialized !");

      loggable_support::_initialize_logging_support(config_);

      if (_name_.empty()) {
        if (config_.has_key("name")) {
          std::string name = config_.fetch_string("name");
          set_name(name);
        }
      }

      if (! has_field()) {
        double magnetic_field_unit = CLHEP::tesla;
        double electric_field_unit = CLHEP::volt / CLHEP::meter;

        int mode = STANDALONE_UNDEFINED;
        if (config_.has_key("mode")) {
          std::string mode_str = config_.fetch_string("mode");
          if (mode_str == "standalone_magnetic_field") {
            mode = STANDALONE_MAGNETIC_FIELD;
            if (mode_str == "standalone_electric_field") {
              mode = STANDALONE_ELECTRIC_FIELD;
            } else {
              DT_THROW(std::logic_error,
                       "Invalid value '" << mode_str << "' for 'mode' property in Geant4 electromagnetic field '" << get_name() << "' !");
            }
          } else {
            DT_THROW(std::logic_error, "Missing 'mode' property in Geant4 electromagnetic field '" << get_name() << "' !");
          }

          if (mode == STANDALONE_ELECTRIC_FIELD) {
            if (config_.has_key("electric_field_unit")) {
              std::string efu = config_.fetch_string("electric_field_unit");
              electric_field_unit = datatools::units::get_electric_field_unit_from(efu);
            }

            if (! geomtools::is_valid(_standalone_constant_electric_field_)) {
              if (config_.has_key("standalone_constant_electric_field")) {
                std::vector<double> seg;
                config_.fetch("standalone_constant_electric_field", seg);
                DT_THROW_IF (seg.size() != 3, std::logic_error,
                             "Invalid dimension for vector of constant electric field coordinates in Geant4 electric field '" << get_name() << "' !");
                _standalone_constant_electric_field_.set(seg[0], seg[1], seg[2]);
                if (! config_.has_explicit_unit("standalone_constant_electric_field")) {
                  _standalone_constant_electric_field_ *= electric_field_unit;
                  DT_LOG_NOTICE(_logprio(), "Forcing unit for standalone constant electric field in Geant4 electric field '" << get_name() << "'...");
                }
              }
            }
          }

          if (mode == STANDALONE_MAGNETIC_FIELD) {
            if (config_.has_key("magnetic_field_unit")) {
              std::string mfu = config_.fetch_string("magnetic_field_unit");
              magnetic_field_unit = datatools::units::get_magnetic_field_unit_from(mfu);
            }

            if (! geomtools::is_valid(_standalone_constant_mag_field_)) {
              if (config_.has_key("standalone_constant_mag_field")) {
                std::vector<double> smg;
                config_.fetch("standalone_constant_mag_field", smg);
                DT_THROW_IF (smg.size() != 3, std::logic_error,
                             "Invalid dimension for vector of constant magnetic field coordinates in Geant4 magnetic field '" << get_name() << "' !");
                _standalone_constant_mag_field_.set(smg[0], smg[1], smg[2]);
                if (! config_.has_explicit_unit("standalone_constant_mag_field")) {
                  _standalone_constant_mag_field_ *= magnetic_field_unit;
                  DT_LOG_NOTICE(_logprio(), "Forcing unit for standalone constant magnetic field in Geant4 magnetic field '" << get_name() << "'...");
                }
              }
            }
          }

          /*
            if (mode == 2) {
            //! geomtools::is_valid(_standalone_constant_mag_field_))
            if (! has_mag_field_manager()) {
            if (_geom_manager_ == 0) {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "Missing geometry manager !";
            throw std::logic_error(message.str());
            }
            std::string mag_field_plugin_name;
            if (config_.has_key("electromagnetic_field.geom_plugin")) {
            mag_field_plugin_name = config_.fetch_string("electromagnetic_field.geom_plugin");
            }
            else {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "Missing field plugin name property '" << "electromagnetic_field.geom_plugin" << "' !";
            throw std::logic_error(message.str());
            }

            if (_geom_manager_->has_plugin(mag_field_plugin_name)
            && _geom_manager_->is_plugin_a<emfield::emfield_geom_plugin>(mag_field_plugin_name)) {
            std::clog << "NOTICE: " << "Found EM field geometry plugin named '"
            << mag_field_plugin_name
            << "'" << std::endl;
            const emfield::emfield_geom_plugin & EMFGP
            = _geom_manager_->get_plugin<emfield::emfield_geom_plugin>(mag_field_plugin_name);
            _mag_field_manager_ = &EMFGP.get_manager();
            }
            else {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "No EM field geometry plugin named '"
            << mag_field_plugin_name << "' !";
            throw std::logic_error(message.str());
            }
            }

            if (config_.has_key("electromagnetic_field.name")) {
            _mag_field_name_ = config_.fetch_string("electromagnetic_field.name");
            }
            else {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "Missing field name property '" << "electromagnetic_field.name" << "' !";
            throw std::logic_error(message.str());
            }

            if (! _mag_field_manager_->has_field(_mag_field_name_)) {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "No field named '" << _mag_field_name_ << "' !";
            throw std::logic_error(message.str());
            }
            _mag_field_ = &_mag_field_manager_->get_field(_mag_field_name_);

            if (! _mag_field_->is_electromagnetic_field()) {
            std::ostringstream message;
            message << "mctools::g4::electromagnetic_field::initialize: "
            << "Field '" << _mag_field_name_ << "' is not a magnetic field !";
            throw std::logic_error(message.str());
            }
            }
          */

        }
      }

      if (config_.has_flag("electromagnetic_field.check_pos_time")) {
        _field_check_pos_time_ = true;
      }

      _initialized_ = true;
   }

    // G4 interface:
    auto electromagnetic_field::DoesFieldChangeEnergy() const -> G4bool
    {
      G4bool val = true;
      if (_field_ != nullptr) {
        if (_field_->is_magnetic_field()) {
          val = false;
        }
        if (_field_->is_electric_field()) {
          val = true;
        }
      } else if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        val = false;
      } else if (geomtools::is_valid(_standalone_constant_electric_field_)) {
        val = true;
      }
      return val;
    }

    // G4 interface:
    void electromagnetic_field::GetFieldValue(const double position_[4],
                                              double * em_field_) const
    {
      DT_LOG_TRACE(_logprio(), "Entering GetFieldValue for Geant4 electromagnetic field '" << get_name() << "'...");
      DT_THROW_IF (! _initialized_, std::logic_error,
                   "Geant4 electromagnetic field '" << get_name() << "' is not initialized !");
      em_field_[EMFIELD_BX] = 0.0;
      em_field_[EMFIELD_BY] = 0.0;
      em_field_[EMFIELD_BZ] = 0.0;
      em_field_[EMFIELD_EX] = 0.0;
      em_field_[EMFIELD_EY] = 0.0;
      em_field_[EMFIELD_EZ] = 0.0;
      if (_field_ != nullptr) {
        geomtools::vector_3d pos(position_[POSTIME_X], position_[POSTIME_Y], position_[POSTIME_Z]);
        double time = position_[POSTIME_T];
        DT_LOG_TRACE(_logprio(), "Compute electromagnetic field at position/time "
                     << pos / CLHEP::mm << " [mm] / " << time / CLHEP::ns << " [ns]"
                     << " for Geant4 electromagnetic field '" << get_name() << "'...");
        if (_field_check_pos_time_) {
          DT_THROW_IF(! _field_->position_and_time_are_valid(pos, time),
                      std::logic_error,
                      "Position and time at "
                      << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] "
                      << " are not valid for electromagnetic field named '" << _name_
                      << "' !");
        }
        if (_field_->is_magnetic_field()) {
          DT_LOG_TRACE(_logprio(), "Compute magnetic field contribution...");
          geomtools::vector_3d the_magnetic_field;
          int status = _field_->compute_magnetic_field(pos, time, the_magnetic_field);
          DT_THROW_IF (status != 0,  std::logic_error,
                       "Magnetic field named '" << _name_
                       << "' cannot compute magnetic field value at "
                       << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] !");
          DT_LOG_TRACE(_logprio(), "Geant4 magnetic field '" << get_name() << "' is : " << the_magnetic_field / CLHEP::gauss << " gauss");
          em_field_[EMFIELD_BX] = the_magnetic_field.x();
          em_field_[EMFIELD_BY] = the_magnetic_field.y();
          em_field_[EMFIELD_BZ] = the_magnetic_field.z();
        }
        if (_field_->is_electric_field()) {
          DT_LOG_TRACE(_logprio(), "Compute electric field contribution...");
          geomtools::vector_3d the_electric_field;
          int status = _field_->compute_electric_field(pos, time, the_electric_field);
          DT_THROW_IF (status != 0,  std::logic_error,
                       "Electric field named '" << _name_
                       << "' cannot compute electric field value at "
                       << pos / CLHEP::mm << " [mm] / " << time / CLHEP::nanosecond << " [ns] !");
          DT_LOG_TRACE(_logprio(), "Geant4 electric field '" << get_name() << "' is : " << the_electric_field / (CLHEP::volt/CLHEP::meter) << " V/m");
          em_field_[EMFIELD_EX] = the_electric_field.x();
          em_field_[EMFIELD_EY] = the_electric_field.y();
          em_field_[EMFIELD_EZ] = the_electric_field.z();
        }
      } else if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        em_field_[EMFIELD_BX] = _standalone_constant_mag_field_.x();
        em_field_[EMFIELD_BY] = _standalone_constant_mag_field_.y();
        em_field_[EMFIELD_BZ] = _standalone_constant_mag_field_.z();
      } else if (geomtools::is_valid(_standalone_constant_electric_field_)) {
        em_field_[EMFIELD_EX] = _standalone_constant_electric_field_.x();
        em_field_[EMFIELD_EY] = _standalone_constant_electric_field_.y();
        em_field_[EMFIELD_EZ] = _standalone_constant_electric_field_.z();
      }
      DT_LOG_TRACE(_logprio(), "Exiting GetFieldValue for Geant4 electromagnetic field '" << get_name() << "'.");
         }

    void electromagnetic_field::dump(std::ostream & out_) const
    {
      out_ << "electromagnetic_field:" << std::endl;
      out_ << "|-- Name                   : '" << _name_ << "'" << std::endl;
      if (geomtools::is_valid(_standalone_constant_mag_field_)) {
        out_ << "|-- Standalone constant magnetic field : "
             << _standalone_constant_mag_field_ / CLHEP::gauss
             << " gauss" << std::endl;
      } else if (geomtools::is_valid(_standalone_constant_electric_field_)) {
        out_ << "|-- Standalone constant electric field : "
             << _standalone_constant_electric_field_ / (CLHEP::volt/CLHEP::meter)
             << " V/m" << std::endl;
      } else {
        out_ << "|-- Electromagnetic field         : " << _field_ << std::endl;
      }
      out_ << "`-- Check field pos/time   : " << _field_check_pos_time_ << std::endl;
   }

  } // end of namespace g4

} // end of namespace mctools
