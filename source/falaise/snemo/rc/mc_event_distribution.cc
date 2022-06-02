/// \file falaise/snemo/rc/mc_event_distribution.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-16
/// Last modified: 2022-05-16
///

// Ourselves:
#include <falaise/snemo/rc/mc_event_distribution.h>

// Standard library:
#include <set>
#include <fstream>

// Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace rc {

    mc_event_distribution::reader::reader(std::string filepath_, std::uint32_t nb_events_)
    {
      _fin_ = std::ifstream(filepath_);
      DT_THROW_IF(!_fin_, std::runtime_error, "Cannot open input file '" << filepath_ << "'!");
      _filepath_ = filepath_;
      // Parse and check header:
      {
        std::string line;
        std::getline(_fin_, line);
        if (line != "!falaise::simrc::mc-event-timestamps") {
          DT_THROW(std::logic_error, "Input file '" << filepath_ << "' is not a MC event timestamps file!");
        }
      }
      {
        std::string line;
        std::getline(_fin_, line);
        std::istringstream liness(line);
        std::uint32_t nbOfTimestamps = 0;
        liness >> nbOfTimestamps;
        if (! liness) {
          DT_THROW(std::logic_error, "Cannot parse the number of MC event timestamps from input file '" << filepath_ << "'!");
        }
        DT_THROW_IF(nbOfTimestamps != nb_events_, std::logic_error, "Number of stored timestamps (=" << nbOfTimestamps << ") does not match the requested number of timestamps (=" << nb_events_ << ")!")
      }
      _nb_events_ = nb_events_;
      _fin_ >> std::ws;
      if (_fin_.eof()) {
        _terminated_ = true;
      }
      return;
    }
    
    bool mc_event_distribution::reader::is_terminated() const
    {
      return _terminated_;
    }
    
    void mc_event_distribution::reader::read(event_rc_timestamping & ts_)
    {
      DT_THROW_IF(is_terminated(), std::logic_error, "Event timestamp reader is temrinated!")
      std::string line;
      std::getline(_fin_, line);
      if (line.empty()) {
        DT_THROW(std::logic_error, "Cannot fetch a new line!");
      }
      std::istringstream liness(line);
      std::int32_t eventCounter = -1;
      std::int32_t runId = -1;
      time::time_point decayTimestamp;
      liness >> eventCounter >> runId >> std::ws;
      if (!liness) {
        DT_THROW(std::logic_error, "Invalid format for next decay timestamping record ('" << line << "')!");
      }
      if (runId < 0) {
        DT_THROW(std::logic_error, "Invalid run ID value (" << runId << ")!");
      }
      std::string timestampRepr;
      std::getline(liness, timestampRepr);
      decayTimestamp = time::time_point_from_string(timestampRepr);
      if (not time::is_valid(decayTimestamp)) {
        DT_THROW(std::logic_error, "Invalid format for timestamp ('" << timestampRepr << "')!");
      }
      ts_.run_id = runId;
      ts_.decay_time = decayTimestamp;
      _counter_++;
      _fin_ >> std::ws;
      if (_fin_.eof()) {
        _terminated_ = true;
      }
      return;
    }
    
    mc_event_distribution::writer::writer(std::string filepath_, std::uint32_t nb_events_)
      : _fout_(filepath_)
    {
      DT_THROW_IF(!_fout_, std::runtime_error, "Cannot open output file '" << filepath_ << "'!");
      _fout_ << "!falaise::simrc::mc-event-timestamps" << '\n';
      _fout_ << nb_events_ << '\n';
      return;
    }
    
    void mc_event_distribution::writer::write(const event_rc_timestamping & ts_) 
    {
      _fout_ << _counter_ << ' ' << ts_.run_id << ' ' << time::to_string(ts_.decay_time) << '\n';
      _counter_++;
      return;
    }

    mc_event_distribution::mc_event_distribution(std::uint32_t nb_events_,
                                                 datatools::logger::priority logging_)
    {
      _logging_ = logging_;      
      _nb_events_ = nb_events_;
      return;
    }

    // static
    std::unique_ptr<mc_event_distribution>
    mc_event_distribution::make_regular_sampling(const run_statistics & run_stats_,
                                                 std::uint32_t nb_events_,
                                                 datatools::logger::priority logging_)
    {
      DT_LOG_DEBUG(logging_, "Make a regular non-stochastic sampler");
      auto u = std::make_unique<mc_event_distribution>(nb_events_, logging_);
      u->_run_stats_ = &run_stats_;
      u->_seed_ = 0;
      u->_precompute_sampling_();
      return u;
    }

    // static
    std::unique_ptr<mc_event_distribution>
    mc_event_distribution::make_random_sampling(const run_statistics & run_stats_,
                                                std::uint32_t nb_events_,
                                                unsigned long seed_,
                                                datatools::logger::priority logging_)
    {
      DT_LOG_DEBUG(logging_, "Make a stochastic sampler");
      auto u = std::make_unique<mc_event_distribution>(nb_events_, logging_);
      u->_run_stats_ = &run_stats_;
      DT_THROW_IF(seed_ == 0, std::logic_error, "Invalid random seed!");
      u->_seed_ = seed_;
      u->_T_ = gsl_rng_taus;
      u->_prng_ = gsl_rng_alloc(u->_T_);
      gsl_rng_set(u->_prng_, (unsigned long int) u->_seed_);
      u->_precompute_sampling_();
      return u;
    }

    // static
    std::unique_ptr<mc_event_distribution>
    mc_event_distribution::make_from_file(std::uint32_t nb_events_,
                                          const std::string & path_,
                                          datatools::logger::priority logging_)
    {
      DT_LOG_DEBUG(logging_, "Make a sampling from pre-computed file");
      auto u = std::make_unique<mc_event_distribution>(nb_events_, logging_);
      std::string timestampsPath(path_);
      datatools::fetch_path_with_env(timestampsPath);
      u->_reader_.reset(new reader(timestampsPath, nb_events_));
      u->_parsed_next_decay_.run_id = -1;
      u->_load_next_decay_from_file_();     
      return u;      
    }

    mc_event_distribution::~mc_event_distribution()
    {
      if (_reader_) {
        _reader_.reset();
      }
      if (_prng_ != nullptr) {
        gsl_rng_free(_prng_);
        _T_ = nullptr;
      }
      return;
    }
 
    void mc_event_distribution::_precompute_sampling_()
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      logging = _logging_;
      auto nbRuns = _run_stats_->_records_.size();
      std::vector<double> probPerRun;
      probPerRun.assign(nbRuns, 0.0);
      for (unsigned int iRec = 0; iRec < nbRuns; iRec++) {
        probPerRun[iRec] = _run_stats_->_records_[iRec].probability;
      }
      _nb_decays_per_run_.assign(nbRuns, 0);
      if (_prng_ != nullptr) {
        gsl_ran_multinomial(_prng_, nbRuns, _nb_events_,
                            probPerRun.data(), _nb_decays_per_run_.data());
      } else {
        std::uint32_t sumNbDecays = 0;
        for (unsigned int iRec = 0; iRec < nbRuns - 1; iRec++) { 
          std::uint32_t nbDecays = _nb_events_ * probPerRun[iRec];
          _nb_decays_per_run_[iRec] = nbDecays;
          sumNbDecays += nbDecays;
        }
        _nb_decays_per_run_.back() = _nb_events_ - sumNbDecays;
      }
      _per_run_data_.reserve(nbRuns);
      for (unsigned int iRec = 0; iRec < nbRuns; iRec++) {
        {
          data_per_run dpr;
          _per_run_data_.push_back(dpr);
        }
        data_per_run & dpr = _per_run_data_.back();
        dpr.run_id = _run_stats_->_records_[iRec].run_id;
        auto nbDecays = _nb_decays_per_run_[iRec];
        dpr.decay_timestamps.reserve(nbDecays);
        DT_LOG_DEBUG(logging, "Run #" << dpr.run_id << " has " << nbDecays << " decays");   
      }
      _current_run_index_ = 0;
      _current_decay_index_ = 0;
      _current_decay_index_in_run_ = 0;

      time::time_point refTime = time::invalid_point();

      const std::vector<snemo::rc::run_statistics::record_type> & runRecords
        = _run_stats_->records();
      std::string frandomFilename = "decay_times.data";
      std::ofstream frandom(frandomFilename);
      for (unsigned int iRec = 0; iRec < nbRuns; iRec++) {
        const auto & runRec = runRecords[iRec];
        auto runId = runRec.run_id;
        run_statistics::inverse_cumul_dist inverseCumul(runRec.cumul_dist);
        unsigned int runNbDecays = _nb_decays_per_run_[iRec];
        const auto & runDesc = _run_stats_->runs().get_run(runId);
        std::set<time::time_point> decayTimes;
        if (not time::is_valid(refTime)) {
          refTime = runDesc.begin();
        }
        double pStep = 1.0 / runNbDecays;
        for (unsigned int iDecay = 0; iDecay < runNbDecays; iDecay++) {
          if (_prng_ != nullptr) {
            // DT_LOG_DEBUG(_logging_, "Running stochastic engine...");
            while (true) {
              double p = gsl_ran_flat(_prng_, 0.0, 1.0);
              double tDecay = inverseCumul(p) / CLHEP::microsecond;
              long nbMuSeconds = (long) tDecay;
              time::time_point decayTime = runDesc.begin() + time::microseconds(nbMuSeconds);
              if (runDesc.contains(decayTime)) {
                decayTimes.insert(decayTime);
                break;
              }
            }
          } else {
            // DT_LOG_DEBUG(_logging_, "Running deterministic engine...");
            double p = (iDecay + 0.5) * pStep;
            double tDecay = inverseCumul(p) / CLHEP::microsecond;
            long nbMuSeconds = (long) tDecay;
            time::time_point decayTime = runDesc.begin() + time::microseconds(nbMuSeconds);
            if (not runDesc.contains(decayTime)) {
              DT_LOG_DEBUG(_logging_, "Decay time not in run...");
              for (unsigned int iBreak = 0; iBreak < runDesc.breaks().size(); iBreak++) {
                const time::time_period & aBreak = runDesc.breaks()[iBreak];
                if (aBreak.contains(decayTime)) {
                  DT_LOG_DEBUG(_logging_, "Decay time located in break #" << iBreak << ", shifting it...");
                  decayTime = aBreak.end();
                  if (not runDesc.contains(decayTime)) {
                    decayTime = runDesc.slices()[iBreak].end();
                  }
                  break;
                }
              }
            }
            decayTimes.insert(decayTime);
          }
        }
        DT_LOG_DEBUG(logging, "Run #" << runId << " : number of generated decay times = " << decayTimes.size());   
        DT_LOG_DEBUG(logging, "Array pre-size : " << _per_run_data_[iRec].decay_timestamps.capacity());   
        for (auto decayTime : decayTimes) {
          _per_run_data_[iRec].run_id = runId;
          _per_run_data_[iRec].decay_timestamps.emplace_back(decayTime);
          frandom << time::to_quantity(decayTime - refTime) / CLHEP::microsecond << " " << runId << '\n';
        }
      }
      frandom.close();
      return;
    }

    bool mc_event_distribution::has_next_decay() const
    {
      if (_reader_) {
        // Reader mode:
        if (_parsed_next_decay_.run_id == -1) return false;
      } else {
        // Sampling modes (random, regular)
        if (_current_decay_index_ == _nb_events_) return false;
      }
      return true;
    }
   
    void mc_event_distribution::_load_next_decay_from_file_()
    {
      DT_THROW_IF(! _reader_, std::logic_error, "No input file reader is defined!");
      DT_THROW_IF(_reader_->is_terminated(), std::logic_error, "Reader is terminated!");
      _reader_->read(_parsed_next_decay_);
      return;
    }
      
    mc_event_distribution::event_rc_timestamping mc_event_distribution::next_decay()
    {
      if (_reader_) {
        DT_THROW_IF(_parsed_next_decay_.run_id == -1, std::logic_error,
                    "All decay timestamps have been consumed from the file!");
        event_rc_timestamping decayRec;
        decayRec.run_id = _parsed_next_decay_.run_id;
        decayRec.decay_time = _parsed_next_decay_.decay_time;
        _parsed_next_decay_.run_id = -1;
        _parsed_next_decay_.decay_time = time::invalid_point();
        if (not _reader_->is_terminated()) {
          _load_next_decay_from_file_();
        }
        // Update:
        _current_decay_index_++;
        return decayRec;
      }
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      logging = _logging_;
      DT_THROW_IF(_current_decay_index_ == _nb_events_, std::logic_error,
                  "All decay timestamps have been consumed!");
      const data_per_run & dpr = _per_run_data_[_current_run_index_];
      std::int32_t currentRunId = dpr.run_id;
      DT_LOG_DEBUG(logging, "currentRunId=" << currentRunId);
      const time::time_point & decayTime
        = dpr.decay_timestamps[_current_decay_index_in_run_];
      event_rc_timestamping decayRec;
      decayRec.run_id = currentRunId;
      decayRec.decay_time = decayTime;
      // Update:
      _current_decay_index_++;
      _current_decay_index_in_run_++;
      if (_current_decay_index_in_run_ == _nb_decays_per_run_[_current_run_index_]) {
        DT_LOG_DEBUG(logging, "Run increment after " << _current_decay_index_in_run_ << " decays!");
        _current_run_index_++;
        _current_decay_index_in_run_ = 0;
      }
      return decayRec;
    }
    
    // virtual
    void mc_event_distribution::print_tree(std::ostream & out_,
                                           const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Total number of MC decays : "
           << _nb_events_
           << std::endl;

      if (_reader_) {      
        out_ << popts.indent << tag
             << "Use input file for precomputed event timestamps"
             << std::endl;
      } else {
        out_ << popts.indent << tag
             << "Number of MC decays per run: "
             << std::endl;
        {
          std::uint32_t count = 0;
          for (std::uint32_t nDecays : _nb_decays_per_run_) {
            out_ << popts.indent << skip_tag;
            if (count + 1 == _nb_decays_per_run_.size()) {
              out_ << popts.indent << last_tag;
            } else {
              out_ << popts.indent << tag;
            }
            out_ << "#decays : " << nDecays;
            out_ << std::endl;
            count++; 
          }
        }
        out_ << popts.indent << tag
             << "Seed : "
             << _seed_
             << std::endl;
      
        out_ << popts.indent << tag
             << "PRNG : "
             << _prng_
             << std::endl;
      
        out_ << popts.indent << tag
             << "Per run data: "
             << std::endl;
        {
          std::uint32_t count = 0;
          for (const data_per_run & prd : _per_run_data_) {
            out_ << popts.indent << skip_tag;
            if (count + 1 == _per_run_data_.size()) {
              out_ << popts.indent << last_tag;
            } else {
              out_ << popts.indent << tag;
            }
            out_ << "Run #" << prd.run_id << " : " << prd.decay_timestamps.size() << " decay times = [" ;
            if (prd.decay_timestamps.size() > 0) {
              out_ << time::to_string(prd.decay_timestamps.front());
            }
            if (prd.decay_timestamps.size() > 1) {
              out_ << " ... " << time::to_string(prd.decay_timestamps.back());
            }
            out_ << ']' << std::endl;
            count++; 
          }
        }
      }
      
      out_ << popts.indent << tag
           << "Current run index    = " << _current_run_index_
           << std::endl;
     
      out_ << popts.indent << tag
           << "Current decay index  = " <<_current_decay_index_
           << std::endl;
     
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Current decay index in run = " << _current_decay_index_in_run_
           << std::endl;
 
      return;
    }

  }  // end of namespace rc

}  // end of namespace snemo
