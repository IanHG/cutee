#ifndef CUTEE_MESSAGE_H_INCLUDED
#define CUTEE_MESSAGE_H_INCLUDED

#include<string>
#include<iostream>
#include<sstream>
#include<type_traits>
#include<complex>
#include<iomanip>

#include "osutil.hpp"
#include "formater.hpp"
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

/**
 * Get demangled type as string
 **/
template<class T>
std::string type_of()
{
   return demangle(typeid(T).name());
}

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

template<class T>
constexpr auto is_complex_v = is_complex<T>::value;

template<class T>
struct is_vector
   :  public std::false_type
{
};

template<class T, class U>
struct is_vector<std::vector<T, U> >
   :  public std::true_type
{
   static_assert(std::is_floating_point<T>::value, "T must be floating-point type.");
};

template<class T>
constexpr auto is_vector_v = is_vector<T>::value;

template<class... Ts>
struct type_sink
{
   using type = void;
};

template
   <  class T
   ,  class U
   ,  class Enable = typename type_sink<T, U>::type
   >
struct has_distance
{
};

template
   <  class T
   ,  class U
   >
struct has_distance
   <  T
   ,  U
   ,  std::enable_if_t
      <  std::is_same_v<std::decay_t<T>, std::decay_t<U> > 
      && std::is_floating_point_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs)
   {
      return numeric::float_ulps(lhs, rhs);
   }
};

template
   <  class T
   ,  class U
   >
struct has_distance
   <  T
   ,  U
   ,  std::enable_if_t
      <  std::is_same_v<std::decay_t<T>, std::decay_t<U> > 
      && is_complex_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs)
   {
      using base_type = decltype(numeric::float_ulps(lhs.real(), rhs.real()));
      return std::complex<base_type>(numeric::float_ulps(lhs.real(), rhs.real()), numeric::float_ulps(lhs.imag(), rhs.imag()));
   }
};

template
   <  class T
   ,  class U
   >
struct has_distance
   <  T
   ,  U
   ,  std::enable_if_t
      <  std::is_same_v<std::decay_t<T>, std::decay_t<U> > 
      && is_vector_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs)
   {
      using base_type = decltype(numeric::float_ulps(lhs[0], rhs[0]));
      std::vector<base_type> ulps;
      auto ulps_size = std::min(lhs.size(), rhs.size());
      ulps.resize(ulps_size);
      for(int i = 0; i < int(ulps_size); ++i)
      {
         ulps[i] = numeric::float_ulps(lhs[i], rhs[i]);
      }
      return ulps;
   }
};

/**
 * Print got
 **/
template
   <  class V 
   ,  std::enable_if_t<detail::exists_operator_output<V>::value, void*> = nullptr
   >
std::string value_string(const V& v)
{
   std::stringstream s_str;
   s_str << std::setprecision(10) << std::scientific << std::boolalpha;
   s_str << v;
   return s_str.str();
}

template
   <  class V
   ,  std::enable_if_t<!detail::exists_operator_output<V>::value, void*> = nullptr
   >
std::string value_string(const V& v)
{ 
   std::stringstream s_str;
   s_str << "{ No output operator }"; 
   return s_str.str();
}

template<class V>
std::string type_string(const V& v)
{
   return type_of<V>();
}

} /* namespace detail */


struct message
{
   enum format : int { fancy, raw };
   static const int short_width = 10;
   
   struct variable_triad
   {
      variable_triad(std::string&& label, std::string&& value, std::string&& type)
         :  _label(std::move(label))
         ,  _value(std::move(value))
         ,  _type (std::move(type))
      {
      }

      std::string _label;
      std::string _value;
      std::string _type ;
   };

   /**
    * Generate fancy message
    **/
   template<class... Ts>
   static std::string __generate_message(const assertion<Ts...>& asrt, const formater& form)
   {
      std::string tab = "   ";
      std::stringstream s_str;
      s_str << std::left << std::setprecision(16) << std::scientific << std::boolalpha; 
      s_str << tab << std::setw(short_width) << "error" << form.warning_color() << asrt._info._message << form.default_color()
            << "\n"
            << tab << std::setw(short_width) << "file"  << form.file_color() << asrt._info._file << ":" << asrt._info._line  << form.default_color()
            << "\n\n";
      

      std::vector<variable_triad> variable_vec;

      if constexpr(sizeof...(Ts) == 1)
      {
         variable_vec.emplace_back
            (  std::string{"value"}
            ,  detail::value_string(std::get<0>(asrt._args))
            ,  detail::type_string (std::get<0>(asrt._args))
            );
      }

      if constexpr(sizeof...(Ts) >= 2)
      {
         variable_vec.emplace_back
            (  std::string{"expected"}
            ,  detail::value_string(std::get<1>(asrt._args))
            ,  detail::type_string (std::get<1>(asrt._args))
            );
         variable_vec.emplace_back
            (  std::string{"got"}
            ,  detail::value_string(std::get<0>(asrt._args))
            ,  detail::type_string (std::get<0>(asrt._args))
            );
      }

      if constexpr(sizeof...(Ts) >= 3)
      {
         using distance = detail::has_distance<decltype(std::get<1>(asrt._args)), decltype(std::get<0>(asrt._args))>;
         if constexpr (distance::value)
         {
            auto dist = distance::calculate_distance(std::get<1>(asrt._args), std::get<0>(asrt._args));
            
            variable_vec.emplace_back
               (  std::string{"precision"}
               ,  detail::value_string(std::get<2>(asrt._args))
               ,  detail::type_string(std::get<2>(asrt._args))
               );

            variable_vec.emplace_back
               (  std::string{"distance"}
               ,  detail::value_string(dist)
               ,  detail::type_string(dist)
               );
         }
      }

      std::size_t width = 0;
      for(const auto& v : variable_vec)
      {
         width = (width > v._value.size()) ? width : v._value.size();
      }

      for(const auto& v : variable_vec)
      {
         s_str << tab   << std::setw(short_width)  << v._label 
                        << std::setw(width)        << v._value 
                        << " "                     << "[" << form.type_color() << v._type << form.default_color() << "]\n";
      }
      
      return s_str.str();
   }

   ///**
   // * Generate default message (Mostly to silence compiler warning...)
   // **/
   //static std::string __default()
   //{
   //   return "";
   //}
      
   /**
    * Generate message dispatcher
    **/
   template<class... Ts>
   static std::string generate(const assertion<Ts...>& asrt, const formater& form)
   {
      return __generate_message(asrt, form);
   }

};

} /* namespace cutee */

#endif /* CUTEE_MESSAGE_H_INCLUDED */
