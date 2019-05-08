#ifndef CUTEE_TEST_FAILED_H_INCLUDED
#define CUTEE_TEST_FAILED_H_INCLUDED

#include<string>
#include<iostream>
#include<sstream>
#include<type_traits>
#include<iomanip>

#include "failed_data.hpp"
#include "float_eq.hpp"

namespace cutee
{
namespace detail
{

template<class T>
struct is_complex
   :  public std::false_type
{
   static_assert(std::is_floating_point<T>::value, "T must be floating-point type.");
};
template<class T>
struct is_complex<std::complex<T>>
   :  public std::true_type
{
   static_assert(std::is_floating_point<T>::value, "T must be floating-point type.");
};


template<bool>
struct output_float_distance
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
      static_assert(std::is_same<T, U>::value, "not the same types.");
   }
};

} /* namespace detail */
                              

//
//
//
struct test_failed
{

   public:
      template<typename T, typename U>
      test_failed(const std::string a_name,
                  const char* a_file, 
                  const unsigned int a_line, 
                  const std::string a_what,
                  const T& a_expected,
                  const U& a_got): 
         m_name(a_name), 
         m_file(a_file), m_line(a_line), m_what(a_what), m_message(),
         m_pdata(failed_data_factory(a_expected,a_got))
      { 
         // make this prettier ?? YES !!..
         m_message.append(m_name);
         m_message.append(": ");
         std::stringstream s;
         s << std::boolalpha << std::scientific << std::setprecision(16); 
         s << " expected ";
         m_pdata->expected(s);
         s << " got ";
         m_pdata->got(s);
         detail::output_float_distance
            <  std::is_floating_point<typename std::decay<T>::type>::value
            || detail::is_complex<typename std::decay<T>::type>::value
            >::apply(s, a_got, a_expected);
         m_message.append(" in file ");
         m_message.append(m_file);
         m_message.append(" on line ");
         m_message.append(std::to_string(m_line));
         m_message.append("\n happened: ");
         m_message.append(m_what);
         m_message.append("\n");
         m_message.append(s.str());
      };

      //
      //
      //
      virtual ~test_failed() 
      { 
         failed_data_destroy(m_pdata); 
      }
      
      //
      //
      //
      const char* what() const throw()
      {
         return m_message.c_str();
      }
   
   private:
      const std::string  m_name; // ADD ME !
      const std::string  m_file;
      const unsigned int m_line;
      const std::string  m_what;
      std::string        m_message;
      failed_data_base*  m_pdata;
};

} /* namespace cutee */

#endif /* CUTEE_TEST_FAILED_H_INCLUDED */
