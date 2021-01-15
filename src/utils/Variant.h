#ifndef __JSON_UTILS_VARIANT_H__
#define __JSON_UTILS_VARIANT_H__
#include <bits/stdc++.h>
#include "TypeTraits.h"
#include "Visitor.h"
namespace vsjson {

template <int Lo, int Hi, typename ...Ts>
struct RuntimeChoose {
    template <typename Visitor>
    static typename Visitor::ReturnType apply(int n, void *data, Visitor &visitor) {
        constexpr int Mid = Lo + (Hi - Lo >> 1);
        if(Mid == n) {
            using Type = typename TypeAt<Mid, Ts...>::Type;
            return visitor(*reinterpret_cast<Type*>(data));
        } else if(Mid > n) {
            return RuntimeChoose<Lo, std::max(Mid - 1, Lo), Ts...>::apply(n, data, visitor);
        } else {
            return RuntimeChoose<std::min(Mid + 1, Hi), Hi, Ts...>::apply(n, data, visitor);
        }
    }
};

template <typename ...Ts>
class Variant {
private:
    int _what;
    char _handle[MaxSize<Ts...>::size];
    
public:
    Variant() {
        _what = -1;
        memset(_handle, 0, sizeof _handle);
    }

    template <typename T,
    typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Variant>::value>>
    Variant(T &&obj) { 
        static_assert(Position<std::decay_t<T>, Ts...>::pos != -1,
            "type not found");
        init(std::forward<T>(obj));
    }

    Variant(const Variant &rhs) {
        CopyConstructVisitor<Ts...> ccv(*this);
        const_cast<Variant&>(rhs).visit(ccv);
    }

    Variant(Variant &&rhs) {
        MoveConstructVisitor<Ts...> mcv(*this);
        rhs.visit(mcv);
        // rhs._what = -1;
        // memset(rhs._handle, 0, sizeof(rhs._handle));
    }

    // DO NOT IMPLEMENT operator= BY SWAP(swap for placement new buffer is wrong)
    // reverse case : implements swap by operator=&&
    void swap(Variant &rhs) {
       std::swap(*this, rhs);
    }

    template <typename T,
    typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Variant>::value>>
    Variant& operator=(T &&rhs) {
        this->~Variant();
        init(std::forward<T>(rhs));
        return *this;
    }

    Variant& operator=(const Variant &rhs){
        if(this == &rhs) return *this;
        this->~Variant();
        CopyConstructVisitor<Ts...> ccv(*this);
        const_cast<Variant&>(rhs).visit(ccv);
        return *this;
    }

    Variant& operator=(Variant &&rhs) {
        if(this == &rhs) return *this;
        this->~Variant();
        MoveConstructVisitor<Ts...> mcv(*this);
        rhs.visit(mcv);
        return *this;
    }

    ~Variant() {
        if(_what == -1) return;
        DeleteVisitor<Ts...> dv(*this);
        visit(dv);
    }

    template <typename T>
    T& get() {
        static_assert(Position<T, Ts...>::pos != -1,
            "type not found");
        if(_what == Position<T, Ts...>::pos) {
            return *reinterpret_cast<T*>(_handle);
        } else {
            throw std::runtime_error("cannot cast");
        }
    }

    template <typename T>
    const T& get() const {
        return const_cast<Variant*>(this)->get<T>();
    }

    template <typename T>
    bool is() const {
        return Position<T, Ts...>::pos == _what;
    }

    template <typename T>
    T to() & {
        ConvertVisitor<T> cv;
        return visit(cv);
    }

    template <typename T>
    T to() const & {
        return const_cast<Variant*>(this)->to<T>();
    }

    template <typename T>
    T to() && {
        MovedConvertVisitor<T> cv;
        return visit(cv);
    }

    template <typename Visitor, typename R = typename Visitor::ReturnType>
    R visit(Visitor &visitor) {
        return RuntimeChoose<0, sizeof...(Ts)-1, Ts...>
            ::apply(_what, _handle, visitor);
    }

    friend std::ostream& operator<<(std::ostream &os, Variant &variant) {
        OsVisitor osv(os);
        return variant.visit(osv);
    }

    int what() const { return _what; }
    const char* handle() { return _handle; }

    template<typename T>
    void init(T &&obj) {
        using DecayT = std::decay_t<T>;
        _what = Position<DecayT, Ts...>::pos;
        new(_handle) DecayT(std::forward<T>(obj));
    }
};

// template <typename T>
// std::enable_if_t<HasOperatorLeftShift<T>::value, 
// std::ostream&> operator<<(std::ostream &os, std::vector<T> &vec) {
//     os << '[';
//     if(vec.size() > 0) {
//         auto last = --vec.end();
//         for(auto it = vec.begin(); it != last; ++it) os << *it << ", ";
//         os << *last;
//     }
//     os << ']';
//     return os;
// }

// template </*typename T, */typename U>
// std::enable_if_t</*HasOperatorLeftShift<T>::value && */HasOperatorLeftShift<U>::value, 
// std::ostream&> operator<<(std::ostream &os, std::map<std::string, U> &map) {
//     os << '{';
//     if(map.size() > 0) {
//         auto last = --map.end();
//         for(auto it = map.begin(); it != last; ++it) os << '\"' << it->first << "\": " << it->second << ", ";
//         os << '\"' << last->first << "\": " << last->second;
//     }
//     os << '}';
//     return os;
// }

} // vsjson
#endif