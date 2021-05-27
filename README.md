# vsJSON

## TL;DR

一个600行不到的JSON库，基于C++14实现，无任何依赖

利用`variant`、`visitor`、`template`和`stream`打造出来的一个好玩又好看的轮子

## 大概用法

```C++
#include <bits/stdc++.h>
#include "vsjson.hpp"
using namespace vsjson;

struct Point {
    int x, y, z;
    operator Json() { return Json::array(x, y, z); }
    Point(int x, int y, int z): x(x), y(y), z(z) {}
    Point(const Json &json)
        : x(json[0].to<int>()),
          y(json[1].to<int>()),
          z(json[2].to<int>()) {}
};

int main() {

    // 静态构造
    Json json =
    {
        {"index0", Json::array(1, 2.5, "3", true)},
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

    // 按index访问 / append / 任意类型转为Json
    json["index3"] = 5;
    json["index0"].append(Point {998, 244, 353});
    std::cout << json << std::endl;
    std::cout << json["index0"][4] << std::endl;
    Point p = json["index0"][4];
    std::cout << p.x << ' ' << p.y << ' ' << p.z << std::endl;

    // 提供STL复用的迭代器
    for(auto &&iter : json) {
        std::cout << iter.first << ' ' << iter.second << std::endl;
    }

    // 重新赋值
    json = nullptr;
    std::cout << json << std::endl;

    // 判断类型
    bool isNull = json.is<nullptr_t>();
    std::cout << (isNull ? "true" : "false") << std::endl;

    // 简单parser
    auto toJson = R"(
        {
            "index0": [1, -2.345, "345", true, false],
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
## 设计理念

0. 我觉得`visitor/variant`非常适合JSON库的设计：`visitor`可以避免类型的枚举，套上`template`甚至一劳永逸，而`variant`又可表示（返回）多个类型，可以在parse阶段大量简化代码量，并且在`class Json`设计中也可更加抽象地表示出来，但是github上没找到类似的实现，于是想造轮子
1. 使用舒适的JSON库很重要，因此需要一个看起来足够`JSON`，用起来又很`C++`的JSON库：`vsJSON`虽然重复造了`Variant`（`std::variant`需要`C++17`），但是`class Json`中的容器是复用STL提供的`std::map`和`std::vector`，使用上并不需要多余的学习成本；另外`class Json`支持的`std::initiliazer_list`构造方式非常直观，并且从语言层面来保证JSON的格式是正确的
2. 符合1要求的有一个著名的库，但是实现太长了
3. 造轮子其实不需要理由

[关于variant和visitor的使用示例](https://zhuanlan.zhihu.com/p/57530780)

稍后我会更新关于variant的实现细节，以及展示从variant到json的实现过程

## TODO

- 提供可读性更高的输出格式
- 完善parser