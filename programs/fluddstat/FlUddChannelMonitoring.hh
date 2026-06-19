#ifndef FLUDDCHANNELMONITORING_HH
#define FLUDDCHANNELMONITORING_HH

#include <cstdint>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <limits>

#include <gsl/gsl_randist.h>

// Third party:
#include <bayeux/mygsl/mean.h>
#include <bayeux/mygsl/histogram.h>
#include <bayeux/datatools/things.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/clhep_units.h>

// This Project
#include <falaise/snemo/datamodels/geomid_utils.h>
#include <falaise/snemo/datamodels/timestamp.h>
#include <falaise/snemo/datamodels/calorimeter_digitized_hit.h>

namespace FLUddStat {

  struct poisson_pdf
  {
    poisson_pdf(const double lambda_) : _lambda_(lambda_) {}

    double operator()(const std::uint32_t & k_) const
    {
      return gsl_ran_poisson_pdf(k_, _lambda_);
    }
    
    double _lambda_;
   
  };

  struct event_info
  {
    std::uint32_t counts = 0;
    snemo::time::time_point time_point{snemo::time::invalid_point()};
    int period_index = -1;
  };

  struct baseline_info
  {
    mygsl::arithmetic_mean mean;
    double value = std::numeric_limits<double>::quiet_NaN();
    double sigma = std::numeric_limits<double>::quiet_NaN();
    //std::vector<std::uint32_t> nsamples;
  };

  struct om_info
  {
    int om_num = -1;
    geomtools::geom_id gid;
    std::uint32_t lto_counts = 0u;
    std::uint32_t lt_counts = 0u;
    std::uint32_t ht_counts = 0u;
    bool lt_triggered = false;
    std::vector<double> baselines;
    std::vector<double> baselines_err;
    std::vector<std::uint32_t> baselines_nsamples;
    std::map<int, std::uint32_t> counts_per_period;
    std::map<int, baseline_info> baseline_per_period;
    double mean_counts = std::numeric_limits<double>::quiet_NaN();
    double sigma_counts = std::numeric_limits<double>::quiet_NaN();
    bool unstable_rate = false;
    double mean_baseline = std::numeric_limits<double>::quiet_NaN();
    double sigma_baseline = std::numeric_limits<double>::quiet_NaN();
    bool unstable_baseline = false;
  };
  
  struct FlUddChannelMonitoring
  {
  public:

    FlUddChannelMonitoring() = default; 
      
    void initialize(const datatools::properties & config_);

    void process(const datatools::things & event_);

    void terminate();

    void process_calo_hits(const snemo::datamodel::CalorimeterDigiHitHdlCollection & calo_hits_);

    void print(std::ostream & out_) const;

  public:

    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    std::uint32_t _event_counter_;
    snemo::datamodel::timestamp _first_event_timestamp_;
    std::int64_t _run_ref_timestamp_ = 0;
    std::int64_t _last_ref_timestamps_ = 0;
    event_info _event_info_;
    std::vector<om_info> _om_infos_;
    std::uint32_t _max_nb_signal_save_ = 0;
    std::string _out_dir_path_s_;
    std::filesystem::path _out_dir_path_;
    std::filesystem::path _out_path_;
    std::uint32_t _period_duration_ = 300; // second;
    bool _only_ht_hits_ = false;
    std::uint32_t _baseline_min_samples_ = 16; // #samples
   

    snemo::time::time_point _first_event_timepoint_{snemo::time::invalid_point()};
   
  };
  
}

#endif // FLUDDCHANNELMONITORING_HH

