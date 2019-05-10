#ifndef CUTEE_ASSERTER_HPP_INCLUDED
#define CUTEE_ASSERTER_HPP_INCLUDED

#include <tuple>
#include <functional>
#include <cassert>

#include "test_suite.hpp"

namespace cutee
{

/**
 * Struct for carrying out assertions. Uses a static pointer to a suite.
 **/
struct asserter
{
   static cutee::test_suite* _suite_ptr;

   static void __set_suite_ptr(cutee::test_suite* suite_ptr)
   {
      _suite_ptr = suite_ptr;
   }

   static void __unset_suite_ptr()
   {
      _suite_ptr = nullptr;
   }

   static void __assert_suite_ptr()
   {
      assert(_suite_ptr != nullptr);
   }
   
   /**
    * Assertions
    **/
   /* Assert float equal with precision */
   template<class T, class U>
   static void assert_equal(T&& t, U&& u, info&& i)
   {
      __assert_suite_ptr();
      _suite_ptr->execute_assertion
         (  assertion<T, U>{
               [](T&& lhs, U&& rhs){
                  return (lhs == rhs);
               }
               ,  std::forward_as_tuple(std::forward<T>(t), std::forward<U>(u))
               ,  std::move(i)
            }
         
         );
   }

   /* Assert float equal with precision */
   template<class T, class U, class I>
   static void assert_float_equal_prec(T&& t, U&& u, I&& ulps, info&& i)
   {
      __assert_suite_ptr();
      _suite_ptr->execute_assertion
         (  assertion<T, U, I>{
               [](T&& lhs, U&& rhs, I&& ulps){
                  return cutee::numeric::float_eq(lhs, rhs, ulps);
               }
               ,  std::forward_as_tuple(std::forward<T>(t), std::forward<U>(u), std::forward<I>(ulps))
               ,  std::move(i)
            }
         
         );
   }
};


} /* namespace cutee */

/**
 * Assertion Macros
 **/
#define F_UNIT_ASSERT_FEQUAL(a, b, c) \
   cutee::asserter::assert_float_equal_prec(a, b, 2, cutee::info{c, __FILE__, __LINE__});

#define F_UNIT_ASSERT_EQUAL(a, b, c) \
   cutee::asserter::assert_equal(a, b, cutee::info{c, __FILE__, __LINE__});

#endif /* CUTEE_ASSERTER_HPP_INCLUDED */
