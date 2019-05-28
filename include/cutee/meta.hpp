#pragma once
#ifndef CUTEE_META_HPP_INCLUDED
#define CUTEE_META_HPP_INCLUDED

#include <type_traits>
#include <complex>
#include <vector>

namespace cutee
{

/**
 * Function checkers.
 **/
#undef PRAGMA_PUSH
#undef PRAGMA_POP

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

/**
 * Check if type is a std::complex
 **/
template<class T>
struct is_complex
   :  public std::false_type
{
};

template<class T>
struct is_complex<std::complex<T> >
   :  public std::true_type
{
};

template<class T>
constexpr auto is_complex_v = is_complex<T>::value;

/**
 * Check if type is a std::vector
 **/
template<class T>
struct is_vector
   :  public std::false_type
{
};

template<class T, class U>
struct is_vector<std::vector<T, U> >
   :  public std::true_type
{
};

template<class T>
constexpr auto is_vector_v = is_vector<T>::value;

/**
 * Type sink for defining void on template parameters
 **/
template<class... Ts>
struct type_sink
{
   using type = void;
};

} /* namespace cutee */

#endif /* CUTEE_META_HPP_INCLUDED */
