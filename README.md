# vsJSON

## TL;DR

一个600行不到的JSON库，基于C++14实现，无任何依赖

利用`variant`、`visitor`、`template`和`stream`打造出来的一个好玩又好看的轮子

## 大概用法

```C++
#include <bits/stdc++.h>
#include "Json.h"
#include "Parser.h"

int main() {
    // 静态构造
    Json json {
        {"index0", Json::array(1, 2, "3", true)},
        {"index1", false},
        {"index2", Json {
            {"index2-1", nullptr}
        }}
    };

    // 直接输出
    std::cout << json << std::endl;

    // 简单序列化
    std::string serial = json.dump();
    std::cout << serial << std::endl;

    // 按index访问/append
    json["index3"] = 5;
    json["index0"].append(false);
    std::cout << json << std::endl;

    // 重新赋值
    json = nullptr;
    std::cout << json << std::endl;

    // 判断类型
    std::cout << (json.is<nullptr_t>() ? "true" : "false") << std::endl;

    // 简单parser
    auto toJson = R"(
        {
            "index0": [1, -2, "345", true, false], 
            "index1": false, 
            "index2": {
                "index2-1": {
                    "index2-1-1" : [true, false, 1, 233],
                    "index2-1-2" : false
                }
            }, 
            "index3": 5
        }
    )";
    auto json2 = parse(toJson);

    std::cout << json2 << std::endl;


    return 0;
}
```


## TODO

- 美化格式
- 完善parser