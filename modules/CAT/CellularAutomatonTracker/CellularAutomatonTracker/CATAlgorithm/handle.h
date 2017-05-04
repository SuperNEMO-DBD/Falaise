/* CAT/handle.h (from datatools/utils/handle.h) */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-04-01
 * Last modified : 2011-04-01
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *
 *  An serializable handle object referencing an object through 
 *  a shared pointer.
 *
 */

#ifndef __CAT__handle_h
#define __CAT__handle_h 1

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <CATAlgorithm/i_predicate.h>

namespace CAT {

  //! Forward declaration :
  template <class T> struct handle_predicate;

  /*! \brief Templatized handle class that host a Boost shared pointer 
   *         and behaves like a reference. 
   *
   *  A \t handle object is given the responsability to 
   *  handle a class instance through its pointer using the shared pointer
   *  mechanism from Boost. The inner hidden pointer can be null.
   *  It must be initialized with the 'new' construction operator.
   *  The handle class is copyable and can be used within STL containers.
   *
   *  \b Example:
   *
   *  @code
   *  #include <CATAlgorithm/handle.h>
   *
   *  {
   *    using namespace CAT;
   *    handle<int> h0;  // a default int is allocated
   *    h0.get () = 999; // access the int and modify its value
   *    cout << "h0 = " << h0.get () << std::endl; // print it
   *    h0.reset (new int (0)); // discard the former int 
   *                            // and allocate a new one
   *    handle<int> h1 (new int (1)); // create a new handle for another int
   *    {
   *      handle<int> h2 (h1); // another handle that shares the same int
   *                           // than the previous handle
   *      h2.get () = 666;     //change the int value through this handle
   *    }
   *    cout << "h1 = " << h1.get () << std::endl; // print it through the
   *                                          // first handle
   *  };
   *  @endcode
   *
   */     

  template <class T>
  class handle
  {
  
    boost::shared_ptr<T> sp_; /*!< The embedded shared pointer. */ 
  
  public:
  
    typedef T value_type;
    typedef value_type * reference_type;
    typedef const reference_type const_reference_type;
    typedef value_type * pointer_type;
    typedef const pointer_type const_pointer_type;
  
    //! The default constructor.
    /*!
     * The default constructor automatically allocate an object 
     * handled by the internal shared pointer.
     */
    //handle () : sp_ (new T)
    /*
      handle () : sp_ (0)
      {
      return;
      }
    */
  
    //! A constructor from a pointer to some on-the-fly allocated instance.
    /*!
     * This constructor is given a pointer to some dynamically 
     * allocated instance and pass it to the internal shared pointer.
     *
     * Example:
     * @code
     * datatools::utils::handle<int> h (new int (3));                             
     * @endcode
     * or
     * @code
     * int * pi = new int (3);
     * datatools::utils::handle<int> h (pi);                              
     * @endcode
     *
     * \warning Never use such kind of mechanism to initialize a handle:
     * @code
     * int i = 3;
     * datatools::utils::handle<int> h (&i);                              
     * @endcode
     *
     */
    handle (T * a_t_ptr = 0) : sp_ (a_t_ptr)
    {
      return;
    }
  
    //! A constructor from a shared pointer of the same type.
    /*!
     * This constructor is given a shared pointer that hosts (or not)
     * a dynamically allocated instance. This internal pointer will then
     * been shared by both shared pointers.
     */
    handle (const boost::shared_ptr<T> & a_sp) : sp_ (a_sp)
    {
      return;
    }
  
    virtual ~handle ()
    {
      /*
        std::clog << "DEVEL: datatools::utils::handle::dtor: Entering..." << std::endl;
        std::clog << "DEVEL: datatools::utils::handle::dtor:   Use count = " 
        << sp_.use_count () << std::endl;
      */
      sp_.reset ();
      //clog << "DEVEL: datatools::utils::handle::dtor: Exiting." << std::endl;
      return;
    }
  
    bool unique() const
    {
      return sp_.unique ();
    }
  
    size_t count () const
    {
      if (sp_.get () == 0) return 0;
      return sp_.use_count ();
    }
  
    //! Check is no allocated instance is handled by the internal shared pointer.
    bool is_null () const
    {
      return sp_.get () == 0;
    }
  
    //! Check is some dynamically allocated instance is handled by the internal shared pointer.
    bool has_data () const
    {
      return ! is_null ();
    }
  
    void swap (handle<T> & other) // never throws
    {
      sp_.swap (other.sp_);
      return;
    }
  
    void swap (boost::shared_ptr<T> & other) // never throws
    {
      sp_.swap (other);
      return;
    }
  
    //! Get a const reference to the hosted dynamically allocated instance.
    const T & get () const
    {
      if (sp_.get () == 0)
        {
          throw std::logic_error ("CAT::handle::get: Handle has no data !");
        }
      return *sp_.get ();
    }
  
    //! Get a non-const reference to the hosted dynamically allocated instance.
    T & get ()
    {
      if (sp_.get () == 0)
        {
          throw std::logic_error ("CAT::handle::get: Handle has no data !");
        }
      return *sp_.get ();
    }
  
    /*
    //! Reset the internal shared pointer with some new dynamically allocated instance created using its default constructor.
    void reset ()
    {
    sp_.reset (0);
    //      sp_.reset (new T);
    return;
    }
    */
  
    //! Reset the internal shared pointer with some new dynamically allocated instance.
    void reset (T * a_t_ptr = 0)
    {
      sp_.reset (a_t_ptr);
      return;
    }
  
    //! Reset the internal shared pointer from another shared pointer.
    void reset (const boost::shared_ptr<T> & a_sp)
    {
      sp_ = a_sp;
      return;
    }
  
  // private:
  //  
  //   friend class boost::serialization::access; 
  //   template <class Archive>
  //   void serialize (Archive & ar, int version)
  //   {
  //     ar & boost::serialization::make_nvp ("sp", sp_);
  //     return;
  //   }
  
  public:
  
    typedef handle_predicate<T> predicate_type;
      
  };

  /*! \brief Templatized predicate class associated to handle instance.
   *
   * The \t handle_predicate class is used to wrap a predicate object to some
   * instance and pass it to some algorithms (typically the std::find_if function)t.
   *
   * \b Example:
   * @code
   * #include <iostream>
   * #include <vector>
   * #include <algorithm>
   *
   * #include <datatools/utils/i_predicate.h>
   * #include <datatools/utils/handle.h>
   *
   * struct is_pi : public i_predicate <double>
   * {
   *   bool operator () (const double & a_number) const
   *   {
   *     return a_number = 3.1415926;
   *   }
   * };
   *
   * int main (void)
   * {
   *   is_pi DP;
   *   typedef handle<double> handle_type;
   *   handle_predicate<double> HP (DP);
   *   std::vector<handle_type> values;
   *   values.push_back (handle_type (new double (1.0)));
   *   values.push_back (handle_type (new double (3.1415926)));
   *   values.push_back (handle_type (new double (10.0)));
   *   std::vector<handle_type>::const_iterator it = 
   *     std::find_if (values.begin (), values.end (), HP);
   *   if (it != values.end ()) std::cout << 3Found PI !" << std::endl;
   *   return 0;
   * }
   * @encode
   *
   */
  template <class T>
  struct handle_predicate : public i_predicate <handle<T> >
  {
    //! The default constructor.
    handle_predicate (const i_predicate<T> & a_predicate, 
                      bool a_no_data_means_false = true) 
      : predicate_ (a_predicate), 
        no_data_means_false_ (a_no_data_means_false)
    {
      return;
    }
      
    //! The predicate method.
    bool operator () (const handle<T> & a_handle) const
    {
      if (! a_handle.has_data()) 
        {
          if (no_data_means_false_) return false;
          throw std::logic_error ("CAT::handle_predicate::operator (): Handle has no data !");
        }
      return (predicate_ (a_handle.get ()));
    }

    const i_predicate<T> & predicate_;           /*!< The embedded predicate. */ 
    bool                   no_data_means_false_; /*!< A flag to indicate the behaviour in case of NULL pointer : predicate returns false (default) or throws an exception). */ 
          
  };

}
     
// From "boost/serialization/shared_ptr.hpp" :
// #ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// namespace boost {
//   namespace serialization{
//     // don't track shared pointers
//     template<class T>
//     struct tracking_level< ::CAT::handle<T> > { 
//       typedef mpl::integral_c_tag tag;
// #if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
//       typedef BOOST_DEDUCED_TYPENAME mpl::int_< ::boost::serialization::track_never> type;
// #else
//       typedef mpl::int_< ::boost::serialization::track_never> type;
// #endif
// #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x570))
//       BOOST_STATIC_CONSTANT(int, value = ::boost::serialization::track_never);
// #else
//       BOOST_STATIC_CONSTANT(int, value = type::value);
// #endif
//     };
//   }}
// #define BOOST_SERIALIZATION_DATATOOLS_UTILS_HANDLE(T)
// #else
// // define macro to let users of these compilers do this
// #define BOOST_SERIALIZATION_DATATOOLS_UTILS_HANDLE(T)                 \
//   BOOST_SERIALIZATION_SHARED_PTR(T)                                   \
//   BOOST_CLASS_TRACKING(                                               \
//                        ::CAT::handle< T >,                            \
//                        ::boost::serialization::track_never            \
//                                                                     ) \
//   /**/
// #endif

#endif // __CAT__handle_h 

/* end of datatools/utils/handle.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
