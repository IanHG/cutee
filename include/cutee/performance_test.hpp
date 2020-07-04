#pragma once
#ifndef CUTEE_PERFORMANCE_TEST_H_INCLUDED
#define CUTEE_PERFORMANCE_TEST_H_INCLUDED

#include<iostream>

#include "test.hpp"
#include "timer.hpp"

namespace cutee
{

template<typename T>
class performance_test
   :  public test_impl<T>      /* using inheritance for EBCO (empty base class optimization) */
{
   using underlying_type = test_impl<T>;

   private:
      clock_timer m_timer;
      int         m_ntimes;

   public:
      template<class... Ts>
      performance_test(int ntimes, Ts&&... ts)
         :  underlying_type(std::forward<Ts>(ts)...)
         ,  m_timer()
         ,  m_ntimes(ntimes)
      { 
      }

      virtual void run() override
      { 
         // Run test 
         for(size_t i = 0; i < this->m_ntimes; ++i) // loop over repeats
         {
            underlying_type::setup();

            // Start timer 
            m_timer.start();

            underlying_type::run(); // run the test

            // Start timer 
            m_timer.stop();

            underlying_type::teardown();
         }
      }

      virtual std::string message() const override
      {
         std::stringstream sstr;

         sstr << " TEST: " << this->name() << "\n"
              << " did "   << m_ntimes << " runs and"
              << " used: " << m_timer.tot_clocks() << " clocks "
              << " (in "   << m_timer.tot_clocks_per_sec() << "s)."
              << std::endl;

         return sstr.str();
      }

      virtual std::string name() const override
      {
         return underlying_type::name() + " (performance)";
      }
};

//
template
   <  class T
   ,  typename... Args
   >
test_ptr_t create_performance_test(int ntimes, const std::string& a_name, Args&&... args)
{
   return test_ptr_t{ new performance_test<T>(ntimes, a_name, std::forward<Args>(args)...) };
}

} /* namespace cutee */

#endif /* CUTEE_PERFORMANCE_TEST_H_INCLUDED */
