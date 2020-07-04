#pragma once
#ifndef CUTEE_FUNCTION_HPP_INCLUDED
#define CUTEE_FUNCTION_HPP_INCLUDED

#include <functional>
#include <type_traits>
#include <tuple>

#include "meta.hpp"

namespace cutee
{

template<class... Ts>
struct function_return
{
   static_assert(false_on_ts_v<Ts...>, "Must provide a function");
};

template<class T>
struct function_return<T>
   :  public function_return<decltype(&T::operator())>
{
};

template<class R, class... Args>
struct function_return<R(&)(Args...)>
{
   using type = R;
};

template<class R, class ClassType, class... Args>
struct function_return<R(ClassType::*)(Args...) const>
{
   using type = R;
};

template<class R, class ClassType, class... Args>
struct function_return<R(ClassType::*)(Args...)>
{
   using type = R;
};

template<class R, class... Args>
struct function_return<std::function<R(Args...)> >
{
   using type = R;
};

template<class... Ts>
using function_return_t = typename function_return<Ts...>::type;

//
void unit_assert_fcn(bool, const std::string&, const char*, int);

template<class F, class... Args>
struct function_wrap
{
   private:
   F                   _fcn;
   std::tuple<Args...> _args;
   
   public:
   function_wrap(F&& fcn, Args&&... args)
      :  _fcn (std::forward<F>(fcn))
      ,  _args(std::forward_as_tuple(args...))
   {
   }
   
   private:
   template<std::size_t ...I>
   void call_func(std::index_sequence<I...>)
   { 
      if constexpr(std::is_same_v<function_return_t<F>, bool>)
      {
         if constexpr(false)
         {  
            unit_assert_fcn(_fcn(std::get<I>(_args)...), "Function failed!", __FILE__, __LINE__);
         }
         else
         {
            unit_assert_fcn(_fcn(std::get<I>(_args)...), "Function failed!", "", 0);
         }
      }
      else
      {
         _fcn(std::get<I>(_args)...);
      }
   }
   
   public:
   void run() 
   {
      call_func(std::index_sequence_for<Args...>{});
   }
};

} /* namespace cutee */

#endif /* CUTEE_FUNCTION_HPP_INCLUDED */
