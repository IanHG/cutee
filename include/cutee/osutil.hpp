#pragma once
#ifndef CUTEE_OSUTIL_HPP_INCLUDED
#define CUTEE_OSUTIL_HPP_INCLUDED

#include <iostream>
#include <typeinfo>

#include "meta.hpp"

/**
 * Demangling stuff
 **/
#if defined(CUTEE_OSTREAM_UTILITY_TYPE) && defined(__GNUG__)
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace cutee
{
namespace detail
{
// Demangle function names
inline std::string demangle(const char* name) 
{
    int status = -4; // some arbitrary value to eliminate the compiler warning
   
    // Call __cxa_demangle
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}
} /* namespace detail */
} /* namespace cutee */
#else
namespace cutee
{
namespace detail
{
// does nothing if not g++
inline std::string demangle(const char* name) 
{
    return name;
}
} /* namespace detail */
} /* namespace cutee */
#endif /* CUTEE_OSTREAM_UTILITY_TYPE && __GNUG__ */

namespace cutee
{
namespace detail
{

CREATE_MEMBER_FUNCTION_CHECKER(begin)
CREATE_MEMBER_FUNCTION_CHECKER(end)

PRAGMA_PUSH
template<class C>
struct exists_operator_output_utilp
{
   private:
      template<class T>
      static constexpr auto check(T*)
         -> typename std::is_same<decltype((std::declval<std::ostream&>()) << (std::declval<T>())), std::ostream&>::type;

      template<class>
      static constexpr std::false_type check(...);
      
      using type = decltype(check<C>(nullptr));

   public:
      static constexpr bool value = type::value;
};
PRAGMA_POP

/**
 * Get demangled type as string
 **/
template<class T>
std::string type_of()
{
   return demangle(typeid(T).name());
}

} /* namespace detail */ 

} /* namespace cutee */

#ifdef CUTEE_OSTREAM_UTILITY
/**
 * Awesome output operator for iterable type.
 **/
template
   <  class ITERABLE
   ,  std::enable_if_t
         <  (  cutee::detail::has_begin<ITERABLE, typename ITERABLE::iterator()>::value
            && cutee::detail::has_end  <ITERABLE, typename ITERABLE::iterator()>::value
            && !cutee::detail::exists_operator_output_utilp<ITERABLE>::value
            )
         ,  void*
         > = nullptr
   >
std::ostream& operator<<(std::ostream& os, const ITERABLE& cont)
{
   os << "(";
   for(auto it = cont.begin(), end = cont.end(); it != end; )
   {
      os << *it;
      if (++it != end)
      {
         os << ", ";
      }
   }
   os << ")";
   return os;
}
#endif /* CUTEE_OSTREAM_UTILITY */

#endif /* CUTEE_OSUTIL_HPP_INCLUDED */
