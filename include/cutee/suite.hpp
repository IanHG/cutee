#ifndef CUTEE_TEST_SUITE_HPP_INCLUDED
#define CUTEE_TEST_SUITE_HPP_INCLUDED

#include <iostream>

#include "unit_test.hpp"
#include "unit_test_factory.hpp"

#include "timer.hpp"
#include "exceptions.hpp"
#include "message.hpp"

namespace cutee
{

class suite
   :  public unit_test_holder
{
   using counter_type   = unsigned int;
   using formater_ptr_t = typename format::formater_ptr_t;
   
   template<class I>
   struct counter
   {
      using counter_type = I;
   
      counter_type _num_tests      = static_cast<counter_type>(0);
      counter_type _num_assertions = static_cast<counter_type>(0); 
      counter_type _num_failed     = static_cast<counter_type>(0);
   };

   private:
      std::string             _name = "";
      counter<counter_type>   _counter;
      clock_timer             _timer;
      formater_ptr_t          _formater = formater_ptr_t{ nullptr };
      
      void header_to_stream(std::ostream& a_ostream);
       
      void statistics_to_stream(std::ostream& a_ostream);

      void footer_to_stream(std::ostream& a_ostream);
       
      void failed_to_stream(std::ostream& a_ostream, const std::string& name, const std::string& msg, bool& first)
      {
         if(first)
         {
            a_ostream << "----------------------------------------------------------------------\n"
                      << "   FAILED TESTS:\n";
            first = false;
         }

         a_ostream   << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                     << _formater->name_color() << "   *** " << name << " ***\n" << _formater->default_color()
                     << msg;
      }
   public:
      suite
         (  const std::string& name = "default_suite"
         )
         :  unit_test_holder()
         ,  _name(name)
      { 
      }
      
      ~suite() = default;

      void do_tests(std::ostream& a_ostream = std::cout, const format& form = format::fancy);
      
      /**
       * Execute assertion
       **/
      template<class... Ts>
      void execute_assertion(assertion<Ts...>&& asrt)
      {
         // Do some accounting
         _counter._num_assertions += 1;
         
         // Perform assertion
         if(!asrt.execute())
         {
            throw exception::assertion_failed(std::move(asrt));
         }
      }

      /*!
       * Interface for running the test suite.
       */
      void run(std::ostream& os = std::cout, const format& form = format::fancy)
      {
         this->do_tests(os);
      }
};

// For backwards compatibility
using test_suite = suite;
 
} /* namespace cutee */

#include "asserter.hpp"

namespace cutee 
{

/**
 *
 **/
void suite::header_to_stream(std::ostream& a_ostream)
{
   // output header
   a_ostream << _formater->bold_on();
   a_ostream << "======================================================================\n"
             << "   NAME: " << _formater->name_color() << this->_name << _formater->default_color() << "\n"
             << "----------------------------------------------------------------------\n"
             << "   TESTS TO BE RUN: " << unit_test_holder::test_size() << "\n";

   // output tests that should be run
   for(decltype(unit_test_holder::test_size()) i = 0; i < unit_test_holder::test_size(); ++i)
      a_ostream << "      " << this->get_test(i)->name() << "\n";
}
 
/**
 *
 **/
void suite::statistics_to_stream(std::ostream& a_ostream)
{
   a_ostream << "----------------------------------------------------------------------\n"
             << "   STATISTICS:\n"
             << "      Finished tests in " << _timer.tot_clocks_per_sec()  << "s, "
             << _counter._num_tests     /_timer.tot_clocks_per_sec()       << " tests/s, "
             << _counter._num_assertions/_timer.tot_clocks_per_sec()       << " assertions/s \n"
             << "      " 
             << _counter._num_tests       << " tests, "
             << _counter._num_assertions  << " assertions, "
             << _counter._num_failed      << " failed\n";
}

/**
 *
 **/
void suite::footer_to_stream(std::ostream& a_ostream)
{
   a_ostream << "----------------------------------------------------------------------\n"
             << (_counter._num_failed ? _formater->warning_color() : _formater->file_color())
             << (_counter._num_failed ? "   UNIT TEST FAILED!\n" : "   SUCCESS\n")
             << _formater->default_color()
             << "======================================================================\n" << std::flush;
   a_ostream << _formater->bold_off();
}

/**
 *
 **/
void suite::do_tests
   (  std::ostream& a_ostream
   ,  const cutee::format& form
   )
{ 
   asserter::__set_suite_ptr(this);
   this->_formater = format::create(form);
   this->header_to_stream(a_ostream);
   bool first = true;

   _timer.start();
   
   // Run tests
   for(decltype(test_size()) i=0; i<test_size(); ++i)
   {
      // Setup
      get_test(i)->setup();

      // Run
      try
      {
         get_test(i)->do_test();
      }
      catch(const exception::failed& e)
      {
         this->failed_to_stream(a_ostream, get_test(i)->name(), _formater->replace_in_string(e.what()), first);
         _counter._num_failed += 1;
      }
      catch(const std::exception& e)
      {
         std::string message = "   std::exception \n";
         message += e.what();
         message += "\n";
         this->failed_to_stream(a_ostream, get_test(i)->name(), _formater->replace_in_string(std::move(message)), first);
         _counter._num_failed += 1;
      }
      catch(...)
      {
         this->failed_to_stream(a_ostream, get_test(i)->name(), "   cutee::suite caught \"something\"...\n", first);
         _counter._num_failed += 1;
      }

      // Count
      _counter._num_tests      += get_test(i)->num_test();
      _counter._num_assertions += get_test(i)->num_assertions();
      _counter._num_failed     += get_test(i)->num_failed(); // for test case (unit test will return 0 no matter)
      
      // Teardown
      get_test(i)->teardown();
   }
   
   // Stop timer
   _timer.stop();
   
   // Print footer
   this->statistics_to_stream (a_ostream);
   this->footer_to_stream     (a_ostream);
   
   // Clean-up
   asserter::__unset_suite_ptr();
}

} /* namespace cutee */

#endif /* CUTEE_TEST_SUITE_HPP_INCLUDED */
