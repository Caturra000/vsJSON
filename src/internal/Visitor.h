#ifndef __JSON_UTILS_VISITOR_H__
#define __JSON_UTILS_VISITOR_H__
#include <bits/stdc++.h>
#include "TypeTraits.h"
#include "VisitorHelper.h"
namespace vsjson {
namespace detail {

template <typename ...Ts>
class Variant;

namespace visitor {

// lhs = rhs
template <typename ...Ts>
struct CopyConstructVisitor: public NoReturn {
    Variant<Ts...> &lhs;
    CopyConstructVisitor(Variant<Ts...> &lhs): lhs(lhs) { }
    template <typename T>
    void operator()(const T &obj) {
        lhs.init(obj);
    }
};

template <typename ...Ts>
struct MoveConstructVisitor: public NoReturn {
    Variant<Ts...> &lhs;
    MoveConstructVisitor(Variant<Ts...> &lhs): lhs(lhs) { }
    template <typename T>
    void operator()(T &obj) {
        lhs.init(std::move(obj));
    }
};

template <typename ...Ts>
struct DeleteVisitor: public NoReturn {
    Variant<Ts...> &self;
    DeleteVisitor(Variant<Ts...> &self): self(self) {}
    template <typename T>
    void operator()(const T &) {
        ((T*)self.handle())->~T();
    }
};


struct OsVisitor: public Return<std::ostream&> {
    std::ostream &_os;
    OsVisitor(std::ostream &os): _os(os) {}

    template <typename T, typename = Require<StreamSupport<T>>>
    std::ostream& operator()(T &obj) {
        _os << obj;
        return _os;
    }

    template <typename T, typename = RequireNot<StreamSupport<T>>>
    std::ostream& operator()(T &obj, ...) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name())
            + " does not support IO");
        return _os;
    }
};

template <typename U>
struct ConvertVisitor: public Return<U> {
    template <typename T, typename = Require<std::is_convertible<T, U>>>
    U operator()(T &obj) { return obj; }
    template <typename T, typename = RequireNot<std::is_convertible<T, U>>>
    U operator()(T &obj, ...) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name())
            + " does not support convert to [type]" + std::string(typeid(U).name()));
        return U{};
    }
};

template <typename U>
struct MovedConvertVisitor: public Return<U> {
    template <typename T, typename = Require<std::is_convertible<T, U>>>
    U operator()(T &obj) { return std::move(obj); }
    template <typename T, typename = RequireNot<std::is_convertible<T, U>>>
    U operator()(T &obj, ...) {
        throw std::runtime_error("[type]" + std::string(typeid(obj).name())
            + " does not support moved convert to [type]" + std::string(typeid(U).name()));
        return U{};
    }
};

} // visitor
} // detail
} // vsjson
#endif