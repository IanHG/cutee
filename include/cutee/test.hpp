#pragma once
#ifndef CUTEE_UNIT_TEST_H
#define CUTEE_UNIT_TEST_H

#include <string>
#include <memory>
#include <type_traits>

#include "meta.hpp"
#include "osutil.hpp"

namespace cutee
{

struct test;

struct test
{
   public:
      //
      virtual ~test() = 0;
      
      // overloadable function for doing the actual test
      virtual void run() = 0;
      
      // overloadable function for optional setup test method
      virtual void setup() { }
      
      // overloadable function for optional teardown test method
      virtual void teardown() { }
      
      // interface function for getting name of test
      virtual std::string name() const = 0;
};

//
inline test::~test()
{
}

#define PRAGMA_PUSH
#define PRAGMA_POP

CREATE_MEMBER_FUNCTION_CHECKER(run)
CREATE_MEMBER_FUNCTION_CHECKER(setup)
CREATE_MEMBER_FUNCTION_CHECKER(teardown)
CREATE_MEMBER_FUNCTION_CHECKER(name)

struct empty
{
};

template<class T>
struct test_impl
   :  public std::conditional_t<std::is_base_of_v<test, T>, empty, test>
   ,  public T
{
   private:
      std::string _name;

   public:
      template<class... Ts>
      test_impl(const std::string& name, Ts&&... ts)
         :  T(std::forward<Ts>(ts)...)
         ,  _name(name)
      {
      }

      void run() override
      {
         if constexpr(has_run_v<T, void()>)
         {
            T::run();
         }
         else
         {
            static_assert(false_on_ts_v<T>, "No run function supplied.");
         }
      }
      
      void setup() override
      {
         if constexpr(has_setup_v<T, void()>)
         {
            T::setup();
         }
      }
      
      void teardown() override
      {
         if constexpr(has_teardown_v<T, void()>)
         {
            T::teardown();
         }
      }
      
      // interface function for getting name of test
      std::string name() const override
      {
         if constexpr(has_name_v<T, std::string()>)
         {
            return _name + T::name();
         }
         else
         {
            return _name;
         }
      }
};

using test_ptr_t = std::unique_ptr<test>;

//
template
   <  class T
   ,  typename... Args
   >
test_ptr_t test_create(const std::string a_name, Args&&... args)
{
   return test_ptr_t{ new test_impl<T>(a_name, std::forward<Args>(args)...) };
}

// Backwards compatibility
using unit_test = test;

class default_test_name
{
   public:
      static std::string acquire_name()
      { 
         ++m_num; 
         return m_name+"_"+std::to_string(m_num); 
      }
   
   private:
      static std::string  m_name;
      static unsigned int m_num;
};

} /* namespace cutee */

#endif /* CUTEE_UNIT_TEST_H */
