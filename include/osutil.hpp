#ifndef CUTEE_OSUTIL_HPP_INCLUDED
#define CUTEE_OSUTIL_HPP_INCLUDED

#include <iostream>
#include <typeinfo>

namespace cutee
{
namespace detail
{
/**
 * Demangling stuff
 **/
#if defined(CUTEE_OSTREAM_UTILITY_TYPE) && defined(__GNUG__)
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

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
#else
// does nothing if not g++
inline std::string demangle(const char* name) 
{
    return name;
}
#endif

/**
 * Function checkers.
 **/
#ifdef __clang__ 
#define PRAGMA_PUSH \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wundefined-inline\"") 
#define PRAGMA_POP \
_Pragma("clang diagnostic pop") 
#else
#define PRAGMA_PUSH
#define PRAGMA_POP
#endif 

#define CREATE_MEMBER_FUNCTION_CHECKER(NAME) \
template<class, class T> \
struct has_##NAME \
{ \
   static_assert(std::integral_constant<T, false>::value, "Must provide function signature."); \
}; \
PRAGMA_PUSH \
template<class C, class Ret, class... Args> \
struct has_##NAME<C, Ret(Args...)> \
{ \
   private: \
      template<class T> \
      static constexpr auto check(T*) \
         -> typename std::is_same<decltype(std::declval<T>().NAME(std::declval<Args>()...) ), Ret>::type; \
 \
      template<class> \
      static constexpr std::false_type check(...); \
 \
      using type = decltype(check<C>(0)); \
 \
   public: \
      static constexpr bool value = type::value; \
}; \
PRAGMA_POP

CREATE_MEMBER_FUNCTION_CHECKER(begin)
CREATE_MEMBER_FUNCTION_CHECKER(end)

#undef CREATE_FUNCTION_CHECKER

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

#undef PRAGMA_PUSH
#undef PRAGMA_POP

} /* namespace detail */ 

} /* namespace cutee */

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
#if defined(CUTEE_OSTREAM_UTILITY_TYPE)
   os << demangle(typeid(cont).name()) << " (";
#else
   os << "(";
#endif /* CUTEE_OSTREAM_UTILITY_TYPE */
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

#endif /* CUTEE_OSUTIL_HPP_INCLUDED */
