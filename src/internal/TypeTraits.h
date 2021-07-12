#ifndef __JSON_UTILS_TYPE_TRAITS_H__
#define __JSON_UTILS_TYPE_TRAITS_H__
#include <bits/stdc++.h>
namespace vsjson {
namespace detail {

// position

template <typename T, typename ...Ts>
struct Position {
    constexpr static int pos = -1;
};

template <typename T, typename U, typename ...Ts>
struct Position<T, U, Ts...> {
    constexpr static int pos = std::is_same<T, U>::value ? 0
        : (Position<T, Ts...>::pos == -1 ? -1 : Position<T, Ts...>::pos + 1);
};

// max size

template <typename T, typename ...Ts>
struct MaxSize {
    constexpr static size_t size = std::max(sizeof(T), MaxSize<Ts...>::size);
};

template <typename T>
struct MaxSize<T> {
    constexpr static size_t size = sizeof(T);
};

// type at

template <size_t Pos, typename T, typename ...Ts>
struct TypeAt: public std::conditional_t<Pos == 0,
    TypeAt<0, T>, TypeAt<Pos-1, Ts...>> {};

template <typename T>
struct TypeAt<0, T> {
    using Type = T;
};

// stream support

template<typename T>
struct StreamSupport {
    using Success = char[1];
    using Fail = char[2];

    template<typename U> static Success& test(size_t (*n)[sizeof(std::cout << *static_cast<U*>(0))]);
    template<typename U> static Fail& test(...);

    static bool const value = sizeof( test<T>( nullptr ) ) == sizeof( Success );
};

// logical and

template <bool ...>
struct And;

template <bool ...bs>
struct And<true, bs...>: public std::integral_constant<bool, And<bs...>::value> {};

template <bool ...bs>
struct And<false, bs...>: public std::integral_constant<bool, false> {};

template <>
struct And<true>: public std::integral_constant<bool, true> {};

template <>
struct And<false>: public std::integral_constant<bool, false> {};

// require
// make typename std::enable_if<blabla::value, T>::type shorter

template <bool ...Conds>
using RequireBool = std::enable_if_t<And<Conds...>::value>;

template <typename ...Conds>
using Require = RequireBool<Conds::value...>;

template <typename ...Conds>
using RequireNot = RequireBool<(!Conds::value)...>;

} // detail
} // vsjson
#endif