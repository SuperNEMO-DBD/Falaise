/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2004 
 * J.J. Gomez-Cadenas, J.A. Hernando
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef event_svc_h
#define event_svc_h

#include <string>
#include <mybhep/event.h>
#include <mybhep/event_cvt.h>
#include <mybhep/converter_svc.h>

using namespace std;


namespace mybhep{
//! event_Svc Class
/*!
This is a singleton, the genious of the lamp which can be
invoked by muttering event_svc::instance() anywhere in the program.
 - It contains a pointer to the transient event class
\ingroup event_svc
*/

  class event_svc 
  {
  public:
    
    //! constructor 
    event_svc();
    //! destructor 

    ~event_svc();
    
    //! returns the reference to itself (the singleton)
    static event_svc& instance() {return *m_instance;}	
    
    event& get_event()  
    {
      return event_;
    }

    converter_svc& converter_service()  
    {
      return *csvc_;
    }
    
    void clear_event();

  private:
    
    //! singleton, pointer to itself
    static event_svc* m_instance;
    
    /// event
    event event_;
    /// event converter
    converter_svc* csvc_;
    
  };   
}
#endif
