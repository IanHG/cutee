#ifndef CUTEE_MESSAGE_H_INCLUDED
#define CUTEE_MESSAGE_H_INCLUDED

#include<string>
#include<iostream>
#include<sstream>
#include<type_traits>
#include<complex>

#include "assertion.hpp"
#include "float_eq.hpp"

namespace cutee
{

namespace detail
{
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

PRAGMA_PUSH
template<class C>
struct exists_operator_output
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

template<class T>
struct is_complex
   :  public std::false_type
{
};

template<class T>
struct is_complex<std::complex<T>>
   :  public std::true_type
{
   static_assert(std::is_floating_point<T>::value, "T must be floating-point type.");
};


template<bool>
struct output_float_distance
   :  public std::false_type
{
   template<class T>
   static void apply(std::stringstream& s, const T& got, const T& expected)
   {
      // do nothing
   }

   template<class T, class U>
   static void apply(std::stringstream& s, const T& got, const U& expected)
   {
      // do nothing
   }
};

// specialization for floating point
template<>
struct output_float_distance<true>
   :  public std::true_type
{
   template
      <  class T
      ,  std::enable_if_t<std::is_floating_point<T>::value, void*> = nullptr
      >
   static void apply(std::stringstream& s, const T& got, const T& expected)
   {
      s << " dist: " << numeric::float_ulps(expected, got);
   }
   
   template
      <  class T
      ,  std::enable_if_t<std::is_floating_point<T>::value, void*> = nullptr
      >
   static void apply(std::stringstream& s, const std::complex<T>& got, const std::complex<T>& expected)
   {
      s  << " dist: (" 
         << numeric::float_ulps(expected.real(), got.real()) 
         << ", "
         << numeric::float_ulps(expected.imag(), got.imag()) 
         << ")"
         ;
   }

   template
      <  class T
      ,  class U
      >
   static void apply(std::stringstream& s, const T& got, const U& expected)
   {
      //static_assert(std::is_same<T, U>::value, "not the same types.");
      s << " dist : N/A";
   }
};

/**
 * Print got
 **/
template
   <  class V 
   ,  std::enable_if_t<detail::exists_operator_output<V>::value, void*> = nullptr
   >
void operator_output(std::stringstream& s, const V& v)
{ 
   s << v;
}

template
   <  class V
   ,  std::enable_if_t<!detail::exists_operator_output<V>::value, void*> = nullptr
   >
void operator_output(std::stringstream& s, const V& v)
{ 
   s << "[ No output operator for type {TYPE OF V} ]";
}

} /* namespace detail */


struct message
{
   enum format : int { fancy, standard, old };

   /**
    * Generate fancy message
    **/
   template<class... Ts>
   static std::string __generate_fancy(const assertion<Ts...>& asrt)
   {
      std::string message;
      message.append("\e[1m\e[33m");
      message.append(asrt._info._file);
      message.append(":");
      message.append(std::to_string(asrt._info._line));
      message.append("\e[39m\n happened: ");
      message.append(asrt._info._message);
      message.append("\n");
      return message;
   }
   
   /**
    * Generate standard message
    **/
   template<class... Ts>
   static std::string __generate_standard(const assertion<Ts...>& asrt)
   {
      return "";
   }
   
   /**
    * Generate old type message
    **/
   template<class... Ts>
   static std::string __generate_old(const assertion<Ts...>& asrt)
   {
      std::string message;
      //std::stringstream s;
      //s << std::boolalpha << std::scientific << std::setprecision(16); 
      //s << " expected ";
      //m_pdata->expected(s);
      //s << " got ";
      //m_pdata->got(s);
      //detail::output_float_distance
      //   <  std::is_floating_point<typename std::decay<T>::type>::value
      //   || detail::is_complex<typename std::decay<T>::type>::value
      //   >::apply(s, a_got, a_expected);
      message.append(" in file ");
      message.append(asrt._info._file);
      message.append(" on line ");
      message.append(std::to_string(asrt._info._line));
      message.append("\n happened: ");
      message.append(asrt._info._message);
      message.append("\n");
      //message.append(s.str());

      return message;
   }

   /**
    * Generate default message (Mostly to silence compiler warning...)
    **/
   static std::string __default()
   {
      return "";
   }
      
   /**
    * Generate message dispatcher
    **/
   template<class... Ts>
   static std::string generate(const assertion<Ts...>& asrt, const format& form)
   {
      switch(form)
      {
         case fancy:
            return __generate_fancy(asrt);
         case standard:
            return __generate_standard(asrt);
         case old:
            return __generate_old(asrt);
      };

      return __default();
   }

};

} /* namespace cutee */

#endif /* CUTEE_MESSAGE_H_INCLUDED */
