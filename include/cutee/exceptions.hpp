#ifndef CUTEE_EXCEPTIONS_HPP_INCLUDED
#define CUTEE_EXCEPTIONS_HPP_INCLUDED

#include <string>

#include "assertion.hpp"
#include "formater.hpp"
#include "message.hpp"

namespace cutee
{
namespace exception
{

/**
 * Cutee exception base class
 **/
struct failed
{
   virtual ~failed() noexcept = 0;
   virtual const char* what() const noexcept = 0;
};

failed::~failed() { }

/**
 * Failed assertion exception
 **/
struct assertion_failed
   :  public failed
{
   std::string _what;
   
   template<class... Ts>
   static std::string __construct_what
      (  const assertion<Ts...>& asrt
      ,  const formater&  form
      )
   {
      return message::generate(asrt, form);
   }
   
   template<class... Ts>
   assertion_failed
      (  const assertion<Ts...>& asrt
      ,  const formater&  form
      )
      :  _what(__construct_what(asrt, form))
   {
   }

   ~assertion_failed() = default;

   const char* what() const noexcept override
   {
      return _what.c_str();
   }
};

} /* namespace exception */
} /* namespace cutee */

#endif /* CUTEE_EXCEPTIONS_HPP_INCLUDED */
