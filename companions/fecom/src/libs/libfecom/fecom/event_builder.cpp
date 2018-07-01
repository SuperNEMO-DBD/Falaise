// fecom/tracker_hit.cpp

// Ourselves:
#include <fecom/event_builder.hpp>

namespace fecom {

  event_builder::event_builder(double l1_time_gate_ns_,
			       double l2_time_gate_ns_)
  {
    l1_time_gate = l1_time_gate_ns_;
    if (l1_time_gate < 0.0) {
      l1_time_gate = EVENT_BUILDING_CALO_L1_GATE_IN_NS;
    }
    l2_time_gate = l2_time_gate_ns_;
    if (l2_time_gate < 0.0) {
      l2_time_gate = EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS;
    }

    datatools::invalidate(time_start);
    datatools::invalidate(calo_time_stop);
    datatools::invalidate(l2_time_stop);
    return;
  }

  event_builder::~event_builder() {}

  void event_builder::set_times(const double time_start_)
  {
    time_start = time_start_;
    calo_time_stop = time_start + l1_time_gate;
    l2_time_stop = time_start + l2_time_gate;
    //std::clog << "Event builder set times method" << std::endl;
    //std::clog << "L1 duration = " << l1_time_gate << " L2 duration = " << l2_time_gate << std::endl;
    //std::clog << "Tstart = " << time_start << " Tstop = " << l2_time_stop << " Calo time stop = " << calo_time_stop << std::endl;
    working_commissioning_event.set_time_start_ns(time_start);
    return;
  }

  void event_builder::build_tracker_hits_from_channels(const fecom::channel_mapping & channel_mapping_)
  {
    if (commissioning_event_for_serialization.get_tracker_channel_hit_collection().size() != 0)
      {
	commissioning_event_for_serialization.build_tracker_hit_from_channels(channel_mapping_);
      }
    return;
  }

  void event_builder::reset_serialization()
  {
    commissioning_event_for_serialization.reset();
    ready_for_serialization = false;
    return;
  }


} // namespace fecom
