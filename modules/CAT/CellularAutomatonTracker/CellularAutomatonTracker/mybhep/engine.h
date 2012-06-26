/* -*- mode: c++ -*- */
/// Contains the definition of a  base class for an engine
/* 
 * J.J. Gomez-Cadenas, J.A. Hernando, A. Cervera, A. Tornero      
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
 */
#ifndef _engine___
#define _engine___

#include <iostream>
#include <mybhep/control_panel.h>
#include <mybhep/messenger.h>
#include <mybhep/bprint.h>


namespace mybhep{
/// Base class for an engine
/** 
 * An engine is an object able to define its internal state
 * (via a control panel, that is a set of switches that can be
 * switched on or off) and to communicate it to the external world
 * via a messenger. It also includes the printing interface 
  *\ingroup base
 */
    class engine : public bprint {
    public:
/// A control panel used to describe the state of the engine
      control_panel panel_;
/// A messenger to comunicate with the rest of the world      
      messenger m_;     

	///Do not instantiate an engine directly
      engine(){}    

    public:
	///Default destructor
	~engine(){}    
	/// Sets the output level for the engine
	void info_level(prlevel level) {m_.set_level(level);} 
	/// returns the control panel for reading
	const control_panel& panel() const {return panel_;}  
	/// returns the messenger
	const messenger& messenger_service() const {return m_;}  

      /// set the verbosity level 
      void set_info_level(string info){
	  if(info == "MUTE")  info_level(MUTE);
	  if(info == "CONCISE")  info_level(CONCISE);
	  if(info == "NORMAL")  info_level(NORMAL);
	  if(info == "WARNING")  info_level(WARNING);
	  if(info == "DETAILED")  info_level(DETAILED);
	  if(info == "VERBOSE")  info_level(VERBOSE);
	  if(info == "VVERBOSE")  info_level(VVERBOSE);
	  if(info == "DUMP")  info_level(DUMP);
      }

	/// parameters set by default if all switches closed
	virtual bool param_set(){return panel_.test_closed();}

    };
}

#endif   /* _ENGINE___ */
