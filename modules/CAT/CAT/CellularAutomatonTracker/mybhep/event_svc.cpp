
#include <mybhep/event_svc.h>
#include <mybhep/error.h>
#include <mybhep/container_algorithm.h>
#include <iostream>
#include <fstream>

namespace mybhep {

event_svc* event_svc::m_instance = 0;

event_svc::event_svc() {
  Assert(m_instance == 0, __FILE__, __LINE__, internal_logic("event_svc already created"));

  m_instance = this;
  csvc_ = new converter_svc;
}

event_svc::~event_svc() { delete csvc_; }

void event_svc::clear_event() {
  event_.clear();
  csvc_->clear();
}
}  // namespace mybhep
