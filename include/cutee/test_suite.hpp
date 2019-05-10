#ifndef CUTEE_TEST_SUITE_H
#define CUTEE_TEST_SUITE_H

#include <iostream>

#include "unit_test.hpp"
#include "unit_test_factory.hpp"
#include "timer.hpp"
#include "exceptions.hpp"
#include "message.hpp"

namespace cutee
{

class test_suite
   :  public unit_test_holder
{
   private:
      typedef unsigned int count_type;
      std::string m_suite_name;
      clock_timer m_timer;
      count_type m_assertions;
      count_type m_failed;
      count_type m_num_test;
      std::vector<std::string> m_failed_tests;
      message::format _format = message::fancy;
      
      void end_output(std::ostream& a_ostream)
      {
         // output header
         a_ostream << "======================================================================\n"
                   << "   NAME: " << m_suite_name << "\n"
                   << "----------------------------------------------------------------------\n"
                   << "   TESTS TO BE RUN: " << unit_test_holder::test_size() << "\n";

         // output tests that should be run
         for(decltype(unit_test_holder::test_size()) i = 0; i < unit_test_holder::test_size(); ++i)
            a_ostream << "      " << this->get_test(i)->name() << "\n";
         
         // output some stats
         a_ostream << "----------------------------------------------------------------------\n"
                   << "   STATISTICS:\n"
                   << "      Finished tests in " << m_timer.tot_clocks_per_sec() << "s, "
                   << m_num_test/m_timer.tot_clocks_per_sec() << " tests/s, "
                   << m_assertions/m_timer.tot_clocks_per_sec() << " assertions/s \n"
                   << "      " << m_num_test << " tests, "
                   << m_assertions << " assertions, "
                   << m_failed << " failed\n";
         
         // output failed tests
         if(m_failed_tests.size())
         {
            a_ostream << "----------------------------------------------------------------------\n"
                      << "   FAILED TESTS:\n";
            for(decltype(m_failed_tests.size()) i = 0; i < m_failed_tests.size(); ++i)
            {
               a_ostream << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         << m_failed_tests[i] << "\n";
            }
         }
         
         // output bottom
         a_ostream << "----------------------------------------------------------------------\n"
                   << (m_failed ? "   UNIT TEST FAILED!\n" : "   SUCCESS\n")
                   << "======================================================================\n" << std::flush;
      }
   public:
      test_suite(std::string a_suite_name = "default_suite")
         : 
            unit_test_holder()
         ,  m_suite_name(a_suite_name)
         ,  m_timer()
         ,  m_assertions(0)
         ,  m_failed(0)
         ,  m_num_test(0)
      { }
      ~test_suite() = default;

      void do_tests(std::ostream& a_ostream = std::cout)
      { 
         m_timer.start();
         for(decltype(test_size()) i=0; i<test_size(); ++i)
         {
            get_test(i)->setup();    // setup test
            try
            {
               get_test(i)->do_test();
            }
            catch(const test_failed &e)
            {
               m_failed_tests.emplace_back(get_test(i)->name() + "\n" + e.what());
               ++m_failed;
            }
            catch(const exception::failed& e)
            {
               m_failed_tests.emplace_back(get_test(i)->name() + "\n" + e.what());
               ++m_failed;
            }
            catch(const std::exception& e)
            {
               m_failed_tests.emplace_back(get_test(i)->name() + "\nstd::exception: " + e.what());
               ++m_failed;
            }
            catch(...)
            {
               m_failed_tests.emplace_back(get_test(i)->name() + "\n TEST SUITE CAUGHT SOMETHING ");
               ++m_failed;
            }

            m_assertions+=get_test(i)->num_assertions();
            m_num_test+=get_test(i)->num_test();
            m_failed+=get_test(i)->num_failed(); // for test case (unit test will return 0 no matter)
            get_test(i)->teardown(); // teardown test
         }
         m_timer.stop();
         end_output(a_ostream);
      }
      
      /**
       * Execute assertion
       **/
      template<class... Ts>
      void execute_assertion(assertion<Ts...>&& asrt)
      {
         // Do some accounting
         m_assertions += 1;
         
         // Perform assertion
         if(!asrt.execute())
         {
            throw exception::assertion_failed(std::move(asrt), _format);
         }
      }

      /*!
       * Interface for running the test suite.
       */
      void run(std::ostream& os = std::cout)
      {
         this->do_tests(os);
      }
};

using suite = test_suite;

} /* namespace cutee */

#endif /* CUTEE_TEST_SUITE_H */
