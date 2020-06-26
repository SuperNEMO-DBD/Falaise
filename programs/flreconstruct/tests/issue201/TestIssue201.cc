#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/datamodels/particle_track_data.h"
#include "falaise/snemo/processing/module.h"
namespace flp = falaise::processing;

// A trivial wrapped module
class TestIssue201 {
 public:
  TestIssue201() = default;
  TestIssue201(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/)
      : TestIssue201() {}

  flp::status process(datatools::things& e) {
    // Need event header and PTD to check
    const auto& eh = e.get<snemo::datamodel::event_header>("EH");
    const auto& ptd = e.get<snemo::datamodel::particle_track_data>("PTD");

    // Use event id and particle count to compare
    int id = eh.get_id().get_event_number();
    size_t expectedParticles = ptdCounts_.at(id);
    size_t actualParticles = ptd.numberOfParticles();

    // Assert that we have the expected number of particles
    std::cout << id << ", " << expectedParticles << ", " << actualParticles << std::endl;
    DT_THROW_IF(expectedParticles != actualParticles, std::logic_error,
                "Event " << id << " has " << actualParticles << " particles, expected " << expectedParticles);

    return flp::status::PROCESS_OK;
  }

 private:
  std::vector<size_t> ptdCounts_ = {1, 5, 1, 4, 3, 2, 1, 1, 1, 0};
};
FALAISE_REGISTER_MODULE(TestIssue201)