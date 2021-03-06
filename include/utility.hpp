#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace utility {

using index_t = long long signed;

inline namespace detail {

template<index_t Index, typename... Ts>
struct ct_select;

template<typename T, typename... Ts>
struct ct_select<0, T, Ts...>
{
  typedef T type;
};

template<index_t Index, typename T, typename... Ts>
struct ct_select<Index, T, Ts...>
{
  static_assert(Index < sizeof...(Ts) + 1, "parameter pack index out of bounds");
  typedef typename ct_select<Index - 1, Ts...>::type type;
};
}

template<index_t... Indices>
struct index_range
{
  constexpr auto size() { return std::integral_constant<index_t, sizeof...(Indices)>{}; }
};

template<index_t Begin, index_t End, index_t Position = Begin, index_t... Result>
constexpr auto
make_index_range()
{
  if constexpr (Begin < End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position + 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else if constexpr (Begin > End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position - 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else
    return index_range<Position>{};
}

template<typename Find, typename T0, typename... Ts, index_t Index>
constexpr auto
index_of(const std::integral_constant<index_t, Index> = std::integral_constant<index_t, 0>{})
{
  if constexpr (std::is_same_v<Find, T0>)
    return Index;
  else
    return index_of<Find, Ts...>(std::integral_constant<index_t, Index + 1>{});
}

template<index_t AtIndex, typename... TypeList>
using select_t = typename ct_select<AtIndex, TypeList...>::type;

template<auto N, auto E, index_t... Indices>
constexpr auto
cpow_impl(const index_range<Indices...>) -> double
{
  constexpr float abs_pow = (1.0 * ... * (Indices, N));
  if constexpr (E > 0)
    return abs_pow;
  if constexpr (E < 0)
    return 1.0 / abs_pow;
  else
    return 1.0;
}

template<auto N, auto E>
constexpr auto
cpow() -> double
{
  if constexpr (N == 0)
    return 0.0;
  if constexpr (E > 0)
    return cpow_impl<N, E>(make_index_range<0, E - 1>());
  else
    return cpow_impl<N, E>(make_index_range<0, E + 1>());
}

template<class... Ints>
constexpr index_t
combine_digits_base10(index_t result, index_t int0, Ints... ints)
{
  if constexpr (sizeof...(Ints) == 0)
    return result + int0;
  else
    return combine_digits_base10(result + (cpow<10, sizeof...(Ints)>() * int0), ints...);
}

constexpr index_t
parse_digit(char C)
{
  return (C >= '0' && C <= '9') ? C - '0' : throw std::out_of_range("only decimal digits are allowed");
}
}
