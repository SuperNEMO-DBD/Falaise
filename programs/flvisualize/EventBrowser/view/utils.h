// -*- mode: c++ ; -*-
/* utils.h
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_UTILS_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_UTILS_H 1

// Standard library
#include <vector>
#include <list>
#include <memory>

#include <TObject.h>

#include <EventBrowser/log.h>

namespace snemo {

  namespace visualization {

    namespace view {

      typedef std::unique_ptr<TObject> object_ptr_type;

      class ObjectsArray : public std::list<object_ptr_type>
      {
      public:
                                
        ObjectsArray(std::size_t /*capacity_*/ = 1000)
        {
        }
                                
        void Add(TObject* obj_)
        {
          this->emplace_back(obj_);
        }

        void Delete()
        {
	  FL_LOG_DEVEL("Deleting " << this->size() << " objects...");
          this->clear();
        }
        
        void SetOwner(bool)
        {
          return;
        }

        size_t GetEntries() const
        {
          return this->size();
        }
 
        size_t GetEntriesFast() const
        {
          return this->size();
        }
                                
        void Draw(Option_t *option_ = nullptr)
        {
          for (const auto & objPtr : *this) {
            objPtr->Draw(option_);
          }
        }
                              
        void Remove(TObject* obj_)
        {
	  FL_LOG_DEVEL("before size = " << this->size());
	  this->remove_if([&](const object_ptr_type & objptr_) { return objptr_.get() == obj_; });
	  // for (std::list<object_ptr_type>::iterator it = this->begin(); it != this->end();)
	  //   {
	  //     if (it->get() == obj_) {
	  // 	it = this->erase(it);
	  // 	break;
	  //     } else {
	  // 	++it;
	  //     }
	  //   }
	  FL_LOG_DEVEL("after size = " << this->size());
	}
 
      };
      
      typedef ObjectsArray TObjArray; // Mock ROOT class

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_UTILS_H
