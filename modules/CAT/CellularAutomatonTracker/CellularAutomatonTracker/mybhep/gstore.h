/* -*- mode: c++ -*- */
/* 
 *generic store
 *
 * J.J. Gomez-Cadenas
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


#ifndef __GBSTORE__
#define __GBSTORE__

#include <mybhep/utilities.h>
#include <sstream>

#include <string>
#include <fstream>
#include <mybhep/store.h>
#include <mybhep/bprint.h>

using namespace std;

namespace mybhep{
  ///Generic store class
  /**
   * 
   * Contains a store of doubles, strings and ints
   *\ingroup base
   */

  class gstore : public bprint{
    
  protected:

    dstore dstore_;
    istore istore_;
    sstore sstore_;
    vstore vstore_;
    ivstore ivstore_;
    svstore svstore_;

  public:
    //! Default constructor
    gstore(){}
    //! default destructor
    ~gstore(){}
    
  

    //! store a new double in the store
    void store(string name,double val){dstore_.store(name,val);}
    //! store a new int in the store 
    void store(string name,int val){istore_.store(name,val);}
    //! store a new string in the store 
    void store(string name,string val){sstore_.store(name,val);}
    //! store a new histo definition in the store
    void store(string name,vdouble& val){vstore_.store(name,val);}
    //! store a new string vector in the store
    void store(string name,vstring& val){svstore_.store(name,val);}
    //! store a new integer vector in the store
    void store(string name,vint& val){ivstore_.store(name,val);}
    
     //! change a double in the store
    void change_dstore(string name,double val){dstore_.sstore(name,val);}
    //! change an int in the store
    void change_istore(string name,int val){istore_.sstore(name,val);}
    //! change a string in the store
    void change_sstore(string name,string val){sstore_.sstore(name,val);}
    //! change a histo definition in the store
    void change_vstore(string name,vdouble& val){vstore_.sstore(name,val);}
    //! change a string vector in the store
    void change_svstore(string name,vstring& val){svstore_.sstore(name,val);}
    //! change an integer vector in the store
    void change_ivstore(string name,vint& val){ivstore_.sstore(name,val);}


    //!size of the double store
    size_t size_dstore()const {return dstore_.size();}
    //!size of the int store
    size_t size_istore()const {return istore_.size();}
    //!size of the string store
    size_t size_sstore()const {return sstore_.size();}
    //!size of the histo definitions store
    size_t size_vstore()const {return vstore_.size();}
   //!size of the vector store
    size_t size_svstore()const {return svstore_.size();}
    //!size of the vector store
    size_t size_ivstore()const {return ivstore_.size();}

    //! Find double (if exists) in the store
    bool find_dstore(string name)const{return dstore_.find(name);}
    //! Find int (if exists) in the store
    bool find_istore(string name)const{return istore_.find(name);}
    //! Find string (if exists) in the store
    bool find_sstore(string name)const{return sstore_.find(name);}
    //! Find HD (if exists) in the store
    bool find_vstore(string name)const{return vstore_.find(name);}
    //! Find string vector (if exists) in the store
    bool find_svstore(string name)const{return svstore_.find(name);}
    //! Find integer vector (if exists) in the store
    bool find_ivstore(string name)const{return ivstore_.find(name);}
    
    //! Fetch a double in the store
    double fetch_dstore(string name)const{return dstore_.fetch(name);}
    //! Fetch a int in the store
    int fetch_istore(string name)const{return istore_.fetch(name);}
    //! Fetch a string in the store
    string fetch_sstore(string name)const{return sstore_.fetch(name);}
    //! Fetch a vector in the store
    const vdouble& fetch_vstore(string name)const{return vstore_.fetch(name);}
    //! Fetch a string vector in the store
    const vstring& fetch_svstore(string name)const{return svstore_.fetch(name);}
    //! Fetch a string vector in the store
    const vint& fetch_ivstore(string name)const{return ivstore_.fetch(name);}
    
    //! erase double from store 
    bool erase_dstore(string name) {return dstore_.erase(name);}
    //! erase int from store 
    bool erase_istore(string name) {return istore_.erase(name);}
    //! erase string from store 
    bool erase_sstore(string name) {return sstore_.erase(name);}
    //! erase HD from store 
    bool erase_vstore(string name) {return vstore_.erase(name);}
    //! erase string vector from store 
    bool erase_svstore(string name) {return svstore_.erase(name);}
    //! erase int vector from store 
    bool erase_ivstore(string name) {return ivstore_.erase(name);}

    //! returns all names in double store
    //vector<string> names_dstore() {return dstore_.names();}  
    //! returns all names in int store
    //vector<string> names_istore() {return istore_.names();}  
    //! returns all names in string store
    //vector<string> names_sstore() {return sstore_.names();}  
    //! returns all names in string vector store
    //vector<string> names_svstore() {return svstore_.names();}  
    //vector<string> names_vstore() {return vstore_.names();}  
    //vector<string> names_ivstore() {return ivstore_.names();}  

    //! returns all names in double store
    vector<string> names_dstore() const {return dstore_.names();}  
    //! returns all names in int store
    vector<string> names_istore() const {return istore_.names();}  
    //! returns all names in string store
    vector<string> names_sstore() const {return sstore_.names();}  
    //! returns all names in vector store
    vector<string> names_vstore() const {return vstore_.names();}  
    //! returns all names in string vector store
    vector<string> names_svstore() const {return svstore_.names();}  
    //! returns all names in int vector store
    vector<string> names_ivstore() const {return ivstore_.names();}

    //! returns all items in double store
    vector<double> items_dstore() const{return dstore_.items();}  
    //! returns all items in int store
    vector<int> items_istore() const{return istore_.items();}  
    //! returns all items in string store
    vector<string> items_sstore() const{return sstore_.items();}  
    //! returns all items in HD store
    //! returns all items in vector store
    vector<vdouble> items_vstore() const{return vstore_.items();}  
    vector<vint> items_ivstore() const{return ivstore_.items();}  
    //! returns all items in string vector store
    vector<vstring> items_svstore() const{return svstore_.items();}  
    
    //remove all elements in gstore
    void clear(){
      dstore_.clear();
      istore_.clear();
      sstore_.clear();
      vstore_.clear();
      svstore_.clear();
      ivstore_.clear();
    }
    
    //returns total size of gstore
    size_t size(){
      
      size_t len=0;
      
      len += size_istore();
      len += size_dstore();
      len += size_sstore();
      len += size_vstore();
      len += size_ivstore();
      len += size_svstore();
      
      return len;
    }
    
    bool empty(){ return (size()==0); }

   
    virtual void info(ostream& s = std::clog) const{
      
      if (level_ > MUTE){
        ostringstream ostr;

        s << endl
          << "double Data in store:\n"
          << dstore_ << "\n"
          << "int Data in store:\n"
          << istore_ << "\n"
          << "string Data in store:\n"
          << sstore_ << "\n"
          << "double vectors in store " << "\n"
          << vstore_ << "\n"
          << "integer vectors in store " << "\n"
          << ivstore_ << "\n"
          << "string vector in store " << "\n"
          << svstore_ << "\n"
          << "real matrices in store " << "\n"
          << endl;}
      else{;}
      
    }//end of info
  
  };//end of class

}//end of namespace

#endif
