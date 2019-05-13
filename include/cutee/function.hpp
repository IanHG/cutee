#ifndef CUTEE_FUNCTION_HPP_INCLUDED
#define CUTEE_FUNCTION_HPP_INCLUDED

#include <functional>
#include <type_traits>
#include <tuple>

#include "unit_test.hpp"

namespace cutee
{

template<class... Ts>
struct __false_on_ts
   :  public std::false_type
{
};

template<class... Ts>
constexpr auto __false_on_ts_v = __false_on_ts<Ts...>::value;

template<class... Ts>
struct __function_return
{
   static_assert(__false_on_ts_v<Ts...>, "Must provide a function");
};

template<class T>
struct __function_return<T>
   :  public __function_return<decltype(&T::operator())>
{
};

template<class R, class... Args>
struct __function_return<R(&)(Args...)>
{
   using type = R;
};

template<class R, class ClassType, class... Args>
struct __function_return<R(ClassType::*)(Args...) const>
{
   using type = R;
};

template<class R, class ClassType, class... Args>
struct __function_return<R(ClassType::*)(Args...)>
{
   using type = R;
};

template<class R, class... Args>
struct __function_return<std::function<R(Args...)> >
{
   using type = R;
};

template<class... Ts>
using __function_return_t = typename __function_return<Ts...>::type;

void unit_assert_fcn(bool, const std::string&, const char*, int);

//template<class F, class S, class... Args>
template<class S, class F, class... Args>
struct __function_wrap
   : public cutee::unit_test
{
   private:
   F                   _fcn;
   std::tuple<Args...> _args;
   
   public:
   //__function_wrap(F&& fcn, S&& msg, Args&&... args)
   __function_wrap(F&& fcn, Args&&... args)
      :  _fcn (std::forward<F>(fcn))
      ,  _args(std::forward_as_tuple(args...))
   {
   }
   
   private:
   template<std::size_t ...I>
   void call_func(std::index_sequence<I...>)
   { 
      if constexpr(std::is_same_v<__function_return_t<F>, bool>)
      {
         //unit_assert_fcn(_fcn(std::get<I>(_args)...), "Function failed!", __FILE__, __LINE__);
         unit_assert_fcn(_fcn(std::get<I>(_args)...), "Function failed!", "", 0);
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

   std::string name() const 
   { 
      return std::string{"__function_wrap"}; 
   }
};

} /* namespace cutee */

#endif /* CUTEE_FUNCTION_HPP_INCLUDED */
