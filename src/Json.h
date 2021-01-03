#ifndef __JSON_H__
#define __JSON_H__
#include <bits/stdc++.h>
#include "utils/Variant.h"
#include "utils/TypeCompat.h"
#include "JsonValue.h"



class Json {
public:

    template <typename ...Args>
    Json(Args &&...args): _value(std::forward<Args>(args)...) { }

    template <size_t N>
    Json(const char (&str)[N]): _value(StringImpl(str)) {}

    Json(bool b): _value(Boolean(b)) {}
    Json(nullptr_t): _value(NullImpl(nullptr)) {}


    Json(const std::initializer_list<ObjectImpl::value_type> &list)
        : _value(ObjectImpl(list)) { }

    template <typename T>
    Json& operator=(const T& obj) {
        _value = obj;
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
        return JsonValue(std::vector<Json>{std::forward<Args>(elems)...});
    }

    friend std::ostream& operator<<(std::ostream &os, Json &json) {
        os << json.value();
        return os;
    }

    template <typename T>
    bool is() const { return _value.is<T>(); }

    template <typename T>
    T& as()  { return _value.get<T>(); }

    bool contains(const StringImpl &index) const {
        return _value.get<ObjectImpl>().count(index);
    }

    Json& operator[](const StringImpl &index) {
        return _value.get<ObjectImpl>()[index];
    }

    Json& operator[](size_t index) {
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

    std::string dump() {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
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
    if(vec.size() > 0) {
        auto last = --vec.end();
        for(auto it = vec.begin(); it != last; ++it) {
            if(it->is<StringImpl>()) os << '\"' << *it << "\", ";
            else os << *it << ", ";
        }
        if(last->is<StringImpl>()) os << '\"' << *last << '\"';
        else os << *last;
    }
    os << ']';
    return os;
}

std::ostream& operator<<(std::ostream &os, ObjectImpl &map) {
    os << '{';
    if(map.size() > 0) {
        auto last = --map.end();
        for(auto it = map.begin(); it != last; ++it) {
            os << '\"' << it->first << "\" : ";
            if(it->second.is<StringImpl>()) os << '\"' << it->second << "\", ";
            else os << it->second << ", ";
        }
        
        os << '\"' << last->first << "\" : ";
        if(last->second.is<StringImpl>()) os << '\"' << last->second << '\"';
        else os << last->second;
    }
    os << '}';
    return os;
}
#endif