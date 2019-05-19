#pragma once
#ifndef CUTEE_CONTAINER_HPP_INCLUDED
#define CUTEE_CONTAINER_HPP_INCLUDED

#include <string>
#include <vector>

//#include "asserter.hpp"
#include "test.hpp"
#include "function.hpp"
#include "performance_test.hpp"

namespace cutee
{

//
class container
{
   private:
      using test_container_t = std::vector<test_ptr_t>;
      
      //
      test_container_t m_tests;
   
   public:
      // virtual destructor
      virtual ~container()
      { 
      }

      //
      // add tests (T is test class type) with name defaulted to default-name
      //
      template<class T>
      void add_test(const std::string a_name=default_test_name::acquire_name())
      { 
         //m_tests.push_back(unit_test_factory<T>(a_name)); 
         m_tests.push_back(test_create<T>(a_name)); 
      }
      
      //
      // add test with extra arguments
      //
      template<class T, typename... Args>
      void add_test(const std::string a_name, Args&&... args)
      { 
         //m_tests.push_back(unit_test_factory<T>(a_name, std::forward<Args>(args)...)); 
         m_tests.push_back(test_create<T>(a_name, std::forward<Args>(args)...)); 
      }
      
      //
      // add test with extra arguments
      //
      template<class T, typename... Args>
      void add(Args&&... args)
      { 
         this->add_test(std::forward<Args>(args)...);
      }

      template<class... Args>
      void add_function(Args&&... args)
      {
         this->add_test<__function_wrap<Args...> >(std::forward<Args>(args)...);
      }
      
      ////
      //// add performance tests (N is num runs, T is test class type)
      ////
      //template<int N, class T>
      //void add_performance_test(const std::string a_name=default_test_name::acquire_name())
      //{ 
      //   m_tests.push_back(performance_test_factory<T,N>(a_name)); 
      //}
      //                        
      ////
      //// add performance tests (N is num runs, T is test class type)
      ////
      //template<int N, class T, typename... Args>
      //void add_performance_test(const std::string a_name, Args&&... args)
      //{ 
      //   m_tests.push_back(performance_test_factory<T,N>(a_name, std::forward<Args>(args)...)); 
      //}
      
      //
      // get test number i
      //
      test_ptr_t& get_test(int i)
      { 
         return m_tests[i]; 
      }
      
      //
      // get number of tests
      //
      auto test_size() const 
      { 
         return m_tests.size(); 
      }
      
      //
      // get number of tests
      //
      auto size() const 
      { 
         return m_tests.size(); 
      }
   
};

} /* namespace cutee */

#endif /* CUTEE_CONTAINER_HPP_INCLUDED */
