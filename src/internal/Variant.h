#ifndef __JSON_UTILS_VARIANT_H__
#define __JSON_UTILS_VARIANT_H__
#include <bits/stdc++.h>
#include "TypeTraits.h"
#include "Visitor.h"
namespace vsjson {
namespace detail {

// used for Variant::visit()
template <int Lo, int Hi, typename ...Ts>
struct RuntimeChoose {
    template <typename Visitor, typename R = typename std::decay_t<Visitor>::ReturnType>
    static R apply(int n, void *data, Visitor &&visitor);
};

template <typename ...Ts>
class Variant {
public:
    Variant();
    template <typename T,
              typename = RequireNot<std::is_same<std::decay_t<T>, Variant>>>
    Variant(T &&obj);
    Variant(const Variant &rhs);
    Variant(Variant &&rhs);

    template <typename T,
              typename = RequireNot<std::is_same<std::decay_t<T>, Variant>>>
    Variant& operator=(T &&rhs);
    Variant& operator=(const Variant &rhs);
    Variant& operator=(Variant &&rhs);

    ~Variant();

    template <typename T> T& get();
    template <typename T> const T& get() const;

    template <typename T> bool is() const;

    template <typename T> T to() &;
    template <typename T> T to() const &;
    template <typename T> T to() &&;

    template <typename Visitor,
              typename R = typename std::decay_t<Visitor>::ReturnType>
    R visit(Visitor &&visitor);

    void swap(Variant &rhs);

    template<typename T>
    void init(T &&obj);

    int what() const { return _what; }
    const char* handle() { return _handle; }

private:
    int _what;
    char _handle[MaxSize<Ts...>::size];
};

template <int Lo, int Hi, typename ...Ts>
template <typename Visitor, typename R>
inline R RuntimeChoose<Lo, Hi, Ts...>::apply(int n, void *data, Visitor &&visitor) {
    constexpr int Mid = Lo + (Hi - Lo >> 1);
    if(Mid == n) {
        using Type = typename TypeAt<Mid, Ts...>::Type;
        return std::forward<Visitor>(visitor)(*reinterpret_cast<Type*>(data));
    } else if(Mid > n) {
        return RuntimeChoose<Lo, std::max(Mid - 1, Lo), Ts...>::apply(n, data, std::forward<Visitor>(visitor));
    } else {
        return RuntimeChoose<std::min(Mid + 1, Hi), Hi, Ts...>::apply(n, data, std::forward<Visitor>(visitor));
    }
}

template <typename ...Ts>
inline Variant<Ts...>::Variant() {
    _what = -1;
    memset(_handle, 0, sizeof _handle);
}

template <typename ...Ts>
template <typename T, typename>
inline Variant<Ts...>::Variant(T &&obj) {
    static_assert(Position<std::decay_t<T>, Ts...>::pos != -1,
        "type not found");
    init(std::forward<T>(obj));
}

template <typename ...Ts>
inline Variant<Ts...>::Variant(const Variant &rhs) {
    visitor::CopyConstructVisitor<Ts...> ccv(*this);
    const_cast<Variant&>(rhs).visit(ccv);
}

template <typename ...Ts>
inline Variant<Ts...>::Variant(Variant &&rhs) {
    visitor::MoveConstructVisitor<Ts...> mcv(*this);
    rhs.visit(mcv);
}

template <typename ...Ts>
template <typename T,typename>
inline Variant<Ts...>& Variant<Ts...>::operator=(T &&rhs) {
    this->~Variant();
    init(std::forward<T>(rhs));
    return *this;
}

template <typename ...Ts>
inline Variant<Ts...>& Variant<Ts...>::operator=(const Variant &rhs){
    if(this == &rhs) return *this;
    this->~Variant();
    visitor::CopyConstructVisitor<Ts...> ccv(*this);
    const_cast<Variant&>(rhs).visit(ccv);
    return *this;
}

template <typename ...Ts>
inline Variant<Ts...>& Variant<Ts...>::operator=(Variant &&rhs) {
    if(this == &rhs) return *this;
    this->~Variant();
    visitor::MoveConstructVisitor<Ts...> mcv(*this);
    rhs.visit(mcv);
    return *this;
}

template <typename ...Ts>
inline Variant<Ts...>::~Variant() {
    if(_what == -1) return;
    visitor::DeleteVisitor<Ts...> dv(*this);
    visit(dv);
}

template <typename ...Ts>
template <typename T>
inline T& Variant<Ts...>::get() {
    static_assert(Position<T, Ts...>::pos != -1,
        "type not found");
    if(_what == Position<T, Ts...>::pos) {
        return *reinterpret_cast<T*>(_handle);
    } else {
        throw std::runtime_error("cannot cast");
    }
}

template <typename ...Ts>
template <typename T>
inline const T& Variant<Ts...>::get() const {
    return const_cast<Variant*>(this)->get<T>();
}

template <typename ...Ts>
template <typename T>
inline bool Variant<Ts...>::is() const {
    return Position<T, Ts...>::pos == _what;
}

template <typename ...Ts>
template <typename T>
inline T Variant<Ts...>::to() & {
    visitor::ConvertVisitor<T> cv;
    return visit(cv);
}

template <typename ...Ts>
template <typename T>
inline T Variant<Ts...>::to() const & {
    return const_cast<Variant*>(this)->to<T>();
}

template <typename ...Ts>
template <typename T>
inline T Variant<Ts...>::to() && {
    visitor::MovedConvertVisitor<T> cv;
    return visit(cv);
}

template <typename ...Ts>
template <typename Visitor, typename R>
inline R Variant<Ts...>::visit(Visitor &&visitor) {
    return RuntimeChoose<0, sizeof...(Ts)-1, Ts...>
        ::apply(_what, _handle, std::forward<Visitor>(visitor));
}

// DO NOT IMPLEMENT operator= BY SWAP(swap for placement new buffer is wrong)
// reverse case : implements swap by operator=&&
template <typename ...Ts>
void Variant<Ts...>::swap(Variant &rhs) {
    std::swap(*this, rhs);
}

template <typename ...Ts>
template<typename T>
inline void Variant<Ts...>::init(T &&obj) {
    using DecayT = std::decay_t<T>;
    _what = Position<DecayT, Ts...>::pos;
    new(_handle) DecayT(std::forward<T>(obj));
}

template <typename ...Ts>
inline std::ostream& operator<<(std::ostream &os, Variant<Ts...> &variant) {
    return variant.visit(visitor::OsVisitor{os});
}

} // detail
} // vsjson
#endif