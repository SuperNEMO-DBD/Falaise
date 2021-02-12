// Ourselves:
#include <GammaTracking/gamma_tracking.h>
// This project
#include <GammaTracking/tof_computing.h>

// Standard library:
#include <algorithm>
#include <iostream>

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
// - Boost:
#include <boost/next_prior.hpp>
// #include <boost/fusion/iterator/next.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>

namespace gt {

gamma_tracking::gamma_tracking() {
  _set_defaults();
  _initialized_ = false;
}

gamma_tracking::gamma_tracking(const gamma_tracking &gt_) {
  _initialized_ = gt_._initialized_;
  _logging_priority_ = gt_._logging_priority_;
  _absolute_ = gt_._absolute_;
  _extern_ = gt_._extern_;
  _max_ = gt_._max_;
  _min_prob_ = gt_._min_prob_;
  _starts_ = gt_._starts_;
  _serie_ = gt_._serie_;
  _min_chi2_ = gt_._min_chi2_;

  for (auto mit = gt_._chi2_.begin(); mit != gt_._chi2_.end(); ++mit) {
    auto it = std::find(_serie_.begin(), _serie_.end(), *(mit->first));
    if (it != _serie_.end()) {
      _chi2_[&(*it)] = mit->second;
    }
  }

  for (auto mit = gt_._proba_.begin(); mit != gt_._proba_.end(); ++mit) {
    auto it = std::find(_serie_.begin(), _serie_.end(), *(mit->first));
    if (it != _serie_.end()) {
      _proba_[&(*it)] = mit->second;
    }
  }
}

gamma_tracking &gamma_tracking::operator=(const gamma_tracking &gt_) {
  this->reset();
  _initialized_ = gt_._initialized_;
  _logging_priority_ = gt_._logging_priority_;
  _absolute_ = gt_._absolute_;
  _extern_ = gt_._extern_;
  _max_ = gt_._max_;
  _min_prob_ = gt_._min_prob_;
  _starts_ = gt_._starts_;
  _serie_ = gt_._serie_;
  _min_chi2_ = gt_._min_chi2_;

  for (auto mit = gt_._chi2_.begin(); mit != gt_._chi2_.end(); ++mit) {
    auto it = std::find(_serie_.begin(), _serie_.end(), *(mit->first));
    if (it != _serie_.end()) {
      _chi2_[&(*it)] = mit->second;
    }
  }

  for (auto mit = gt_._proba_.begin(); mit != gt_._proba_.end(); ++mit) {
    auto it = std::find(_serie_.begin(), _serie_.end(), *(mit->first));
    if (it != _serie_.end()) {
      _proba_[&(*it)] = mit->second;
    }
  }

  return *this;
}

gamma_tracking::~gamma_tracking() {
  if (is_initialized()) {
    reset();
  }
}

void gamma_tracking::set_logging_priority(datatools::logger::priority priority_) {
  _logging_priority_ = priority_;
}

datatools::logger::priority gamma_tracking::get_logging_priority() const {
  return _logging_priority_;
}

bool gamma_tracking::is_initialized() const { return _initialized_; }

void gamma_tracking::set_initialized(bool initialized_) { _initialized_ = initialized_; }

void gamma_tracking::initialize(const datatools::properties &config_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  datatools::logger::priority p = datatools::logger::extract_logging_configuration(
      config_, datatools::logger::PRIO_UNDEFINED, true);
  if (p != datatools::logger::PRIO_UNDEFINED) {
    set_logging_priority(p);
  }

  if (config_.has_flag("use_absolute")) {
    set_absolute(true);
  }

  if (config_.has_flag("use_extern")) {
    set_extern(true);
  }

  if (config_.has_key("minimal_probability")) {
    _min_prob_ = config_.fetch_real("minimal_probability");
  }

  set_initialized(true);
}

void gamma_tracking::_set_defaults() {
  _logging_priority_ = datatools::logger::PRIO_WARNING;
  _max_ = 0;
  _min_prob_ = 1e-5;
  _min_chi2_.insert(std::make_pair(1, gsl_cdf_chisq_Qinv(_min_prob_, 1)));
  _absolute_ = false;
  _extern_ = false;
}

bool gamma_tracking::has_tracks() { return !_serie_.empty(); }

void gamma_tracking::add(int number_) {
  list_type tamp;
  tamp.push_back(number_);

  if (is_inside_serie(tamp)) {
    return;
  }
  _serie_.push_back(tamp);
  _proba_[&(_serie_.back())] = 1.0;
  _chi2_[&(_serie_.back())] = 0.0;
  ++_max_;
}

void gamma_tracking::add_probability(int number1_, int number2_, double proba_) {
  add(number1_);
  add(number2_);

  if (number1_ == number2_) {
    return;
  }

  if (proba_ < _min_prob_) {
    return;
  }

  list_type tamp;
  tamp.push_back(number1_);
  tamp.push_back(number2_);
  if (is_inside_serie(tamp)) {
    return;
  }
  _serie_.push_back(tamp);

  const double chi2 = gsl_cdf_chisq_Qinv(proba_, 1);
  _chi2_[&(_serie_.back())] = chi2;
  _proba_[&(_serie_.back())] = proba_;
}

void gamma_tracking::add_chi2(int number1_, int number2_, double chi2_) {
  if (number1_ == number2_ || chi2_ > get_chi_limit(1)) {
    return;
  }
  const double proba = gsl_cdf_chisq_Q(chi2_, 1);
  add_probability(number1_, number2_, proba);
}

void gamma_tracking::add_start(int number_) {
  bool its_inside = false;
  for (auto &rit : _serie_) {
    its_inside = is_inside(rit, number_);
  }

  if (!its_inside) {
    return;
  }
  _starts_.push_back(number_);
}

void gamma_tracking::dump(std::ostream &out_) const {
  for (auto it = _serie_.begin(); it != _serie_.end(); ++it) {
    if (boost::next(it) == _serie_.end()) {
      out_ << "`- ";
    } else {
      out_ << "|- ";
    }
    out_ << "for list ";
    for (auto iit = it->begin(); iit != it->end(); ++iit) {
      out_ << *iit;
      if (boost::next(iit) != it->end()) {
        out_ << "->";
      }
    }
    out_ << " - probability is " << _proba_.at(&(*it)) << std::endl;
  }
}

bool gamma_tracking::is_inside_serie(const list_type &list_) const {
  return std::find(_serie_.begin(), _serie_.end(), list_) != _serie_.end();
}

bool gamma_tracking::is_inside(const list_type &check_, int value_) const {
  return std::find(check_.begin(), check_.end(), value_) != check_.end();
}

bool gamma_tracking::is_inside(const list_type &check_, const list_type &values_) const {
  for (auto it = values_.begin(); it != values_.end(); ++it) {
    if (std::find(check_.begin(), check_.end(), (*it)) != check_.end()) {
      return true;
    }
  }
  return false;
}

void gamma_tracking::extract(list_type &source_, const list_type &values_) {
  for (auto it = values_.begin(); it != values_.end(); ++it) {
    auto to_extract = std::find(source_.begin(), source_.end(), *it);
    if (to_extract != source_.end()) {
      source_.erase(to_extract);
    }
  }
}

void gamma_tracking::put_inside(const list_type &from_, list_type &to_) {
  for (auto it = from_.begin(); it != from_.end(); ++it) {
    to_.push_back(*it);
  }
  to_.sort();
  to_.unique();
}

void gamma_tracking::set_absolute(bool absolute_) { _absolute_ = absolute_; }

bool gamma_tracking::is_absolute() { return _absolute_; }

void gamma_tracking::set_extern(bool extern_) { _extern_ = extern_; }

bool gamma_tracking::is_extern() { return _extern_; }

void gamma_tracking::set_probability_min(double min_prob_) {
  _min_prob_ = min_prob_;
  for (auto &it : _min_chi2_) {
    it.second = gsl_cdf_chisq_Qinv(_min_prob_, it.first);
  }
}

void gamma_tracking::get_reflects(solution_type &solution_, double prob_list_,
                                  const list_type *starts_, const list_type *exclude_,
                                  bool deathless_starts_) {
  solution_type &to_return = solution_;

  list_type to_exclude;
  if (exclude_ != nullptr) {
    to_exclude = *exclude_;
  }

  list_type starts;
  if (starts_ != nullptr) {
    starts = *starts_;
  }

  put_inside(_starts_, starts);
  sort_probabilities();

  for (solution_type::const_iterator rit = _serie_.begin(); rit != _serie_.end(); ++rit) {
    const list_type &a_list = *rit;
    if (a_list.size() > _max_ - to_exclude.size() || is_inside(a_list, to_exclude) ||
        prob_list_ > _proba_[&a_list]) {
      continue;
    }

    if (starts.empty() || is_inside(starts, *(a_list.begin()))) {
      if (is_extern() && (!is_inside(starts, a_list.front()) ||
                          (a_list.size() == 2 && is_inside(starts, a_list.back())))) {
        continue;
      }

      to_return.push_back(a_list);

      if (!starts.empty() && deathless_starts_) {
        list_type serie = a_list;
        serie.pop_front();
        put_inside(serie, to_exclude);
      } else {
        put_inside(a_list, to_exclude);
      }
    } else {
      if (!is_extern()) {
        put_inside(a_list, to_exclude);
        if (!starts.empty() && deathless_starts_) {
          extract(to_exclude, starts);
        }
      }
    }
  }
}

const gamma_tracking::solution_type &gamma_tracking::get_all() const { return _serie_; }

double gamma_tracking::get_probability(const list_type &scin_ids_) const {
  double probability = datatools::invalid_real();
  auto it = std::find(_serie_.begin(), _serie_.end(), scin_ids_);
  if (it != _serie_.end()) {
    probability = _proba_.at(&(*it));
  }
  return probability;
}

double gamma_tracking::get_probability(int scin_id1_, int scin_id2_) const {
  list_type l1;
  l1.push_back(scin_id1_);
  l1.push_back(scin_id2_);
  return get_probability(l1);
}

double gamma_tracking::get_chi2(const list_type &scin_ids_) const {
  double chi2 = datatools::invalid_real();
  auto it = std::find(_serie_.begin(), _serie_.end(), scin_ids_);
  if (it != _serie_.end()) {
    chi2 = _chi2_.at(&(*it));
  }
  return chi2;
}

double gamma_tracking::get_chi2(int scin_id1_, int scin_id2_) const {
  list_type l1;
  l1.push_back(scin_id1_);
  l1.push_back(scin_id2_);
  return get_chi2(l1);
}

bool gamma_tracking::sort_reflect(const list_type &ref1_, const list_type &ref2_) {
  return ref1_.size() > ref2_.size();
}

void gamma_tracking::sort_probabilities() {
  if (_serie_.size() <= 1) {
    return;
  }

  bool has_changed = true;
  while (has_changed) {
    has_changed = false;
    auto it1 = _serie_.begin();
    auto it2 = _serie_.begin();
    it2++;
    while (it2 != _serie_.end() && !has_changed) {
      if (it1->size() > 1 && it2->size() > 1 && _proba_[&(*it1)] < _proba_[&(*it2)] &&
          (is_absolute() || it1->size() <= it2->size())) {
        has_changed = true;
        _serie_.splice(it1, _serie_, it2);
      } else {
        it1++;
        it2++;
      }
    }
  }
}

double gamma_tracking::get_chi_limit(unsigned int freedom_) {
  if (_min_chi2_.count(freedom_) == 0u) {
    _min_chi2_[freedom_] = gsl_cdf_chisq_Qinv(_min_prob_, freedom_);
  }
  return _min_chi2_[freedom_];
}

const event &gamma_tracking::get_event() const { return _event_; }

event &gamma_tracking::grab_event() { return _event_; }

void gamma_tracking::prepare_process() {
  const event::calorimeter_collection_type &the_gamma_calos = _event_.get_calorimeters();

  if (the_gamma_calos.size() == 1) {
    add(the_gamma_calos.begin()->first);
  } else {
    for (auto icalo = the_gamma_calos.begin(); icalo != the_gamma_calos.end(); ++icalo) {
      for (auto jcalo = boost::next(icalo); jcalo != the_gamma_calos.end(); ++jcalo) {
        auto it1 = icalo->second < jcalo->second ? icalo : jcalo;
        auto it2 = icalo->second < jcalo->second ? jcalo : icalo;
        const double tof_chi2 = tof_computing::get_chi2(it1->second, it2->second);
        const double tof_prob = tof_computing::get_internal_probability(tof_chi2);
        add_probability(it1->first, it2->first, tof_prob);
      }
    }
  }
}

void gamma_tracking::process() {
  bool has_next = false;
  size_t first_loop = 1;
  list_type tamp_list;

  auto l_it1 = _serie_.begin();
  while (l_it1 != _serie_.end()) {
    for (auto l_it2 = _serie_.begin(); l_it2 != _serie_.end(); ++l_it2) {
      if (l_it2->size() == 2 && l_it1->size() > first_loop && l_it1->back() == l_it2->front() &&
          !(is_inside(*l_it1, l_it2->back())) &&
          (_starts_.empty() || is_inside(_starts_, (*(l_it1->begin()))))) {
        bool starts_in = false;

        if (is_extern()) {
          for (auto it = _starts_.begin(); it != _starts_.end(); ++it) {
            if (std::find(++(l_it1->begin()), l_it1->end(), *it) != l_it1->end()) {
              starts_in = true;
              break;
            }

            if (l_it2->back() == *it) {
              starts_in = true;
              break;
            }
          }
        }

        if (starts_in) {
          continue;
        }

        const int freedom = l_it1->size() + l_it2->size() - 2;
        const double chi2 = _chi2_[&(*l_it1)] + _chi2_[&(*l_it2)];
        if (chi2 < get_chi_limit(freedom)) {
          const double the_prob = gsl_cdf_chisq_Q(chi2, freedom);

          tamp_list = (*l_it1);
          tamp_list.insert(tamp_list.end(), ++(l_it2->begin()), l_it2->end());
          if (!is_inside_serie(tamp_list)) {
            has_next = true;
            _serie_.push_front(tamp_list);
          }

          tamp_list.clear();
          _proba_[&(_serie_.front())] = the_prob;  //_proba_[&(*l_it1)]*_proba_[&(*l_it2)];
          _chi2_[&(_serie_.front())] = chi2;
        }
      }
    }

    l_it1++;

    if (l_it1 == _serie_.end() && has_next) {
      l_it1 = _serie_.begin();
      has_next = false;
      first_loop = 2;
    }
  }

  _serie_.sort(sort_reflect);
}

void gamma_tracking::reset() {
  _serie_.clear();
  _proba_.clear();
  _starts_.clear();
  _event_.reset();
}

}  // namespace gt
