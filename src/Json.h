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


    static JsonValue array(const std::initializer_list<Json> &list) {
        return JsonValue(std::vector<Json>(list));
    }

    static JsonValue array() {
        return JsonValue(std::vector<Json>());
    }

    friend std::ostream& operator<<(std::ostream &os, Json &json) {
        os << json.value();
        return os;
    }

    bool isString() { return _value._what == 3; } // FIXME: HARD CODE

    bool contains(const StringImpl &index) {
        return _value.get<ObjectImpl>().count(index);
    }

    Json& operator[](const StringImpl &index) {
        return _value.get<ObjectImpl>()[index];
    }

    size_t size() {
        return _value.get<ObjectImpl>().size();
    }
    
    size_t arraySize() {
        return _value.get<ArrayImpl>().size();
    }

    void pushBack(const Json &json) {
        _value.get<ArrayImpl>().emplace_back(json);
    }
private:
    JsonValue _value;
};



std::ostream& operator<<(std::ostream &os, ArrayImpl &vec) {
    os << '[';
    if(vec.size() > 0) {
        auto last = --vec.end();
        for(auto it = vec.begin(); it != last; ++it) {
            if(it->isString()) os << '\"' << *it << "\", ";
            else os << *it << ", ";
        }
        if(last->isString()) os << '\"' << *last << '\"';
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
            os << '\"' << it->first << "\": ";
            if(it->second.isString()) os << '\"' << it->second << "\", ";
            else os << it->second << ", ";
        }
        
        os << '\"' << last->first << "\": ";
        if(last->second.isString()) os << '\"' << last->second << '\"';
        else os << last->second;
    }
    os << '}';
    return os;
}
#endif