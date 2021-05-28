#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__
#include <bits/stdc++.h>
#include "Json.h"
#include "JsonException.h"
namespace vsjson {

namespace ParserImpl {
    Json parseImpl(const char *&p);
    Json parseNumberImpl(const char *&p);
    StringImpl parseString(const char *&p);
    Json parseObject(const char *&p);
    Json parseArray(const char *&p);
    IntegerImpl parseInteger(const char *&p);
    DecimalImpl parseDeciaml(const char *&p);
}

inline Json parse(const char *p) {
    return ParserImpl::parseImpl(p);
}

inline Json parse(const std::string &str) {
    return parse(str.data());
}

namespace ParserImpl {

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
    IntegerImpl buf = 0;
    bool neg = false;
    switch (*p) {
        case '-':
            ++p;
            neg = true;
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
            // case: -.123
            if(*p != '.') {
                buf = parseInteger(p);
            }
            // shoule be integer
            if(!p || *p != '.') {
                return !neg ? buf : -buf;
            }
        case '.':
            return !neg ? (buf + parseDeciaml(p))
                : -(buf + parseDeciaml(p));
        default:
            return nullptr;
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
    while(p && *p != '\"') ++p;
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
    while(p && isdigit(*p)) { // -
        i = i*10 + (*p - '0');
        ++p;
    }
    if(!p || !*p) {
        throw JsonException(
            "integer parse failure: assert non-\\n");
    }
    return i;
}

inline DecimalImpl parseDeciaml(const char *&p) {
    ++p; // '.'
    DecimalImpl d = 0, idx = 0.1;
    while(p && isdigit(*p)) { // TODO E e
        d += idx * ((*p) - '0');
        idx *= 0.1;
        ++p;
    }
    if(!p || !*p) {
        throw JsonException(
            "decimal parse failure: assert non-\\n");
    }
    return d;
}

} // ParserImpl

} // vsjson
#endif