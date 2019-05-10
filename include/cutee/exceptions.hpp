#ifndef CUTEE_EXCEPTIONS_HPP_INCLUDED
#define CUTEE_EXCEPTIONS_HPP_INCLUDED

#include <string>

#include "assertion.hpp"

namespace cutee
{
namespace exception
{

struct failed
{
   virtual ~failed() noexcept = 0;
   virtual const char* what() const noexcept = 0;
};

failed::~failed() { }

struct assertion_failed
   :  public failed
{
   std::string _what;
   
   template<class... Ts>
   static std::string __construct_what(const assertion<Ts...>& asrt)
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
   
   template<class... Ts>
   assertion_failed(const assertion<Ts...>& asrt)
      :  _what(__construct_what(asrt))
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
