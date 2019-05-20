#pragma once
#ifndef CUTEE_META_HPP_INCLUDED
#define CUTEE_META_HPP_INCLUDED

#include <type_traits>

namespace cutee
{

template<class... Ts>
struct false_on_ts
   :  public std::false_type
{
};

template<class... Ts>
constexpr auto false_on_ts_v = false_on_ts<Ts...>::value;

} /* namespace cutee */

#endif /* CUTEE_META_HPP_INCLUDED */
