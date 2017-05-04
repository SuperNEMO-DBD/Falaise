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
    void store(std::string name,double val){dstore_.store(name,val);}
    //! store a new int in the store
    void store(std::string name,int val){istore_.store(name,val);}
    //! store a new std::string in the store
    void store(std::string name,std::string val){sstore_.store(name,val);}
    //! store a new histo definition in the store
    void store(std::string name,vdouble& val){vstore_.store(name,val);}
    //! store a new std::string std::vector in the store
    void store(std::string name,vstring& val){svstore_.store(name,val);}
    //! store a new integer std::vector in the store
    void store(std::string name,vint& val){ivstore_.store(name,val);}

     //! change a double in the store
    void change_dstore(std::string name,double val){dstore_.sstore(name,val);}
    //! change an int in the store
    void change_istore(std::string name,int val){istore_.sstore(name,val);}
    //! change a std::string in the store
    void change_sstore(std::string name,std::string val){sstore_.sstore(name,val);}
    //! change a histo definition in the store
    void change_vstore(std::string name,vdouble& val){vstore_.sstore(name,val);}
    //! change a std::string std::vector in the store
    void change_svstore(std::string name,vstring& val){svstore_.sstore(name,val);}
    //! change an integer std::vector in the store
    void change_ivstore(std::string name,vint& val){ivstore_.sstore(name,val);}


    //!size of the double store
    size_t size_dstore()const {return dstore_.size();}
    //!size of the int store
    size_t size_istore()const {return istore_.size();}
    //!size of the std::string store
    size_t size_sstore()const {return sstore_.size();}
    //!size of the histo definitions store
    size_t size_vstore()const {return vstore_.size();}
   //!size of the std::vector store
    size_t size_svstore()const {return svstore_.size();}
    //!size of the std::vector store
    size_t size_ivstore()const {return ivstore_.size();}

    //! Find double (if exists) in the store
    bool find_dstore(std::string name)const{return dstore_.find(name);}
    //! Find int (if exists) in the store
    bool find_istore(std::string name)const{return istore_.find(name);}
    //! Find std::string (if exists) in the store
    bool find_sstore(std::string name)const{return sstore_.find(name);}
    //! Find HD (if exists) in the store
    bool find_vstore(std::string name)const{return vstore_.find(name);}
    //! Find std::string std::vector (if exists) in the store
    bool find_svstore(std::string name)const{return svstore_.find(name);}
    //! Find integer std::vector (if exists) in the store
    bool find_ivstore(std::string name)const{return ivstore_.find(name);}

    //! Fetch a double in the store
    double fetch_dstore(std::string name)const{return dstore_.fetch(name);}
    //! Fetch a int in the store
    int fetch_istore(std::string name)const{return istore_.fetch(name);}
    //! Fetch a std::string in the store
    std::string fetch_sstore(std::string name)const{return sstore_.fetch(name);}
    //! Fetch a std::vector in the store
    const vdouble& fetch_vstore(std::string name)const{return vstore_.fetch(name);}
    //! Fetch a std::string std::vector in the store
    const vstring& fetch_svstore(std::string name)const{return svstore_.fetch(name);}
    //! Fetch a std::string std::vector in the store
    const vint& fetch_ivstore(std::string name)const{return ivstore_.fetch(name);}

    //! erase double from store
    bool erase_dstore(std::string name) {return dstore_.erase(name);}
    //! erase int from store
    bool erase_istore(std::string name) {return istore_.erase(name);}
    //! erase std::string from store
    bool erase_sstore(std::string name) {return sstore_.erase(name);}
    //! erase HD from store
    bool erase_vstore(std::string name) {return vstore_.erase(name);}
    //! erase std::string std::vector from store
    bool erase_svstore(std::string name) {return svstore_.erase(name);}
    //! erase int std::vector from store
    bool erase_ivstore(std::string name) {return ivstore_.erase(name);}

    //! returns all names in double store
    //vector<string> names_dstore() {return dstore_.names();}
    //! returns all names in int store
    //vector<string> names_istore() {return istore_.names();}
    //! returns all names in std::string store
    //vector<string> names_sstore() {return sstore_.names();}
    //! returns all names in std::string std::vector store
    //vector<string> names_svstore() {return svstore_.names();}
    //vector<string> names_vstore() {return vstore_.names();}
    //vector<string> names_ivstore() {return ivstore_.names();}

    //! returns all names in double store
    std::vector<std::string> names_dstore() const {return dstore_.names();}
    //! returns all names in int store
    std::vector<std::string> names_istore() const {return istore_.names();}
    //! returns all names in std::string store
    std::vector<std::string> names_sstore() const {return sstore_.names();}
    //! returns all names in std::vector store
    std::vector<std::string> names_vstore() const {return vstore_.names();}
    //! returns all names in std::string std::vector store
    std::vector<std::string> names_svstore() const {return svstore_.names();}
    //! returns all names in int std::vector store
    std::vector<std::string> names_ivstore() const {return ivstore_.names();}

    //! returns all items in double store
    std::vector<double> items_dstore() const{return dstore_.items();}
    //! returns all items in int store
    std::vector<int> items_istore() const{return istore_.items();}
    //! returns all items in std::string store
    std::vector<std::string> items_sstore() const{return sstore_.items();}
    //! returns all items in HD store
    //! returns all items in std::vector store
    std::vector<vdouble> items_vstore() const{return vstore_.items();}
    std::vector<vint> items_ivstore() const{return ivstore_.items();}
    //! returns all items in std::string std::vector store
    std::vector<vstring> items_svstore() const{return svstore_.items();}

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


    virtual void info(std::ostream& out = std::clog) const{

      if (level_ > MUTE){
        std::ostringstream ostr;

        out << std::endl
            << "double Data in store:\n"
            << dstore_ << "\n"
            << "int Data in store:\n"
            << istore_ << "\n"
            << "std::string Data in store:\n"
            << sstore_ << "\n"
            << "double std::vectors in store " << "\n"
            << vstore_ << "\n"
            << "integer std::vectors in store " << "\n"
            << ivstore_ << "\n"
            << "std::string std::vector in store " << "\n"
            << svstore_ << "\n"
            << "real matrices in store " << "\n"
            << std::endl;
      }

    }//end of info

  };//end of class

}//end of namespace

#endif
