#pragma once
#ifndef CUTEE_MESSAGE_H_INCLUDED
#define CUTEE_MESSAGE_H_INCLUDED

#include<string>
#include<iostream>
#include<sstream>
#include<type_traits>
#include<complex>
#include<iomanip>

#include "meta.hpp"
#include "osutil.hpp"
#include "formater.hpp"
#include "assertion.hpp"
#include "float_eq.hpp"

namespace cutee
{

namespace detail
{

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

template
   <  class T
   ,  class U
   //,  class Enable = typename type_sink<T, U>::type
   ,  class Enable = void
   >
struct has_distance
   :  public std::false_type
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
      <  std::is_same_v
         <  std::decay_t<T>
         ,  std::decay_t<U> 
         > 
      && std::is_floating_point_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs, const assertion_type& type)
   {
      if(type != assertion_type::comp_zero)
      {
         return numeric::float_ulps(lhs, rhs);
      }
      else
      {
         return numeric::float_ulps(lhs, lhs + rhs);
      }
   }
   
   static constexpr auto precision = std::numeric_limits<T>::max_digits10;
};

template
   <  class T
   ,  class U
   >
struct has_distance
   <  T
   ,  U
   ,  std::enable_if_t
      <  std::is_same_v
         <  std::decay_t<T>
         ,  std::decay_t<U> 
         > 
      && is_complex_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs, const assertion_type& type)
   {
      using base_type = decltype(numeric::float_ulps(lhs.real(), rhs.real()));
      if(type != assertion_type::comp_zero)
      {
         return std::complex<base_type>
            (  numeric::float_ulps(lhs.real(), rhs.real())
            ,  numeric::float_ulps(lhs.imag(), rhs.imag())
            );
      }
      else
      {
         return std::complex<base_type>
            (  numeric::float_ulps(lhs.real(), lhs.real() + rhs.real())
            ,  numeric::float_ulps(lhs.imag(), lhs.imag() + rhs.imag())
            );
      }
   }
   
   static constexpr auto precision = std::numeric_limits<typename std::decay_t<T>::value_type>::max_digits10;
};

template
   <  class T
   ,  class U
   >
struct has_distance
   <  T
   ,  U
   ,  std::enable_if_t
      <  std::is_same_v
         <  std::decay_t<T>
         ,  std::decay_t<U> 
         > 
      && is_vector_v<std::decay_t<T> > 
      > 
   >
   :  public std::true_type
{
   constexpr static auto calculate_distance(const T& lhs, const U& rhs, const assertion_type& type)
   {
      using base_type = decltype(numeric::float_ulps(lhs[0], rhs[0]));
      std::vector<base_type> ulps;
      auto ulps_size = std::min(lhs.size(), rhs.size());
      ulps.resize(ulps_size);
      for(int i = 0; i < int(ulps_size); ++i)
      {
         if(type != assertion_type::comp_zero)
         {
            ulps[i] = numeric::float_ulps(lhs[i], rhs[i]);
         }
         else
         {
            ulps[i] = numeric::float_ulps(lhs[i], lhs[i] + rhs[i]);
         }
      }
      return ulps;
   }

   static constexpr auto precision = std::numeric_limits<typename std::decay_t<T>::value_type>::max_digits10;
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
   s_str << std::left << std::scientific << std::boolalpha;
   if constexpr(has_distance<V, V>::value)
   {
      s_str << std::setprecision(has_distance<V, V>::precision);
   }
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
   static const int short_width = 13;
   
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
   static std::string __generate_message(const assertion<Ts...>& asrt)
   {
      std::string tab = "   ";
      std::stringstream s_str;
      s_str << std::left << std::setprecision(16) << std::scientific << std::boolalpha; 
      s_str << tab << std::setw(short_width) << "error" << "[/warning_color]" << asrt._info._message << "[/default_color]"
            << "\n"
            << tab << std::setw(short_width) << "file"  
            << "[/file_color]";
      if(!asrt._info._file.empty())
      {
         s_str << asrt._info._file << ":" << asrt._info._line;
      }
      else
      {
         s_str << "N/A";
      }
      s_str << "[/default_color]"
            << "\n\n";
      

      std::vector<variable_triad> variable_vec;

      if constexpr(sizeof...(Ts) == 1)
      {
         variable_vec.emplace_back
            (  std::string{"value"} + std::string{(asrt._info._type == assertion_type::not_equal ? " not" : "")}
            ,  detail::value_string(std::get<0>(asrt._args))
            ,  detail::type_string (std::get<0>(asrt._args))
            );
      }

      if constexpr(sizeof...(Ts) >= 2)
      {
         variable_vec.emplace_back
            (  (  asrt._info._type != assertion_type::comp_zero 
               ?  std::string{"expected"} + std::string{(asrt._info._type == assertion_type::not_equal ? " not" : "")}
               :  std::string{"compare"}
               )
            ,  detail::value_string(std::get<1>(asrt._args))
            ,  detail::type_string (std::get<1>(asrt._args))
            );
         variable_vec.emplace_back
            (  (  asrt._info._type != assertion_type::comp_zero
               ?  std::string{"got"}
               :  std::string{"zero"}
               )
            ,  detail::value_string(std::get<0>(asrt._args))
            ,  detail::type_string (std::get<0>(asrt._args))
            );
      }

      if constexpr(sizeof...(Ts) >= 3)
      {
         using distance = detail::has_distance<decltype(std::get<1>(asrt._args)), decltype(std::get<0>(asrt._args))>;
         if constexpr (distance::value)
         {
            auto dist = distance::calculate_distance(std::get<1>(asrt._args), std::get<0>(asrt._args), asrt._info._type);
            
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
                        << "[/value_color]"        << std::setw(width)        << v._value << "[/default_color]"
                        << " "                     << "[" << "[/type_color]"  << v._type  << "[/default_color]" << "]\n";
      }
      
      return s_str.str();
   }
      
   /**
    * Generate message dispatcher
    **/
   template<class... Ts>
   static std::string generate(const assertion<Ts...>& asrt)
   {
      return __generate_message(asrt);
   }

};

} /* namespace cutee */

#endif /* CUTEE_MESSAGE_H_INCLUDED */
