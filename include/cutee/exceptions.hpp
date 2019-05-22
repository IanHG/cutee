#pragma once
#ifndef CUTEE_EXCEPTIONS_HPP_INCLUDED
#define CUTEE_EXCEPTIONS_HPP_INCLUDED

#include <string>

#include "assertion.hpp"
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

inline failed::~failed() noexcept { }

/**
 * Failed assertion exception
 **/
struct assertion_failed
   :  public failed
{
   std::string _what;
   
   template<class... Ts>
   static std::string construct_what__
      (  const assertion<Ts...>& asrt
      )
   {
      return message::generate(asrt);
   }
   
   template<class... Ts>
   assertion_failed
      (  const assertion<Ts...>& asrt
      )
      :  _what(construct_what__(asrt))
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
