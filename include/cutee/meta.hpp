#pragma once
#ifndef CUTEE_META_HPP_INCLUDED
#define CUTEE_META_HPP_INCLUDED

#include <type_traits>

namespace cutee
{

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
#endif  /* __clang__ */ 

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
\
template<class C, class F>\
constexpr bool has_##NAME##_v = has_##NAME<C, F>::value;\
\
PRAGMA_POP

/**
 * False type on Ts...
 *
 * Useful for making static asserts depend on templates for defering assertion.
 **/
template<class... Ts>
struct false_on_ts
   :  public std::false_type
{
};

template<class... Ts>
constexpr auto false_on_ts_v = false_on_ts<Ts...>::value;

} /* namespace cutee */

#endif /* CUTEE_META_HPP_INCLUDED */
