// Ourselves:
#include <falaise/datasets/dataset.hpp>

// Standard library:
#include <stdexcept>
#include <numeric>
#include <iomanip>

// Third party
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/datasets/dataset_model.hpp>
#include <falaise/datasets/dataset_producer.hpp>
#include <falaise/datasets/dataset_producer.hpp>

namespace snemo {
  namespace datasets {

    dataset::dataset(const instance_id & id_,
		     dataset_model_ref_type dataset_model_)
      : _id_(id_)
      , _model_(dataset_model_)
    {
      return;
    }
    
    dataset::dataset(const instance_id & id_,
		     dataset_model_ref_type dataset_model_,
		     dataset_producer_ref_type producer_,
		     opt_dataset_ref_type input_dataset_opt_)
      : _id_(id_)
      , _model_(dataset_model_)
    {
      auto & theProducer = producer_.get();
      auto & theProducerModel = theProducer.producer_model();
      if (&theProducerModel.output_data_model() != &_model_.get()) {
	DT_THROW(std::logic_error,
		 "Producer's output dataset model " 
		 << std::quoted(theProducerModel.output_data_model().id().to_string())
		 << " does not match the expected " << std::quoted(_model_.get().id().to_string()) << " dataset model");
      }
      _producer_opt_ = producer_;
      if (theProducerModel.has_input_data_model()) {
	if (not input_dataset_opt_.has_value()) {
	  DT_THROW(std::logic_error, "Producer of model " << std::quoted(theProducerModel.id().to_string()) << " requires an input dataset but none is provided");
	}
	auto & theInputDataset = input_dataset_opt_.value().get();
	if (&theProducerModel.input_data_model() != &theInputDataset.model()) {
	  DT_THROW(std::logic_error,
		   "Attempt to pass input data with model " 
		   << std::quoted(theInputDataset.model().id().to_string())
		   << " to the dataset producer " << std::quoted(theProducer.id().to_string()) 
		   << " which requires data model " << std::quoted(theProducerModel.input_data_model().id().to_string()));	  
	}
	_input_dataset_opt_ = input_dataset_opt_;
      }
      return;
    }
  
    const instance_id & dataset::id() const
    {
      return _id_;
    }

    const dataset_model & dataset::model() const
    {
      return _model_.get();
    }
  
    bool dataset::has_producer() const
    {
      return _producer_opt_.has_value();
    }
  
    const dataset_producer & dataset::producer() const
    {
      if (not has_producer()) {
	throw std::logic_error("No producer");
      }
      return _producer_opt_.value();
    }

    bool dataset::has_input_dataset() const
    {
      return _input_dataset_opt_.has_value();
    }

    const dataset & dataset::input_dataset() const
    {
      return _input_dataset_opt_.value();
    }

    bool dataset::has_chunk(const instance_id & id_) const
    {
      for (const auto & item : _chunks_) {
	if (item.id() == id_) return true;
      }
      return false;
    }
 
    void dataset::add_chunk(const datachunk & chunk_)
    {
      DT_THROW_IF(has_chunk(chunk_.id()), std::logic_error,
		  "Chunk ID " << chunk_.id() << "is already used");
      _chunks_.push_back(chunk_);
      return;
    }
  
    void dataset::add_chunk(const instance_id & id_,
			    const std::string & path_,
			    datachunk::opt_nentries_type number_of_entries_opt_)
    {
      add_chunk(datachunk(id_, path_, number_of_entries_opt_));
      return;
    }

    const std::vector<datachunk> & dataset::chunks() const
    {
      return _chunks_;
    }
 
    bool dataset::has_number_of_entries() const
    {
      for (const auto & item : _chunks_) {
	if (not item.has_number_of_entries()) {
	  return false;
	}
      }
      return true;
    }

    std::uint32_t dataset::number_of_entries() const
    {
      if (not has_number_of_entries()) {
	throw std::logic_error("Unknown number of entries");
      }
      return std::accumulate(_chunks_.begin(), _chunks_.end(), 0u,
			     [&](uint32_t sum_, const datachunk & chk_) {
			       return std::move(sum_) + chk_.number_of_entries(); });
    }
 
    void dataset::print_tree(std::ostream & out_) const
    {
      out_ << "|-- " << "ID : " << std::quoted(_id_.to_string()) << " [@" << this << ']' << '\n';
      out_ << "|-- " << "Dataset model : "
	   << std::quoted(_model_.get().id().to_string()) << " [@" << &_model_.get() << ']' << '\n';
      out_ << "|-- " << "Dataset producer : ";
      if (not has_producer()) {
	out_ << "none";
      } else {
	out_ << std::quoted(_producer_opt_.value().get().id().to_string()) << " [@" << &_producer_opt_.value().get() << ']' ;
      }      
      out_ << '\n';
      out_ << "|-- " << "Input dataset : ";
      if (not has_input_dataset()) {
	out_ << "none";
      } else {
	out_ << std::quoted(_input_dataset_opt_.value().get().id().to_string()) << " [@" << &_input_dataset_opt_.value().get() << ']' ;
      }      
      out_ << '\n';

      
      out_ << "|-- " << "Chunks : " << _chunks_.size() << '\n';
      for (auto iChunk = 0u; iChunk <  _chunks_.size(); iChunk++) {
	std::string tag = "|-- ";
	std::string tag2 = "|   ";
	if (iChunk + 1 == _chunks_.size()) {
	  tag  = "`-- ";
	  tag2 = "    ";
	}
	out_ << "|   " << tag << "Chunk[" << iChunk << "] : " << '\n';
	out_ << "|   " << tag2 << "|-- " << "ID : \"" << _chunks_[iChunk].id() << '"' << '\n';
	out_ << "|   " << tag2 << "|-- " << "Path : " << std::quoted(_chunks_[iChunk].path()) << '\n';
	out_ << "|   " << tag2 << "`-- " << "Number of entries : ";
	if (_chunks_[iChunk].has_number_of_entries()) {
	  out_ << _chunks_[iChunk].number_of_entries();
	} else {
	  out_ << "unknown";
	}
	out_ << '\n';
      }
      out_ << "`-- " << "Number of entries : ";
      if (has_number_of_entries()) {
	out_ << number_of_entries();
      } else {
	out_ << "unknown";
      }
      out_ << '\n';
      return;
    }

  } // namespace datasets 
} // namespace snemo 
