#ifndef __JSON_UTILS_VARIANT_H__
#define __JSON_UTILS_VARIANT_H__
#include <bits/stdc++.h>
#include "TypeTraits.h"


template <typename ...Types>
class Variant;

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

// lhs = rhs
template <typename ...Types>
struct CopyConstructVisitor {
    using ReturnType = void;
    Variant<Types...> &lhs;
    CopyConstructVisitor(Variant<Types...> &lhs): lhs(lhs) { }
    template <typename T>
    ReturnType operator()(const T &obj) {
        lhs.init(obj);
    }
};

template <typename ...Types>
struct MoveConstructVisitor {
    using ReturnType = void;
    Variant<Types...> &lhs;
    MoveConstructVisitor(Variant<Types...> &lhs): lhs(lhs) { }
    template <typename T>
    ReturnType operator()(T &obj) {
        lhs.init(std::move(obj));
    }
};

template <typename ...Types>
struct DeleteVisitor {
    using ReturnType = void;
    Variant<Types...> &self;
    DeleteVisitor(Variant<Types...> &self): self(self) {}
    template <typename T>
    ReturnType operator()(const T &) {
        ((T*)self.handle())->~T();
    }
};


struct OsVisitor {
    using ReturnType = std::ostream&;
    std::ostream &_os;
    OsVisitor(std::ostream &os): _os(os) {}
    
    template <typename T>
    std::enable_if_t<HasOperatorLeftShift<T>::value, 
    ReturnType> operator()(T &obj) { 
        _os << obj;
        return _os;
    }

    template <typename T>
    std::enable_if_t<!HasOperatorLeftShift<T>::value, 
    ReturnType> operator()(T &obj) { 
        throw std::runtime_error("[type]" + std::string(typeid(obj).name()) 
            + " does not support IO");
        return _os;
    }
};

template <typename U>
struct ConvertVisitor {
    using ReturnType = U;

    template <typename T>
    std::enable_if_t<std::is_convertible<T, U>::value,
    ReturnType> operator()(T &obj) {
        return obj;
    }

    template <typename T>
    std::enable_if_t<!std::is_convertible<T, U>::value,
    ReturnType> operator()(T &obj) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name()) 
            + " does not support convert to [type]" + std::string(typeid(U).name())); 
        return U{};
    }
};

template <typename U>
struct MovedConvertVisitor {
    using ReturnType = U;

    template <typename T>
    std::enable_if_t<std::is_convertible<T, U>::value,
    ReturnType> operator()(T &obj) {
        return std::move(obj);
    }

    template <typename T>
    std::enable_if_t<!std::is_convertible<T, U>::value,
    ReturnType> operator()(T &obj) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name()) 
            + " does not support moved convert to [type]" + std::string(typeid(U).name())); 
        return U{};
    }
};

template <typename ...Types>
class Variant {
private:
    int _what;
    char _handle[MaxSize<Types...>::size];
    
public:
    Variant() {
        _what = -1;
        memset(_handle, 0, sizeof _handle);
    }

    template <typename T,
    typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Variant>::value>>
    Variant(T &&obj) { 
        static_assert(Position<std::decay_t<T>, Types...>::pos != -1,
            "type not found");
        init(std::forward<T>(obj));
    }

    Variant(const Variant &rhs) {
        CopyConstructVisitor<Types...> ccv(*this);
        const_cast<Variant&>(rhs).visit(ccv);
    }

    Variant(Variant &&rhs) {
        MoveConstructVisitor<Types...> mcv(*this);
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
        CopyConstructVisitor<Types...> ccv(*this);
        const_cast<Variant&>(rhs).visit(ccv);
        return *this;
    }

    Variant& operator=(Variant &&rhs) {
        if(this == &rhs) return *this;
        this->~Variant();
        MoveConstructVisitor<Types...> mcv(*this);
        rhs.visit(mcv);
        return *this;
    }

    ~Variant() {
        if(_what == -1) return;
        DeleteVisitor<Types...> dv(*this);
        visit(dv);
    }

    template <typename T>
    T& get() {
        static_assert(Position<T, Types...>::pos != -1,
            "type not found");
        if(_what == Position<T, Types...>::pos) {
            return *reinterpret_cast<T*>(_handle);
        } else {
            throw std::runtime_error("cannot cast");
        }
    }

    template <typename T>
    const T& get() const {
        return get<T>();
    }

    template <typename T>
    bool is() const {
        return Position<T, Types...>::pos == _what;
    }

    template <typename T>
    T to() & {
        ConvertVisitor<T> cv;
        return visit(cv);
    }

    template <typename T>
    T to() && {
        MovedConvertVisitor<T> cv;
        return visit(cv);
    }

    template <typename Visitor>
    typename Visitor::ReturnType visit(Visitor &visitor) {
        return RuntimeChoose<0, sizeof...(Types)-1, Types...>
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
        _what = Position<DecayT, Types...>::pos;
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


#endif