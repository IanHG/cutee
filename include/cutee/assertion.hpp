#ifndef CUTEE_ASSERTION_HPP_INCLUDED
#define CUTEE_ASSERTION_HPP_INCLUDED

#include <string>
#include <functional>
#include <tuple>

namespace cutee
{

struct info
{
   std::string _message;
   std::string _file;
   int         _line;
};

template<class... Ts> 
struct assertion
{
   std::function<bool(Ts...)> _assert_function;
   std::tuple<Ts...>          _args;
   info                       _info;

   
   template<size_t... I>
   auto __execute_internal(std::index_sequence<I...>) const
   {
      return _assert_function(std::get<I>(_args)...);
   }

   auto execute() const 
   {
      return __execute_internal(std::index_sequence_for<Ts...>{});  
   }

   constexpr auto size() const
   {
      return std::tuple_size<std::tuple<Ts...> >::value;
   }
};

} /* namespace cutee */

#endif /* CUTEE_ASSERTION_HPP_INCLUDED */
