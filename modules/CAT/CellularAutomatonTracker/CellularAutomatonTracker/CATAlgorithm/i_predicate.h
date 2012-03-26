/* i_predicate.h */

#ifndef __CATAlgorithm__i_predicate_h
#define __CATAlgorithm__i_predicate_h 1

namespace CAT {

template <class T>
class i_predicate : public std::unary_function<const T &, bool>
{
 public:
  
  //virtual ~i_predicate () {}
  virtual bool operator () (const T & a_obj) const = 0;
};

template<class Mother, class Daughter>
  class mother_to_daughter_predicate : public i_predicate<Daughter>
{
  i_predicate<Mother> * mother_predicate_;
 public:
  mother_to_daughter_predicate (i_predicate<Mother> & a_mother_predicate)
    {
      mother_predicate_ = &a_mother_predicate;
      return;
    }
  virtual bool operator () (const Daughter & a_obj) const
  {
    return (*mother_predicate_) (a_obj);
  }
};

}

#endif // __CAT__i_predicate_h

/* end of i_predicate.h */
