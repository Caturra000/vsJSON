#ifndef __JSON_EXCEPTION_H__
#define __JSON_EXCEPTION_H__
#include <bits/stdc++.h>
namespace vsjson {

// json功能较为单一，只使用一个excpetion类
class JsonException: public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // vsjson
#endif