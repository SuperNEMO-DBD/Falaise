/* -*- mode: c++ -*- */
/* 
 *Simple reader class
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


#ifndef __SREADER__
#define __SREADER__
#include <sstream>

#include <fstream>
#include <mybhep/gstore.h>
#include <mybhep/engine.h>
#include <mybhep/utilities.h>

using namespace std;
namespace mybhep{
    /// Simple reader class
  /**
   * Reads a data file with format group-name,param-name,
   * param-type param-value, where group-name and param-name are 
   * strings with the name of the group (for example the instance 
   * to which this parameter belongs)  and the name of the parameter, 
   * type is a string with value
   * S if the parameter is a string or D if the parameter is a double
   * and I if the param is a parameter and param-value is a string, double
   * or integer
   *
   * Any line starting by "#" is considered as a comment. 
   \ingroup base
   */
  
  class sreader : public engine{
  
  public:
    /// Default constructor
    sreader(gstore& store){
      store_ = &store;
      panel_.new_switch("file");
      panel_.new_switch("group");
    }
  
    /// sets the name of the file
    void file(string fileName)
    {
      m_fileName = fileName;

      if (panel_.switch_open("file")){    
        panel_.close_switch("file");
      }else{
        m_dataFile.close();
      }
   
      m_dataFile.open(fileName.c_str(), ios::in);
      
      Assert(!m_dataFile.fail(),__FILE__,__LINE__,
             internal_logic("Failed to open file "+m_fileName)); 

      m_.message("sreader initialized with file ",m_fileName,VVERBOSE);

    }
    
    /// returns the name of the file
    string file(){
      Assert(panel_.switch_closed("file"),__FILE__,__LINE__,
             internal_logic("--sreader file not set to reader"));       
      
      return m_fileName;
    }
  
    /// sets the name of the group 
    void group(string groupName){
      
      if (panel_.switch_closed("group")){    
        panel_.open_switch("group");
        file(m_fileName);
      }

      m_groupName = groupName;
      panel_.close_switch("group");

      m_.message("sreader initialized with group ",m_groupName,VVERBOSE);

    }

    /// returns the name of the group 
    string group(){
      Assert(panel_.switch_closed("group"),__FILE__,__LINE__,
             internal_logic("--sreader group not set to reader"));      
      
      return m_groupName;
    }
  
    /// Destructor closes file
    ~sreader()
    {
      if (panel_.switch_closed("file"))    
        m_dataFile.close();
    }
  
    /// Read the file 
    void read()
    {
      Assert(panel_.switch_closed("file"),__FILE__,__LINE__,
             internal_logic("--sreader file not set to reader"));       
    
      Assert(panel_.switch_closed("group"),__FILE__,__LINE__,
             internal_logic("--sreader group not set to reader"));      
      
      while (!m_dataFile.eof()) {
        
        double value;
        int ivalue;
        string svalue;
        
        string label,test,dummy;
        string group = nextEntry();
      
      m_.message("sreader in read(); ",group,VVERBOSE);

        if (compare_string(group,m_groupName)== false){
          getline(m_dataFile, dummy);
          
          continue;
        }
        m_dataFile >> label;
        m_dataFile >> test;

      m_.message(" test = ",test,"label = ", label,VVERBOSE);
      
      try{
        Assert(test =="S" || test =="D" || test =="I" 
               || test =="V"
               || test =="DV"
               || test =="IV"
               || test =="SV", 
               __FILE__,__LINE__,
               internal_logic("must be S, D, I, V, DV,IV, SV"));
        
        // V=VD, but still here to keep backward compatibility
        
        if (compare_string_nocase(test,"S")){
          m_dataFile >> svalue;
          store_->store(label,svalue);

          m_.message(" data = ",svalue,VVERBOSE);
          
        }
        else if (compare_string_nocase(test,"D")){
          m_dataFile >> value;
          store_->store(label,value);
        }
        else if (compare_string_nocase(test,"I")){
          m_dataFile >> ivalue;
          store_->store(label,ivalue);
        }
        else if (compare_string_nocase(test,"V")||
                 compare_string_nocase(test,"DV")){
          size_t size;
          //double value;
          string value;
          dvector<double> dv;
          m_dataFile >> size;      
          //        clog <<" " << size <<endl;
            
          for(size_t i=0; i<size; i++)
            {
              m_dataFile >> value;
              dv.push_back(mybhep::double_from_string(value));
            }
            
          store_->store(label,dv);
          
        }
        else if (compare_string_nocase(test,"IV")){
          size_t size;
          //int value;
          string value;
          dvector<int> dv;
          m_dataFile >> size;      
          //        clog <<" " << size <<endl;
            
          for(size_t i=0; i<size; i++)
            {
              m_dataFile >> value;
              dv.push_back(mybhep::int_from_string(value));
            }
            
          store_->store(label,dv);
        }
        else if (compare_string_nocase(test,"SV")){
          size_t size;
          string value;
          dvector<string> dv;
          m_dataFile >> size;      
          //        clog <<" " << size <<endl;
            
          for(size_t i=0; i<size; i++)
            {
              m_dataFile >> value;
              dv.push_back(value);
            }
            
          store_->store(label,dv);
        }
      }
      catch(internal_logic& err){
        cerr << "test must be empty test = " << test << endl;
        cerr << " this means end of file" << endl;
        cerr << " otherwise value of parameter is ignored" << endl;
      }
      }
    }

  private:
    gstore* store_;
    ifstream m_dataFile;
    string m_fileName;
    string m_groupName;
  
    /**
     * Advances in input file removing all comments, and returns the
     * next entry.
     */
    string nextEntry() 
    {
      string word, dummy;
    
      // if the word begins by "#" (a comment)...
      // discard the rest of the line      
      do {
        m_dataFile >> word;
        if (word[0] == '#')         
          getline(m_dataFile, dummy);   
      } while (word[0] == '#' && !m_dataFile.eof());
      
      return word;
    }
  };
}
#endif
