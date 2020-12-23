#ifndef __JSON_UTILS_TYPE_TRAITS_H__
#define __JSON_UTILS_TYPE_TRAITS_H__
#include <bits/stdc++.h>
template <typename T, typename ...Ts>
struct Position {
    constexpr static int pos = -1;
};

template <typename T, typename U, typename ...Ts>
struct Position<T, U, Ts...> {
    constexpr static int pos = std::is_same<T, U>::value ? 0 
        : (Position<T, Ts...>::pos == -1 ? -1 : Position<T, Ts...>::pos + 1);
};


template <typename T, typename ...Ts>
struct MaxSize {
    constexpr static size_t size = std::max(sizeof(T), MaxSize<Ts...>::size);
};
template <typename T>
struct MaxSize<T> {
    constexpr static size_t size = sizeof(T);
};



template <size_t Pos, typename T, typename ...Ts>
struct TypeAt: public std::conditional_t<Pos == 0, 
    TypeAt<0, T>, TypeAt<Pos-1, Ts...>> {};

template <typename T>
struct TypeAt<0, T> {
    using Type = T;
};




struct YesOrNo {
    using Yes = char[1];
    using No = char[2];
};

template<typename T>
struct HasOperatorLeftShift : YesOrNo {

    template<typename U> static Yes& test(
        size_t (*n)[ sizeof( std::cout << * static_cast<U*>(0) ) ] );

    template<typename U> static No& test(...);

    static bool const value = sizeof( test<T>( nullptr ) ) == sizeof( Yes );
};
#endif