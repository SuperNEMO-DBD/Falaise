/* -*- mode: c++ -*- */
/*
 *
 * Copyright 2003 
 * J.J. Gomez-Cadenas, J.A. Hernando,
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

#ifndef _tstore___
#define _tstore___

#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <mybhep/utilities.h>
#include <mybhep/error.h>
#include <mybhep/bprint.h>

using namespace std;  

namespace mybhep{

//!Template server
/**
 * Asociative container, capable of storing and retrieveing 
 * generic objects by name
\ingroup base 
 */
    
  template <class T>
  class tstore : public bprint{
  protected:  
    //! attributes: a map of objects of type T keyed by name    
    map<string, T> m_store;   
    
  public:
    //! Nothing to do in constructor 
    tstore(){ };
    //! Nothing to do in destructor
     ~tstore(){};
  
    const map<string,T>& store_map() const
    {
      return m_store;
    }

    void clear(){
      m_store.clear();
    }

  
    //! store an object
    void store(string name,T& object)
    {
      bool status = find (name);
      Assert(status == false,__FILE__,__LINE__,
             internal_logic("name already in store, name ="+ name));
      
        m_store[name] = object;
    }

    //! store an object
    void store(string name, const T& object)
    {
      bool status = find (name);
      Assert(status == false,__FILE__,__LINE__,
             internal_logic("name already in store, name = "+ name));
      
        m_store[name] = object;
    }
  
    //! strong store an object (rewrite name if necessary)
    void sstore(string name,T& object)
    {
      
        m_store[name] = object;
    }

    //! strong-store an object (rewrite name if necessary)
    void sstore(string name, const T& object)
    {      
        m_store[name] = object;
    }
  
    //!size of the store
     size_t size()const {return m_store.size();}
    
    //! find an element in the store by name
     bool find (string name) const
    {
      bool gotcha = false;
      typename map<string, T>::const_iterator pi;
      
      for ( pi = m_store.begin(); 
            pi!= m_store.end(); ++pi)
        {
          if ( (pi->first) == name) {
              gotcha = true;
              break;
          }
        } 

      return gotcha;
    
    }
    //! erase an object in store
    /**
       returns true if the object is in the store, false otherwise
    */
     bool erase (string name)
    {
      int count = m_store.erase (name);
      if (count == 1) return true;
      else if (count == 0) return false;
      else 
        {
          cerr << " warning, name = " << name 
               <<"  found " << count << " times in store"
               << endl;
          return true;
        }
    
    }
    //! operator []
    T& operator[](string name)
    {return fetch(name);} 

    //! operator []
    const T& operator[](string name) const 
    {return fetch(name);}


    //! fetch an object from its name
    const T& fetch (string name) const
    {
        Assert(find(name), __FILE__,__LINE__,
               internal_logic("name not found in --tstore::fetch(), name ="
                              + name));
     
      typename map<string, T>::const_iterator it;
      for (it = m_store.begin(); it!= m_store.end(); ++it)
      {
        if(it->first == name) break;
      }
      return it->second;
    }
    
    T& fetch (string name) 
    { 
        Assert(find(name), __FILE__,__LINE__,
               internal_logic("name not found in --tstore::fetch(), name ="
                              + name));
      
        return m_store[name];    
    }


    //! returns all names in store
    vector<string> names () const  
    {
      typename map<string, T>::const_iterator pi;
      vector<string> nam;
      
      for (pi = m_store.begin(); pi!= m_store.end(); ++pi)
        {
          nam.push_back(pi->first);
        } 
      return nam;
    }
    
    //! returns all names in store (constant)
    // const vector<string> names ()  const
//     {
//       typename map<string, T>::const_iterator pi;
//       vector<string> nam;
      
//       for (pi = m_store.begin(); pi!= m_store.end(); ++pi)
//      {
//        nam.push_back(pi->first);
//      } 
//       return nam;

//     }

    //! returns the dvector of items
    vector<T> items() const{
      vector<T> vitems;
      typename map<string, T>::const_iterator it;
      for (it = m_store.begin(); it!= m_store.end(); ++it)
      {
          vitems.push_back(it->second);
      }
      return vitems;
    }
    //! print interface
    virtual void info(ostream& s = clog) const{
      if (level_ > MUTE){
        ostringstream ostr;
        
        typename map<string, T>::const_iterator pi;
        
        for (pi = (m_store).begin(); pi!= (m_store).end(); ++pi)
          {
            ostr << pi->first<< " = " << pi->second << endl;  
          } 
        
        s << endl
          << ostr.str() << "\n"
          << endl;
      }
      else{
        ;}
    }

  };
}
#endif  

