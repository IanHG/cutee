#pragma once
#ifndef CUTEE_PERFORMANCE_TEST_H_INCLUDED
#define CUTEE_PERFORMANCE_TEST_H_INCLUDED

#include<iostream>

#include "test.hpp"
#include "timer.hpp"

namespace cutee
{

template<size_t repeats, typename T>
class performance_test
   :  public  virtual test
   ,  private virtual T       /* using inheritance for EBCO (empty base class optimization) */
{
   private:
      clock_timer m_timer;

      void benchmark_output(std::ostream& a_stream = std::cout) const
      {
         a_stream << " TEST: " << name() << "\n"
                  << " did "   << repeats << " runs and "
                  << " used: " << m_timer.tot_clocks() << " clocks "
                  << " (in "   << m_timer.tot_clocks_per_sec() << "s)."
                  << std::endl;
      }

   public:
      template<typename... Args>
      performance_test(const std::string a_name, const Args&... args): 
         unit_test(), test(a_name,args...), m_timer()
      { }

      void run()
      { 
         // Start timer 
         m_timer.start();

         // Run test 
         for(size_t i = 0; i < repeats; ++i) // loop over repeats
         {
            T::run(); // run the test
         }

         // Stop timer
         m_timer.stop();
         
         // Print some benchmark
         benchmark_output();
      }
};

} /* namespace cutee */

#endif /* CUTEE_PERFORMANCE_TEST_H_INCLUDED */
