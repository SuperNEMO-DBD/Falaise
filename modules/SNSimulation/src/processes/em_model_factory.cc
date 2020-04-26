// em_model_factory.cc

// Ourselves:
#include <mctools/g4/processes/em_model_factory.h>

// Third party:
// Geant4:
#include <G4Version.hh>

// Base model:
// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4DummyModel.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// EM standard model:
#include <G4BetheBlochIonGasModel.hh>
#include <G4BetheBlochModel.hh>
#include <G4BraggIonModel.hh>
#include <G4ICRU73QOModel.hh>
#include <G4WentzelVIRelModel.hh>
#include <G4hCoulombScatteringModel.hh>
#if G4VERSION_NUMBER >= 1000
#include <G4PAIPhotModel.hh>
#include <G4SeltzerBergerModel.hh>
#include <G4UrbanMscModel.hh>
#else  // Geant4 9.X
#include <G4PAIPhotonModel.hh>
#include <G4PEEffectModel.hh>
#include <G4UrbanMscModel90.hh>
#include <G4UrbanMscModel92.hh>
#include <G4UrbanMscModel93.hh>
#include <G4UrbanMscModel95.hh>
#include <G4UrbanMscModel96.hh>
#include <G4eBremsstrahlungModel.hh>
#endif
#include <G4BetheHeitlerModel.hh>
#include <G4BraggIonGasModel.hh>
#include <G4BraggModel.hh>
#include <G4GoudsmitSaundersonMscModel.hh>
#include <G4ICRU49NuclearStoppingModel.hh>
#include <G4IonCoulombScatteringModel.hh>
#include <G4KleinNishinaModel.hh>
#include <G4MollerBhabhaModel.hh>
#include <G4PAIModel.hh>
#include <G4PEEffectFluoModel.hh>
#include <G4PairProductionRelModel.hh>
#include <G4SeltzerBergerModel.hh>
#include <G4WentzelVIModel.hh>
#include <G4XrayRayleighModel.hh>
#include <G4eBremParametrizedModel.hh>
#include <G4eBremsstrahlungRelModel.hh>
#include <G4eCoulombScatteringModel.hh>
#include <G4eSingleCoulombScatteringModel.hh>
#include <G4eeToTwoGammaModel.hh>

// Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>

namespace snsim {

namespace processes {

em_model_factory::em_model_factory() { _initialized_ = false; }

em_model_factory::~em_model_factory() {
  if (is_initialized()) {
    reset();
  }
}

auto em_model_factory::is_initialized() const -> bool { return _initialized_; }

void em_model_factory::initialize() {
  DT_THROW_IF(is_initialized(), std::logic_error, "EM model factory is already initialized!");
  _registration();
  _initialized_ = true;
}

void em_model_factory::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "EM model factory is not initialized!");
  _initialized_ = false;
}

auto em_model_factory::has_model_type(const std::string &model_type_id_) const -> bool {
  return _reg_.has(model_type_id_);
}

auto em_model_factory::create_model(const std::string &model_type_id_,
                                    const std::string &model_name_) -> G4VEmModel * {
  datatools::properties dummy;
  return create_model(model_type_id_, model_name_, dummy);
}

auto em_model_factory::create_model(const std::string &model_type_id_,
                                    const std::string &model_name_,
                                    const datatools::properties &model_config_) -> G4VEmModel * {
  DT_THROW_IF(!is_initialized(), std::logic_error, "EM model factory is not initialized!");
  G4VEmModel *model = nullptr;

  model = _create_model(model_type_id_, model_name_, model_config_);

  return model;
}

auto em_model_factory::_create_model(const std::string &model_type_id_,
                                     const std::string &model_name_, const datatools::properties &
                                     /* model_config_ */) -> G4VEmModel * {
  G4VEmModel *model = nullptr;

  if (_reg_.has(model_type_id_)) {
    const fact_reg_type::factory_type &f = _reg_.get(model_type_id_);
    model = f();
    _change_model_name_(*model, model_name_);
  }

  return model;
}

// static
void em_model_factory::_change_model_name_(G4VEmModel &model_, const std::string &name_) {
  const G4String *g4name = &model_.GetName();
  auto *mutable_g4name = const_cast<G4String *>(g4name);
  *mutable_g4name = name_;
}

void em_model_factory::_registration() {
  _registration_base();
  _registration_standard();
}

void em_model_factory::_registration_base() { _reg_.registration<G4DummyModel>("Dummy"); }

void em_model_factory::_registration_standard() {
  /*

    $ cd {the-Geant4-base-source-directory}

    Find the EM model class name:
    $ find source/processes/electromagnetic/standard/ -name "G4*Model*.hh" | cut -d/ -f6 | cut -d.
    -f1 > em_model_std.lis

    Automatically build the registration instructions:

    $ cat em_model_std.lis | awk -f process_em_model_list.awk

    with Awk script:
    @code
    BEGIN {
    printf("// List of standard EM models:\n");
    }
    (NF==1){
    class_id=$1;
    gsub("G4","",class_id)
    gsub("Model","",class_id)
    printf("_reg_.registration(\"%s\", boost::factory<%s*>());\n", class_id, $1);
    }
    END{
    printf("// End of list.\n");
    }
    @encode
  */
  // List of standard EM models:
  _reg_.registration<G4BraggIonModel>("BraggIon");
  _reg_.registration<G4ICRU73QOModel>("ICRU73QO");
  _reg_.registration<G4BetheBlochIonGasModel>("BetheBlochIonGas");
  _reg_.registration<G4hCoulombScatteringModel>("hCoulombScattering");
  _reg_.registration<G4WentzelVIRelModel>("WentzelVIRel");
  _reg_.registration<G4BetheBlochModel>("BetheBloch");
  _reg_.registration<G4eBremParametrizedModel>("eBremParametrized");
  _reg_.registration<G4PAIModel>("PAI");
  _reg_.registration<G4WentzelVIModel>("WentzelVI");
#if G4VERSION_NUMBER >= 1000
  _reg_.registration<G4UrbanMscModel>("UrbanMsc");
  _reg_.registration<G4SeltzerBergerModel>("SeltzerBerger");
  _reg_.registration<G4PAIPhotModel>("PAIPhot");
#else
  _reg_.registration<G4UrbanMscModel90>("UrbanMsc90");
  _reg_.registration<G4UrbanMscModel92>("UrbanMsc92");
  _reg_.registration<G4UrbanMscModel93>("UrbanMsc93");
  _reg_.registration<G4UrbanMscModel95>("UrbanMsc95");
  _reg_.registration<G4UrbanMscModel96>("UrbanMsc96");
  _reg_.registration<G4eBremsstrahlungModel>("eBremsstrahlung");
  _reg_.registration<G4PEEffectModel>("PEEffect");
  _reg_.registration<G4PAIPhotonModel>("PAIPhoton");
#endif
  _reg_.registration<G4XrayRayleighModel>("XrayRayleigh");
  _reg_.registration<G4BetheHeitlerModel>("BetheHeitler");
  _reg_.registration<G4eBremsstrahlungRelModel>("eBremsstrahlungRel");
  _reg_.registration<G4KleinNishinaModel>("KleinNishina");
  _reg_.registration<G4IonCoulombScatteringModel>("IonCoulombScattering");
  _reg_.registration<G4PairProductionRelModel>("PairProductionRel");
  _reg_.registration<G4eeToTwoGammaModel>("eeToTwoGamma");
  _reg_.registration<G4eCoulombScatteringModel>("eCoulombScattering");
  _reg_.registration<G4MollerBhabhaModel>("MollerBhabha");
  _reg_.registration<G4BraggIonGasModel>("BraggIonGas");
  _reg_.registration<G4BraggModel>("Bragg");
  _reg_.registration<G4GoudsmitSaundersonMscModel>("GoudsmitSaundersonMsc");
  // _reg_.registration<G4SeltzerBergerModel>("SeltzerBerger");
  _reg_.registration<G4ICRU49NuclearStoppingModel>("ICRU49NuclearStopping");
  _reg_.registration<G4PEEffectFluoModel>("PEEffectFluo");
  _reg_.registration<G4eSingleCoulombScatteringModel>("eSingleCoulombScattering");
}

// static
auto em_model_factory::get_instance() -> const em_model_factory & {
  return const_cast<em_model_factory &>(_instance_());
}

// static
auto em_model_factory::grab_instance() -> em_model_factory & { return _instance_(); }

// static
auto em_model_factory::_instance_() -> em_model_factory & {
  static boost::scoped_ptr<em_model_factory> _fact;
  if (!_fact) {
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Instantation of the static EM model factory...");
    _fact.reset(new em_model_factory);
    _fact->initialize();
  }
  return *_fact;
}

}  // end of namespace processes

}  // namespace snsim
