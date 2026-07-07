
// Ourselves:
#include "falaise/snemo/processing/calo_signal_baseline.h"

#include <bayeux/mygsl/mean.h>

namespace snemo {

  namespace processing {

    // friend
    std::ostream & operator<<(std::ostream & out_,
			      const calo_signal_baseline_measurement & csbm_)
    {
      out_ << '(' << (csbm_.value / CLHEP::volt) << " +/- " << csbm_.sigma / CLHEP::volt << ')' << " V ";
      return out_;
    }

    calo_signal_baseline_measurement
    calo_signal_baseline_calculator::operator()(const calo_signal_waveform_type & udd_calo_waveform_) const
    {
      bool onlyFirstBlock = false;
      // onlyFirstBlock = true;
      calo_signal_baseline_measurement baselineMeas;
      auto nbSamples = udd_calo_waveform_.size();
      std::int16_t nblocks = udd_calo_waveform_.size() / _block_size_;
      DT_LOG_DEBUG(verbosity, "Number of signal samples : " << nbSamples);
      DT_LOG_DEBUG(verbosity, "Max number of blocks = " << nblocks);
      mygsl::arithmetic_mean statBlock;
      std::vector<std::pair<double, double>> blockStats;
      blockStats.reserve(nblocks);
      int iBlock = 0;
      bool signalCandidate = false;
      for (auto iSample = 0u; iSample < nbSamples; iSample++) {
	double sampleTime = iSample * _calo_tdc2ns_;
	double sampleAdc = (udd_calo_waveform_[iSample] - _calo_adczero_) * _calo_adc2volt_;
	statBlock.add(sampleAdc);
	if (((iSample + 1) % _block_size_) == 0) {
          // For each last sample in the block:
          DT_LOG_DEBUG(verbosity, "End of block at sample #" << iSample);
	  double meanBlock = statBlock.get_mean();
          bool besselCorrection = true;
          double varBlock = statBlock.get_variance(besselCorrection);
          double meanVarBlock = varBlock / statBlock.get_n();
          double meanSigmaBlock = std::sqrt(meanVarBlock);
          DT_LOG_DEBUG(verbosity, "Block #" << iBlock << " (t=" << sampleTime << " ns) mu=" << meanBlock << " sig_mu=" << meanSigmaBlock);
          std::pair<double, double> p(meanBlock, meanSigmaBlock);
          blockStats.push_back(p);
          if (iBlock > 0) {
            double meanBlockPrevious = blockStats[iBlock -1].first;
            double meanSigmaBlockPrevious = blockStats[iBlock -1].second;
	    double effectiveSigma = std::max(meanSigmaBlock, meanSigmaBlockPrevious);
	    DT_LOG_DEBUG(verbosity, "effectiveSigma = " << effectiveSigma);
	    static const double adcResolution = _calo_adc2volt_ / std::sqrt(3.0);
	    if (effectiveSigma < adcResolution) {
              DT_LOG_DEBUG(verbosity, "forcing effectiveSigma = " << effectiveSigma);
	      effectiveSigma = adcResolution;
	    }
            // double combined_sigma = std::sqrt(meanSigmaBlock * meanSigmaBlock +
            //                                   meanSigmaBlock_previous * meanSigmaBlockPrevious);
            if (std::abs(meanBlock - meanBlockPrevious) > _nsigmas_ * effectiveSigma) {
              DT_LOG_DEBUG(verbosity, "Baseline variation detected at block #" << iBlock);
	      signalCandidate = true;
              break;
            }
          }
          statBlock.reset();
          iBlock++;
	  if (onlyFirstBlock and iBlock == 1) {
	    DT_LOG_DEBUG(verbosity, "Break after first block");
	    break;
	  }
        }
      }
      // Use only blocks before the one with the variation:
      DT_LOG_DEBUG(verbosity, "iBlock=" << iBlock);
      int lastBlock = iBlock;
      if (signalCandidate) lastBlock--;
      DT_LOG_DEBUG(verbosity, "Last block before variation #" << lastBlock);
      if (lastBlock > 0) {
        mygsl::arithmetic_mean baselineStatBlock;
        int maxSampleForBaseline = (int) ((lastBlock + 1) * _block_size_ - 1);
        DT_LOG_DEBUG(verbosity, "Max sample for baseline = " << maxSampleForBaseline);
        double maxTimeForBaseline = maxSampleForBaseline * _calo_tdc2ns_;
        if (maxTimeForBaseline < _min_width_time_) {
          // Not enough wide window for high quality baseline computing:
          baselineMeas.quality |= calo_signal_baseline_measurement::quality_poor;
        }
        DT_LOG_DEBUG(verbosity, "Max time for baseline = " << maxTimeForBaseline / CLHEP::ns << " (ns)");
        for (int iSample = 0; iSample < maxSampleForBaseline; iSample++) {
	  double sampleAdc = (udd_calo_waveform_[iSample] - _calo_adczero_) * _calo_adc2volt_;
	  baselineStatBlock.add(sampleAdc);
        }
        baselineMeas.start_time = 0.0;
        baselineMeas.duration = maxTimeForBaseline;
	baselineMeas.nb_samples = baselineStatBlock.get_n();
 	baselineMeas.value = baselineStatBlock.get_mean();
        bool besselCorrection = true;
        double baseline_variance = baselineStatBlock.get_variance(besselCorrection) / baselineStatBlock.get_n();
        // No bias correction of the standard error (considered as negligible < 1%):
        // baseline_.noise_mV = std::sqrt(baselineStatBlock.get_variance(besselCorrection)); 
        baselineMeas.sigma = std::sqrt(baseline_variance);
        DT_LOG_DEBUG(verbosity, "Baseline = " << baselineMeas.value / CLHEP::volt << " +/- " << baselineMeas.sigma / CLHEP::volt << " (V)");
      }
     
      return baselineMeas;
    }
 
    
  }
}


