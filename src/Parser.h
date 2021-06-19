#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__
#include <bits/stdc++.h>
#include "Json.h"
#include "JsonException.h"
namespace vsjson {

/// interface

Json parse(const char *p);
Json parse(const std::string &str);

/// impl

namespace parser {
    Json parseImpl(const char *&p);
    Json parseNumberImpl(const char *&p);
    StringImpl parseString(const char *&p);
    Json parseObject(const char *&p);
    Json parseArray(const char *&p);
    IntegerImpl parseInteger(const char *&p);
    DecimalImpl parseDeciaml(const char *&p);
    DecimalImpl parseExponent(const char *&p);
}

inline Json parse(const char *p) {
    return parser::parseImpl(p);
}

inline Json parse(const std::string &str) {
    return parse(str.data());
}

namespace parser {

inline bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r';
}

inline const char* skipWhitespace(const char *p) {
    while(p && (isWhitespace(*p))) ++p;
    return p;
}

inline Json parseImpl(const char *&p) {
    p = skipWhitespace(p);
    if(!p || !*p) return nullptr;
    IntegerImpl buf = 0;
    bool neg = false;
    switch (*p) {
        case '{':
            return parseObject(p);
        case '[':
            return parseArray(p);
        case '\"':
            return parseString(p);
        case 'n':
            p += 4;
            return nullptr;
        case 't':
            p += 4;
            return true;
        case 'f':
            p += 5;
            return false;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            return parseNumberImpl(p);
        default:
            // impossible
            return nullptr;
    }
    return nullptr;
}

inline Json parseNumberImpl(const char *&p) {
    bool neg = false;
    if(*p == '-') {
        ++p;
        neg = true;
    }
    if(isdigit(*p)) {
        IntegerImpl integer = parseInteger(p);
        if(*p != '.') {
            return !neg ? integer : -integer;
        }
        // *p == '.'
        DecimalImpl decimal = parseDeciaml(p);
        decimal += integer;
        if(*p != 'e' && *p != 'E') {
            return !neg ? decimal : -decimal;
        }
        // *p == 'e' || *p == 'E'
        DecimalImpl e = parseExponent(p);
        return !neg ? e*decimal : -e*decimal;
    }
    return nullptr;
}

inline Json parseObject(const char *&p) {
    ++p; // {
    p = skipWhitespace(p);
    Json object(ObjectImpl{});
    if(*p == '}') {
        ++p;
        return object;
    }
    for(;;) {
        p = skipWhitespace(p);
        StringImpl key = parseString(p);
        p = skipWhitespace(p);
        if(*p != ':') {
            throw JsonException(
                "object parse failure: expect [:]");
        }
        ++p; // :
        p = skipWhitespace(p);
        object[key] = parseImpl(p);
        p = skipWhitespace(p);
        if(*p == '}') {
            ++p;
            break;
        } else if(*p == ',') {
            ++p;
        } else {
            throw JsonException(
                "object parse failure: unknown reason");
        }
    }
    return object;
}

inline Json parseArray(const char *&p) {
    ++p;
    p = skipWhitespace(p);
    Json array = Json::array();
    if(*p == ']') {
        ++p;
        return array;
    }
    for(;;) {
        p = skipWhitespace(p);
        auto r = parseImpl(p);
        array.append(r);
        p = skipWhitespace(p);
        if(*p == ']') {
            ++p;
            break;
        } else if(*p == ',') {
            ++p;
        } else {
            throw JsonException(
                "arrary parse failure: unknown reason");
        }
    }
    return array;
}

inline StringImpl parseString(const char *&p) {
    p = skipWhitespace(p);
    if(*p != '\"') {
        throw JsonException(
            "string parse failure: expect [\"]");
    }
    ++p; // "
    auto start = p;
    while(p && *p != '\"') {
        if(*p == '\\') ++p; // switch-case *p: quote / solidus / backspace...
        ++p;
    }
    if(!p) {
        throw JsonException(
            "string parse failure: pair [\"]");
    }
    auto end = p; //[start, end)
    ++p; // "
    return StringImpl(start, end - start);
}

inline IntegerImpl parseInteger(const char *&p) {
    IntegerImpl i = 0;
    p = skipWhitespace(p);
    while(p && isdigit(*p)) { // -
        i = i*10 + (*p - '0');
        ++p;
    }
    p = skipWhitespace(p);
    if(!p || !*p) {
        throw JsonException(
            "integer parse failure: assert non-\\n");
    }
    return i;
}

inline DecimalImpl parseDeciaml(const char *&p) {
    ++p; // '.'
    p = skipWhitespace(p);
    DecimalImpl d = 0, idx = 0.1;
    while(p && isdigit(*p)) {
        d += idx * ((*p) - '0');
        idx *= 0.1;
        ++p;
    }
    p = skipWhitespace(p);
    if(!p || !*p) {
        throw JsonException(
            "decimal parse failure: assert non-\\n");
    }
    return d;
}

template <typename>
struct ExponentCache;

// generate 1.0, 10.0, 100.0... at compile time
template <size_t ...Is>
struct ExponentCache<std::index_sequence<Is...>> {
    constexpr static DecimalImpl pows[] = {std::pow(10, Is)...};
};

template <size_t ...Is>
constexpr DecimalImpl ExponentCache<std::index_sequence<Is...>>::pows[];

inline DecimalImpl parseExponent(const char *&p) {
    static constexpr size_t exponentLimit = 309;
    using Generator = std::make_index_sequence<exponentLimit>;
    ++p; // e E
    p = skipWhitespace(p);
    bool neg = (*p == '-');
    if(*p == '+' || *p == '-') ++p;
    IntegerImpl i = parseInteger(p);
    p = skipWhitespace(p);
    if(!p || !*p) {
        throw JsonException(
            "exponent parse failure: assert non-\\n");
    }
    DecimalImpl e = ExponentCache<Generator>::pows[i];
    return !neg ? e : 1.0/e;
}

} // parser

} // vsjson
#endif