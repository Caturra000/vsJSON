#ifndef __JSON_UTILS_VISITOR_H__
#define __JSON_UTILS_VISITOR_H__
#include <bits/stdc++.h>
#include "TypeTraits.h"
#include "VisitorHelper.h"
namespace vsjson {

template <typename ...Types>
class Variant;

// lhs = rhs
template <typename ...Types>
struct CopyConstructVisitor: public NoReturn {
    Variant<Types...> &lhs;
    CopyConstructVisitor(Variant<Types...> &lhs): lhs(lhs) { }
    template <typename T>
    void operator()(const T &obj) {
        lhs.init(obj);
    }
};

template <typename ...Types>
struct MoveConstructVisitor: public NoReturn {
    Variant<Types...> &lhs;
    MoveConstructVisitor(Variant<Types...> &lhs): lhs(lhs) { }
    template <typename T>
    void operator()(T &obj) {
        lhs.init(std::move(obj));
    }
};

template <typename ...Types>
struct DeleteVisitor: public NoReturn {
    Variant<Types...> &self;
    DeleteVisitor(Variant<Types...> &self): self(self) {}
    template <typename T>
    void operator()(const T &) {
        ((T*)self.handle())->~T();
    }
};


struct OsVisitor: public Return<std::ostream&> {
    std::ostream &_os;
    OsVisitor(std::ostream &os): _os(os) {}
    
    template <typename T>
    std::enable_if_t<HasOperatorLeftShift<T>::value, 
    std::ostream&> operator()(T &obj) { 
        _os << obj;
        return _os;
    }

    template <typename T>
    std::enable_if_t<!HasOperatorLeftShift<T>::value, 
    std::ostream&> operator()(T &obj) { 
        throw std::runtime_error("[type]" + std::string(typeid(obj).name()) 
            + " does not support IO");
        return _os;
    }
};

template <typename U>
struct ConvertVisitor: public Return<U> {
    template <typename T>
    std::enable_if_t<std::is_convertible<T, U>::value,
    U> operator()(T &obj) { return obj; }
    template <typename T>
    std::enable_if_t<!std::is_convertible<T, U>::value,
    U> operator()(T &obj) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name())
            + " does not support convert to [type]" + std::string(typeid(U).name()));
        return U{};
    }
};

template <typename U>
struct MovedConvertVisitor: public Return<U> {
    template <typename T>
    std::enable_if_t<std::is_convertible<T, U>::value,
    U> operator()(T &obj) { return std::move(obj); }
    template <typename T>
    std::enable_if_t<!std::is_convertible<T, U>::value,
    U> operator()(T &obj) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name())
            + " does not support moved convert to [type]" + std::string(typeid(U).name()));
        return U{};
    }
};

} // vsjson
#endif