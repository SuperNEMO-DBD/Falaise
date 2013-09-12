// DumbReconstruction - Trivial reconstruction to demo structure
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.
#ifndef DUMBRECONSTRUCTION_HH
#define DUMBRECONSTRUCTION_HH
// Standard Library
#include <fstream>

// Third Party
// - Boost
#include <boost/bind.hpp>

// This Project
//

// dummy reader/event/workqueue/writer typedefs
typedef char FLEvent;
typedef std::ifstream FLRawDataReader;
typedef std::istream_iterator<FLEvent> FLRawDataReaderIterator;

typedef std::ofstream FLReconstructWriter;
typedef std::ostream_iterator<FLEvent> FLReconstructWriterIterator;

//! C++03 only
struct FLPipeline : public std::unary_function<char,char> {
  FLPipeline() : events(0) {;}

  char operator()(char c) {
    ++events;
    return (c + 5);
  }

  size_t events;
};

namespace falaise {
FLPipeline create_work_queue() {return FLPipeline);}
} // namespace falaise


class DumbReconstruction {
 public:
  DumbReconstruction() {;}

  ~DumbReconstruction() {
    this->onConfigurationChange();
  }

  //!configure reconstruction chain
  bool doConfigure(const std::string& source, 
                   const std::string& sink,
                   datatools::logger::priority p) {
    this->onConfigurationChange();
    if (source.empty() || sink.empty()) return false;

    dataReader_.open(source.c_str());
    dataWriter_.open(sink.c_str());

    if (!dataReader_.is_open() || !dataWriter_.is_open()) {
      this->onConfigurationChange();
      return false;
    }

    logLevel_ = p;
    return true;
  }

  //!run reconstruction
  int doReconstruction() {
    // Create reader iterator
    FLRawDataReaderIterator inQueue(dataReader_);
    FLRawDataReaderIterator inQueueEnd;

    // Create writer iterator
    FLReconstructWriterIterator outQueue(dataWriter_);

    // Create pipeline (single threaded, hard coded for now...)
    // Note the use of a factory function - ergo can later configure
    // and construct it at runtime.
    FLPipeline worker = falaise::create_pipeline();

    // Run work queue
    DT_LOG_INFORMATION(logLevel_, "starting work queue");
    std::transform(inQueue, inQueueEnd, 
                   outQueue,
                  // Don't worry about the boost bind, the important
                  // thing is that here's where we put the pipeline object 
                   boost::bind(boost::ref(worker),_1)
                   );
    DT_LOG_INFORMATION(logLevel_, "work queue completed");

    DT_LOG_INFORMATION(logLevel_, 
                       "processed " << worker.events << " events");
    return 0;
  }

  //!cleanup
  void onConfigurationChange() {
    dataReader_.close();
    dataWriter_.close();
  }

 private:
  FLRawDataReader dataReader_;
  FLReconstructWriter dataWriter_;
  datatools::logger::priority logLevel_;
};
#endif // DUMBRECONSTRUCTION_HH

