#pragma once
#ifndef CUTEE_TEST_SUITE_HPP_INCLUDED
#define CUTEE_TEST_SUITE_HPP_INCLUDED

#include <iostream>

#include "test.hpp"
#include "container.hpp"

#include "timer.hpp"
#include "exceptions.hpp"
#include "message.hpp"
#include "writer.hpp"

namespace cutee
{

class suite
   :  public container
{
   using counter_type = unsigned int;
   using writer_ptr_t = const writer*; 
   
   template<class I>
   struct counter
   {
      using counter_type = I;
   
      counter_type _num_tests      = static_cast<counter_type>(0);
      counter_type _num_assertions = static_cast<counter_type>(0); 
      counter_type _num_failed     = static_cast<counter_type>(0);

      void reset()
      {
         this->_num_tests      = 0;
         this->_num_assertions = 0;
         this->_num_failed     = 0;
      }
   };

   private:
      std::string            _name = "";
      counter<counter_type>  _counter;
      clock_timer            _timer;
      bool                   _first  = false;
      writer_ptr_t           _writer = writer_ptr_t{ nullptr };
      
      /* Create message strings */
      std::string create_header_message()       const;
      std::string create_statistics_message()   const;
      std::string create_footer_message()       const;
       
      std::string create_failed_message(const std::string& name, const std::string& msg)
      {
         std::stringstream sstr;
         if(this->_first)
         {
            sstr << "----------------------------------------------------------------------\n"
                 << "   FAILED TESTS:\n";
            this->_first = false;
         }

         sstr   << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                << "[/name_color]"<< "   *** " << name << " ***\n" << "[/default_color]"
                << msg;

         return sstr.str();
      }

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

      friend struct asserter;
      friend class collection;

      void run_test(test_interface&);

   public:
      /**
       * Constructor
       **/
      suite
         (  const std::string& name = "default_suite"
         )
         :  _name(name)
      { 
      }
      
      /**
       * Default destructor
       **/
      ~suite() = default;
      
      /*!
       * Old interface for running the test suite.
       */
      bool do_tests(std::ostream& os = std::cout, const format& form = format::fancy);

      /*!
       *
       */
      bool do_tests(const writer&);

      /*!
       * Interface for running the test suite.
       */
      bool run(std::ostream& os = std::cout, const format& form = format::fancy)
      {
         return this->do_tests(os, form);
      }
      
      bool run(const writer& w)
      {
         return this->do_tests(w);
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
inline std::string suite::create_header_message() const
{
   std::stringstream sstr;
   // output header
   sstr  << "[/bold_on]"
         << "======================================================================\n"
         << "   NAME: " << "[/name_color]" << this->_name << "[/default_color]" << "\n"
         << "----------------------------------------------------------------------\n"
         << "   TESTS TO BE RUN: " << this->test_size() << "\n";

   // output tests that should be run
   for(decltype(this->test_size()) i = 0; i < this->test_size(); ++i)
   {
      sstr << "      " << const_cast<suite*>(this)->get_test(i)->name() << "\n";
   }

   return sstr.str();
}
 
/**
 *
 **/
inline std::string suite::create_statistics_message() const
{
   std::stringstream sstr;
   sstr << "----------------------------------------------------------------------\n"
        << "   STATISTICS:\n"
        << "      Finished tests in " << _timer.tot_clocks_per_sec()  << "s, "
        << _counter._num_tests     /_timer.tot_clocks_per_sec()       << " tests/s, "
        << _counter._num_assertions/_timer.tot_clocks_per_sec()       << " assertions/s \n"
        << "      " 
        << _counter._num_tests       << " tests, "
        << _counter._num_assertions  << " assertions, "
        << _counter._num_failed      << " failed\n";
   return sstr.str();
}

/**
 *
 **/
inline std::string suite::create_footer_message() const
{
   std::stringstream sstr;
   sstr << "----------------------------------------------------------------------\n"
        << (_counter._num_failed ? "[/warning_color]" : "[/file_color]")
        << (_counter._num_failed ? "   UNIT TEST FAILED!\n" : "   SUCCESS\n")
        << "[/default_color]"
        << "======================================================================\n"
        << "[/bold_off]";
   return sstr.str();
}

/**
 *
 **/
inline void suite::run_test(test_interface& t)
{
   // Setup
   t.setup();

   // Run
   try
   {
      t.run();
   }
   catch(const exception::failed& e)
   {
      this->_writer->write(this->create_failed_message(t.name(), e.what()));
      _counter._num_failed += 1;
   }
   catch(const std::exception& e)
   {
      std::string message = "   std::exception \n";
      message += e.what();
      message += "\n";
      this->_writer->write(this->create_failed_message(t.name(), std::move(message)));
      _counter._num_failed += 1;
   }
   catch(...)
   {
      this->_writer->write(this->create_failed_message(t.name(), "   cutee::suite caught \"something\"...\n"));
      _counter._num_failed += 1;
   }

   // Count
   _counter._num_tests += 1;
   
   // Teardown
   t.teardown();
}


/**
 *
 **/
inline bool suite::do_tests
   (  std::ostream&        os
   ,  const cutee::format& form
   )
{
   auto w = writer_ptr_t{new formated_writer{os, form}};
   this->do_tests(*w);
}

inline bool suite::do_tests
   (  const writer& w
   )
{
   asserter::__set_suite_ptr(this);
   this->_counter.reset();
   this->_first  = true;
   this->_writer = &w; //
   this->_writer->write(this->create_header_message());
   
   // Start timer
   _timer.start();
   
   // Run tests
   for(decltype(test_size()) i=0; i<test_size(); ++i)
   {
      this->run_test(*(get_test(i)));
   }
   
   // Stop timer
   _timer.stop();
   
   // Print footer
   this->_writer->write(this->create_statistics_message());
   this->_writer->write(this->create_footer_message());
   
   // Clean-up
   asserter::__unset_suite_ptr();
   this->_writer = writer_ptr_t{nullptr};

   return this->_counter._num_failed == 0;
}

} /* namespace cutee */

#endif /* CUTEE_TEST_SUITE_HPP_INCLUDED */
