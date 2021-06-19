#ifndef __JSON_H__
#define __JSON_H__
#include <bits/stdc++.h>
#include "internal/Variant.h"
#include "internal/TypeCompat.h"
#include "JsonValue.h"
namespace vsjson {

class Json {
public: // static
    template <typename ...Args>
    static JsonValue array(Args &&...elems);

public:
    Json(): _value(ObjectImpl()) {}
    template <typename ...Args>
    Json(Args &&...args): _value(std::forward<Args>(args)...) {}
    template <size_t N>
    Json(const char (&str)[N]): _value(StringImpl(str)) {}
    Json(bool b): _value(BooleanImpl(b)) {}
    Json(nullptr_t): _value(NullImpl(nullptr)) {}
    Json(const std::string &str): _value(StringImpl(str)) {}
    Json(std::string &&str): _value(StringImpl(static_cast<std::string&&>(str))) {}
    Json(const std::initializer_list<ObjectImpl::value_type> &list): _value(ObjectImpl(list)) {}
    Json(std::initializer_list<ObjectImpl::value_type> &&list): _value(ObjectImpl(std::move(list))) {}

    Json(const Json &rhs): _value(rhs._value) {}
    Json(Json &&rhs): _value(std::move(rhs._value)) {}

    template <typename T,
              typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Json>::value>>
    Json& operator=(T&& obj);
    Json& operator=(const Json &rhs);
    Json& operator=(Json &&rhs);
    template <size_t N>
    Json& operator=(const char (&str)[N]);
    Json& operator=(bool b);
    Json& operator=(nullptr_t);
    Json& operator=(const std::string &str);
    Json& operator=(std::string &&str);

    template <typename T> bool is() const { return _value.is<T>(); }
    template <typename T> T& as() { return _value.get<T>(); }
    template <typename T> T to() const & { return _value.to<T>(); }
    template <typename T> T to() && { return std::move(_value).to<T>(); }

    bool contains(const StringImpl &index) const;

    Json& operator[](const StringImpl &index);
    Json& operator[](size_t index);
    const Json& operator[](size_t index) const;

    size_t size() const;
    size_t arraySize() const;

    void append(const Json &json);
    void append(Json &&json);

    auto begin() -> ObjectImpl::iterator;
    auto end() -> ObjectImpl::iterator;

    std::string dump();
    template <typename ...Specifieds>
    std::string dump(Specifieds &&...ses);

    // FIXME: use swap on operator=
    void swap(Json &rhs) { std::swap(*this, rhs); }

    friend std::ostream& operator<<(std::ostream &os, vsjson::Json &json) { return os << json._value; }

private:
    JsonValue _value;
};

template <typename ...Args>
inline JsonValue Json::array(Args &&...elems) {
    return JsonValue(ArrayImpl{std::forward<Args>(elems)...});
}

template <typename T,typename>
inline Json& Json::operator=(T&& obj) {
    this->~Json();
    _value = std::forward<T>(obj);
    return *this;
}

inline Json& Json::operator=(const Json &rhs) {
    if(this == &rhs) return *this;
    this->~Json();
    _value = rhs._value;
    return *this;
}

inline Json& Json::operator=(Json &&rhs) {
    if(this == &rhs) return *this;
    this->~Json();
    _value = std::move(rhs._value);
    return *this;
}

template <size_t N>
inline Json& Json::operator=(const char (&str)[N]) {
    _value = StringImpl(str);
    return *this;
}

inline Json& Json::operator=(bool b) {
    _value = BooleanImpl(b);
    return *this;
}

inline Json& Json::operator=(nullptr_t) {
    _value = NullImpl(nullptr);
    return *this;
}

inline Json& Json::operator=(const std::string &str) {
    _value = StringImpl(str);
    return *this;
}

inline Json& Json::operator=(std::string &&str) {
    _value = StringImpl(static_cast<std::string&&>(str));
    return *this;
}

inline bool Json::contains(const StringImpl &index) const {
    return _value.get<ObjectImpl>().count(index);
}

inline Json& Json::operator[](const StringImpl &index) {
    return _value.get<ObjectImpl>()[index];
}
inline Json& Json::operator[](size_t index) {
    return _value.get<ArrayImpl>()[index];
}
inline const Json& Json::operator[](size_t index) const {
    return _value.get<ArrayImpl>()[index];
}

inline size_t Json::size() const {
    return _value.get<ObjectImpl>().size();
}

inline size_t Json::arraySize() const {
    return _value.get<ArrayImpl>().size();
}

inline void Json::append(const Json &json) {
    _value.get<ArrayImpl>().emplace_back(json);
}
inline void Json::append(Json &&json) {
    _value.get<ArrayImpl>().emplace_back(static_cast<Json&&>(json));
}

inline auto Json::begin() -> ObjectImpl::iterator {
    return _value.get<ObjectImpl>().begin();
}

inline auto Json::end() -> ObjectImpl::iterator {
    return _value.get<ObjectImpl>().end();
}

inline std::string Json::dump() {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

template <typename ...Specifieds>
inline std::string Json::dump(Specifieds &&...ses) {
    std::stringstream ss;
    // set stream status or put data before dump
    std::initializer_list<int> {((ss << std::forward<Specifieds>(ses)), 19260817)...};
    ss << (*this);
    return ss.str();
}

/// specialization

template <>
inline bool Json::is<bool>() const { return _value.is<BooleanImpl>(); }

template <>
inline bool Json::is<nullptr_t>() const { return _value.is<NullImpl>(); }

template <>
inline bool Json::is<std::string>() const { return _value.is<StringImpl>(); }

template <>
inline std::string& Json::as<std::string>()  { return _value.get<StringImpl>(); }

inline std::ostream& operator<<(std::ostream &os, ArrayImpl &vec) {
    os << '[';
    for(auto iter = vec.begin(); iter != vec.end(); ++iter) {
        os << *iter;
        if(std::distance(iter, vec.end()) > 1) {
            os << ',';
        }
    }
    os << ']';
    return os;
}

inline std::ostream& operator<<(std::ostream &os, ObjectImpl &map) {
    os << '{';
    for(auto iter = map.begin(); iter != map.end(); ++iter) {
        os << '\"' << iter->first << "\":" << iter->second;
        if(std::distance(iter, map.end()) > 1) {
            os << ',';
        }
    }
    os << '}';
    return os;
}

} // vsjson
#endif