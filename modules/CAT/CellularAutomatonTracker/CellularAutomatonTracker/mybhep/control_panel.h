/* -*- mode: c++ -*- */
///  Definition of class control panel

/* Control panel
 *
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
 */

#ifndef _CCONTRL_
#define _CCONTRL_
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <mybhep/error.h>


namespace mybhep{
/// Represents a control panel.
/** A control panel is a set of switches whose configuration 
 * defines the state of the machine controlled by the panel. A switch has only
 * two possible states, open or closed 
  *\ingroup base
 */
    
  class control_panel {

  private:
    /// map of bools keyed by name
    std::map<std::string, bool> cp_;   
  public:
    ///nothing to do in constructor and destructor
    control_panel(){};
    ~control_panel(){};
    
    /// Add a switch to the board (initially open)
    void new_switch(std::string name){

      cp_[name] = false;

    }

    /// Close a switch
    void close_switch(std::string name){
      Assert(find_switch(name),__FILE__,__LINE__,
	     internal_logic("--switch not found in control board--"));
      cp_[name] = true;
    }

    /// Open a switch
    void open_switch(std::string name){
      // open the switch if in the controal board
      // abort otherwise

      Assert(find_switch(name),__FILE__,__LINE__,
	     internal_logic("--switch not found in control board--"));
      cp_[name] = false;
    }

    /// Change the state of a switch
    void change_switch(std::string name){

      Assert(find_switch(name),__FILE__,__LINE__,
	     internal_logic("--switch not found in control board--"));
      bool state = cp_[name];
      if (state == false) 
	cp_[name] = true;
      else
	cp_[name] = false;      
    }

    /// Change the state of every switch in the board
    void change_switches(){
      typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      for (map_const_iterator p = cp_.begin(); p!= cp_.end(); ++p){
	change_switch(p->first);
      } 
    }

    /// Close every switch in the board
    void close_switches(){
      typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      for (map_const_iterator p = cp_.begin(); p!= cp_.end(); ++p){
	close_switch(p->first);
      } 
    }
    
    /// Open every switch in the board
    void open_switches(){
      typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      for (map_const_iterator p = cp_.begin(); p!= cp_.end(); ++p){
	open_switch(p->first);
      } 
    }

    /// Find a switch (if exists) in the board
    bool find_switch(std::string name) const {
      bool gotcha = false;
      typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      for (map_const_iterator p = cp_.begin(); p!= cp_.end(); ++p){
	if ( (p->first) == name) gotcha = true;
      } 
      return gotcha;
    }


    /// Delete a switch from the board
    /**Return a int (0 if not found in the board or the
     * number of times the switch was found in the board)
     */
    int delete_switch(std::string name){
      int times = cp_.erase (name);
      return times;
    }

    /// Return true if the switch is closed
    bool switch_closed(std::string name) const {
      Assert(find_switch(name),__FILE__,__LINE__,
	     internal_logic("--switch not found in control board--"));
      return cp_.find(name)->second;
    }


    /// Return true if the switch is open
    bool switch_open(std::string name) const {
      Assert(find_switch(name),__FILE__,__LINE__,
	     internal_logic("--switch not found in control board--"));
      return !cp_.find(name)->second;

    }


    /// Test a state via a std::vector of (names) of switches
    /** returns true if all closed, false if one or more open */

    bool test_state( std::vector<std::string> state) const {
	bool test = true;
	for (size_t i = 0; i < state.size(); i++){
	    test = test && switch_closed(state[i]);
	}
	return test;
    }
    /// Test if all switches are closed
    /** returns true if all closed, false if one or more open */

    bool test_closed() const {
	bool test = true;
	typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      
	for (map_const_iterator p = cp_.begin(); p!= cp_.end(); ++p){
	    test = test && p->second;
	}

	return test;
    }

    friend std::ostream& operator << (std::ostream& s, const control_panel& panel){
      typedef  std::map<std::string, bool>::const_iterator map_const_iterator;
      
      for (map_const_iterator p = panel.cp_.begin(); 
	   p!= panel.cp_.end(); ++p){
	s << " switch name = " << p->first
	  << " switch state = " <<p->second << std::endl;
      } 	  
      return s;
    }
  };
}
#endif  
