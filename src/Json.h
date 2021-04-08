#ifndef __JSON_H__
#define __JSON_H__
#include <bits/stdc++.h>
#include "internal/Variant.h"
#include "internal/TypeCompat.h"
#include "JsonValue.h"
namespace vsjson {

class Json {
public:

    Json(): _value(ObjectImpl()) {}

    template <typename ...Args>
    Json(Args &&...args): _value(std::forward<Args>(args)...) {}

    template <size_t N>
    Json(const char (&str)[N]): _value(StringImpl(str)) {}

    Json(bool b): _value(Boolean(b)) {}
    Json(nullptr_t): _value(NullImpl(nullptr)) {}

    Json(const Json &rhs): _value(rhs._value) {}
    Json(Json &&rhs): _value(std::move(rhs._value)) {}

    Json(const std::initializer_list<ObjectImpl::value_type> &list)
        : _value(ObjectImpl(list)) {}

    Json(std::initializer_list<ObjectImpl::value_type> &&list)
        : _value(ObjectImpl(std::move(list))) {}


    // FIXME: use swap on operator=
    void swap(Json &rhs) {
        std::swap(*this, rhs);
    }

    template <typename T,
    typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Json>::value>>
    Json& operator=(T&& obj) {
        this->~Json();
        _value = std::forward<T>(obj);
        return *this;
    }

    Json& operator=(const Json &rhs) {
        if(this == &rhs) return *this;
        this->~Json();
        _value = rhs._value;
        return *this;
    }

    Json& operator=(Json &&rhs) {
        if(this == &rhs) return *this;
        this->~Json();
        _value = std::move(rhs._value);
        return *this;
    }
    
    template <size_t N>
    Json& operator=(const char (&str)[N]) {
        _value = StringImpl(str);
        return *this;
    }

    Json& operator=(bool b) {
        _value = BooleanImpl(b);
        return *this;
    }

    Json& operator=(nullptr_t) {
        _value = NullImpl(nullptr);
        return *this;
    }

    JsonValue& value() { return _value; }

    template <typename ...Args>
    static JsonValue array(Args &&...elems) {
        return JsonValue(ArrayImpl{std::forward<Args>(elems)...});
    }

    friend std::ostream& operator<<(std::ostream &os, Json &json) {
        os << json.value();
        return os;
    }

    template <typename T>
    bool is() const { return _value.is<T>(); }

    template <typename T>
    T& as()  { return _value.get<T>(); }

    template <typename T>
    T to() const & { return _value.to<T>(); }

    template <typename T>
    T to() && { return std::move(_value).to<T>(); }

    bool contains(const StringImpl &index) const {
        return _value.get<ObjectImpl>().count(index);
    }

    Json& operator[](const StringImpl &index) {
        return _value.get<ObjectImpl>()[index];
    }

    Json& operator[](size_t index) {
        return _value.get<ArrayImpl>()[index];
    }

    const Json& operator[](size_t index) const {
        return _value.get<ArrayImpl>()[index];
    }

    size_t size() const {
        return _value.get<ObjectImpl>().size();
    }
    
    size_t arraySize() const {
        return _value.get<ArrayImpl>().size();
    }

    void append(const Json &json) {
        _value.get<ArrayImpl>().emplace_back(json);
    }

    void append(Json &&json) {
        _value.get<ArrayImpl>().emplace_back(static_cast<Json&&>(json));
    }

    std::string dump() {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    auto begin() -> ObjectImpl::iterator {
        return _value.get<ObjectImpl>().begin();
    }

    auto end() -> ObjectImpl::iterator {
        return _value.get<ObjectImpl>().end();
    }

private:
    JsonValue _value;
};

template <>
bool Json::is<bool>() const { return _value.is<BooleanImpl>(); }

template <>
bool Json::is<nullptr_t>() const { return _value.is<NullImpl>(); }


std::ostream& operator<<(std::ostream &os, ArrayImpl &vec) {
    os << '[';
    if(!vec.empty()) {
        auto last = --vec.end();
        for(auto it = vec.begin(); it != last; ++it) {
            if(it->is<StringImpl>()) os << '\"' << *it << "\",";
            else os << *it << ',';
        }
        if(last->is<StringImpl>()) os << '\"' << *last << '\"';
        else os << *last;
    }
    os << ']';
    return os;
}

std::ostream& operator<<(std::ostream &os, ObjectImpl &map) {
    os << '{';
    if(!map.empty()) {
        auto last = --map.end();
        for(auto it = map.begin(); it != last; ++it) {
            os << '\"' << it->first << "\":";
            if(it->second.is<StringImpl>()) os << '\"' << it->second << "\",";
            else os << it->second << ',';
        }
        os << '\"' << last->first << "\":";
        if(last->second.is<StringImpl>()) os << '\"' << last->second << '\"';
        else os << last->second;
    }
    os << '}';
    return os;
}

} // vsjson
#endif