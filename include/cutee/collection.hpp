#pragma once
#ifndef CUTEE_COLLECTION_HPP_INCLUDED
#define CUTEE_COLLECTION_HPP_INCLUDED

#include <iostream>

#include "test.hpp"
#include "exceptions.hpp"

#include "container.hpp"
#include "suite.hpp"

namespace cutee
{

class collection
   :  virtual protected container
   ,  virtual public    test
{
   public:
      collection()            = default;
      virtual ~collection()   = default;
      
      // Run the collection
      void run()
      {
         for(decltype(test_size()) i=0; i<test_size(); ++i)
         {
            asserter::_suite_ptr->run_test(*(this->get_test(i)));
         }
      }
      
      // Get name of collection (concat all individual test names)
      std::string name() const
      {
         std::stringstream sstr;
         for(decltype(test_size()) i=0; i<test_size(); ++i)
         {
            sstr << "\n         " << const_cast<collection*>(this)->get_test(i)->name();
         }
         return sstr.str();
      }
};

// for backwards compatibility
using test_case = collection;

} /* namespace cutee */

#endif /* CUTEE_COLLECTION_HPP_INCLUDED */
