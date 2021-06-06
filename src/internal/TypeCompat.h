#ifndef __JSON_UTILS_TYPE_COMPAT_H__
#define __JSON_UTILS_TYPE_COMPAT_H__
#include <bits/stdc++.h>
namespace vsjson {

struct Boolean {
    bool boolean;
    Boolean() = default;
    Boolean(bool boolean): boolean(boolean) {}
    Boolean(const Boolean&) = default;
    operator bool() { return boolean; }
    friend std::ostream& operator<<(std::ostream &os, Boolean b) {
        os << (b.boolean ? "true" : "false");
        return os;
    }
};

struct Null {
    nullptr_t null;
    Null() = default;
    Null(nullptr_t): null(nullptr) {}
    Null(const Null &) = default;
    operator nullptr_t() { return nullptr; }
    friend std::ostream& operator<<(std::ostream &os, Null null) {
        os << "null";
        return os;
    }
};

struct FormatString: public std::string {
    using Base = std::string;
    using Base::Base;
    FormatString(const Base &base): Base(base) {}
    FormatString(Base &&base): Base(static_cast<Base&&>(base)) {}
    FormatString(const FormatString &) = default;
    FormatString(FormatString &&) = default;
    FormatString& operator=(FormatString rhs) {
        rhs.swap(*this);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream &os, FormatString &fs) {
        os << '\"' << static_cast<Base&>(fs) << '\"';
        return os;
    }
};

} // vsjson
#endif