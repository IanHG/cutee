#ifndef CUTEE_UNIT_TEST_FACTORY_H_INCLUDED
#define CUTEE_UNIT_TEST_FACTORY_H_INCLUDED

#include <string>
#include <vector>

//#include "asserter.hpp"
#include "unit_test.hpp"
#include "function.hpp"
#include "performance_test.hpp"

namespace cutee
{

//
//
//
template
   <  class T
   ,  typename... Args
   >
unit_test* unit_test_factory(const std::string a_name, Args&&... args)
{
   //return new T(args...);
   return new unit_test_name<T>(a_name, std::forward<Args>(args)...);
}

//
//
//
template
   <  class T
   ,  int N
   ,  typename... Args
   >
unit_test* performance_test_factory(const std::string a_name, Args&&... args)
{
   return new performance_test<N,unit_test_name<T> >(a_name, std::forward<Args>(args)...);
}

//
//
//
template<class T>
void unit_test_destroy(T* ptr)
{
   delete ptr;
}

//
//
//
class unit_test_holder
{
   public:
      //
      // virtual destructor
      //
      virtual ~unit_test_holder()
      { 
         for(decltype(m_tests.size()) i=0; i<m_tests.size(); ++i) 
            unit_test_destroy(m_tests[i]); 
      }

      //
      // add tests (T is test class type) with name defaulted to default-name
      //
      template<class T>
      void add_test(const std::string a_name=default_test_name::acquire_name())
      { 
         m_tests.push_back(unit_test_factory<T>(a_name)); 
      }
      
      //
      // add test with extra arguments
      //
      template<class T, typename... Args>
      void add_test(const std::string a_name, Args&&... args)
      { 
         m_tests.push_back(unit_test_factory<T>(a_name, std::forward<Args>(args)...)); 
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
      
      //
      // add performance tests (N is num runs, T is test class type)
      //
      template<int N, class T>
      void add_performance_test(const std::string a_name=default_test_name::acquire_name())
      { 
         m_tests.push_back(performance_test_factory<T,N>(a_name)); 
      }
                              
      //
      // add performance tests (N is num runs, T is test class type)
      //
      template<int N, class T, typename... Args>
      void add_performance_test(const std::string a_name, Args&&... args)
      { 
         m_tests.push_back(performance_test_factory<T,N>(a_name, std::forward<Args>(args)...)); 
      }
      
      //
      // get test number i
      //
      unit_test* get_test(int i) 
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
   
   private:
      std::vector<unit_test*> m_tests;
};

} /* namespace cutee */

#endif /* CUTEE_UNIT_TEST_FACTORY_H_INCLUDED */
