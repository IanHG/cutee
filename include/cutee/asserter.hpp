#ifndef CUTEE_ASSERTER_HPP_INCLUDED
#define CUTEE_ASSERTER_HPP_INCLUDED

#include <tuple>
#include <functional>
#include <cassert>

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
   template<class T>
   static void assertt(T&& t, info&& i)
   {
      __assert_suite_ptr();
      _suite_ptr->execute_assertion
         (  assertion<T>{
               [](T&& lhs){
                  return bool(lhs);
               }
               ,  std::forward_as_tuple(std::forward<T>(t))
               ,  std::move(i)
            }
         
         );
   }
   
   /* Assert not */
   template<class T>
   static void assert_not(T&& t, info&& i)
   {
      __assert_suite_ptr();
      i._type = assertion_type::not_equal;
      _suite_ptr->execute_assertion
         (  assertion<T>{
               [](T&& lhs){
                  return !bool(lhs);
               }
               ,  std::forward_as_tuple(std::forward<T>(t))
               ,  std::move(i)
            }
         
         );
   }

   /* Assert equal */
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
   template<class T, class U>
   static void assert_not_equal(T&& t, U&& u, info&& i)
   {
      __assert_suite_ptr();
      i._type = assertion_type::not_equal;
      _suite_ptr->execute_assertion
         (  assertion<T, U>{
               [](T&& lhs, U&& rhs){
                  return (lhs != rhs);
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
   
   /* Assert float equal to zero in comparisson with number with precision */
   template<class T, class U, class I>
   static void assert_float_numeq_zero_prec(T&& t, U&& u, I&& ulps, info&& i)
   {
      __assert_suite_ptr();
      i._type = assertion_type::comp_zero;
      _suite_ptr->execute_assertion
         (  assertion<T, U, I>{
               [](T&& lhs, U&& rhs, I&& ulps){
                  return cutee::numeric::float_numeq_zero(lhs, rhs, ulps);
               }
               ,  std::forward_as_tuple(std::forward<T>(t), std::forward<U>(u), std::forward<I>(ulps))
               ,  std::move(i)
            }
         
         );
   }
};

/**
 * Used by __function_wrapper to assert "bool" function.
 **/
inline void unit_assert_fcn(bool check, const std::string& message, const char* file, int line)
{
   cutee::asserter::assertt(check, cutee::info{message, file, line});
}

} /* namespace cutee */

#endif /* CUTEE_ASSERTER_HPP_INCLUDED */
