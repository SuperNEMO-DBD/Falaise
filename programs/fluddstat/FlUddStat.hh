#ifndef FLUDDSTAT_HH
#define FLUDDSTAT_HH

// Standard Library
#include <string>

namespace FLUddStat {

  struct FLUddStatConfig
  {
    std::string inputUddFilePath;
    std::string outputStatFilePath;
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
