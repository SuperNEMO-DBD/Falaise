#ifndef FLUDDSTAT_HH
#define FLUDDSTAT_HH

// Standard Library
#include <string>
#include <cstdint>

#include <bayeux/datatools/logger.h>

namespace FLUddStat {

  struct FLUddStatConfig
  {
    datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
    std::uint32_t maxNbEvents = 0;
    std::string inputUddFilePath;
    std::string outputDirPath;
  };
 
  struct FLUddStatApp
  {
  public:
    FLUddStatApp(const FLUddStatConfig & config_);
    int run();
  private:
    FLUddStatConfig _config_;
  };
  
} // FLUddStat

#endif // FLUDDSTAT_HH
