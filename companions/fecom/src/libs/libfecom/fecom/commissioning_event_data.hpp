//! \file fecom/commissioning_event_data.hpp
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_COMMISSIONING_EVENT_DATA_HPP
#define FECOM_COMMISSIONING_EVENT_DATA_HPP

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/commissioning_event.hpp>

namespace fecom {

  struct commissioning_event_data
    : public datatools::i_serializable
  {
    typedef std::vector<fecom::commissioning_event> commissioning_event_collection;

    commissioning_event_data();

    virtual ~commissioning_event_data();

    bool is_valid() const;

    void add_commissioning_event(commissioning_event & a_commissioning_event_);

    const commissioning_event_data::commissioning_event_collection & get_commissioning_event_collection() const;

    commissioning_event_data::commissioning_event_collection & grab_commissioning_event_collection();

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private:

    void _reset_();

    commissioning_event_collection _commissioning_event_collection_; ///< Commissioning event collection

    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // namespace fecom

#endif // FECOM_COMMISSIONING_EVENT_DATA_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
