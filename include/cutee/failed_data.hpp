#ifndef CUTEE_FAILED_DATA_H_INCLUDED
#define CUTEE_FAILED_DATA_H_INCLUDED

#include<string>
#include<iostream>
#include<sstream>

#include<type_traits>
#include "message.hpp"

namespace cutee
{

//
//
//
struct failed_data_base
{
   public:
      virtual void expected(std::stringstream&) const = 0;
      virtual void got(std::stringstream&)      const = 0;

      virtual ~failed_data_base() 
      { 
      }
};

//
//
//
template<typename T, typename U>
struct failed_data: public failed_data_base
{
   public:
      failed_data(const T& a_expected, const U& a_got):
         m_expected(a_expected), m_got(a_got) 
      { 
      }
      
      void expected(std::stringstream& s) const 
      { 
         this->expected_impl<>(s); 
      }
      
      void got(std::stringstream& s) const 
      { 
         this->got_impl<>(s); 
      }
   
   private:
      /**
       * Print expected
       **/
      template
         <  class V = T
         ,  std::enable_if_t<detail::exists_operator_output<V>::value, void*> = nullptr
         >
      void expected_impl(std::stringstream& s) const 
      { 
         s << m_expected; 
      }
      
      template
         <  class V = T
         ,  std::enable_if_t<!detail::exists_operator_output<V>::value, void*> = nullptr
         >
      void expected_impl(std::stringstream& s) const 
      { 
         s << "No output operator for 'expected'"; 
      }

      /**
       * Print got
       **/
      template
         <  class V = U
         ,  std::enable_if_t<detail::exists_operator_output<V>::value, void*> = nullptr
         >
      void got_impl(std::stringstream& s)      const 
      { 
         s << m_got; 
      }
      
      template
         <  class V = U
         ,  std::enable_if_t<!detail::exists_operator_output<V>::value, void*> = nullptr
         >
      void got_impl(std::stringstream& s)      const 
      { 
         s << "No output operator for 'got'."; 
      }

      /**
       * Member variables
       **/
      const T m_expected;
      const U m_got;
};

//
//
//
template<typename T, typename U>
failed_data<T, U>* failed_data_factory(const T& a_expected, const U& a_got)
{ 
   return new failed_data<T, U>(a_expected,a_got); 
}

//
//
//
void failed_data_destroy(failed_data_base* a_failed_data);

} /* namespace cutee */

#endif /* CUTEE_TESTING_TEST_FAILED_H_INCLUDED */
