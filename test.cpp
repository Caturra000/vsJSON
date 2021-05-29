#include <bits/stdc++.h>
#include "vsjson.hpp"

struct Point {
    int x, y, z;
    operator vsjson::Json() { return vsjson::Json::array(x, y, z); }
    Point(int x, int y, int z): x(x), y(y), z(z) {}
    Point(const std::vector<vsjson::Json> &array)
        : x(array[0].to<int>()),
          y(array[1].to<int>()),
          z(array[2].to<int>()) {}
    Point() = default;
};

int main() {

    // 静态构造
    vsjson::Json json {
        {"index0", vsjson::Json::array(1, 2.5, "3", true)},
        {"index1", false},
        {"index2", vsjson::Json {
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
    Point p = json["index0"][4].to<Point>();
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
    auto json2 = vsjson::parse(toJson);

    auto toArrayJson = "[1, 2.3, false, \"jojo\"]";
    auto json3 = vsjson::parse(toArrayJson);

    std::cout << json2 << std::endl;
    std::cout << json3 << std::endl;

    // special judge
    std::string format1 = "\"3\"";
    auto json4 = vsjson::parse(format1);
    std::string format2 = json4.to<std::string>();
    std::string &format3 = json4.as<std::string>();
    std::string format4 = json4.dump();
    std::cout << format2 << std::endl;
    std::cout << format3 << std::endl;
    std::cout << format4 << std::endl;

    // 定制std::stringstream
    std::string veryVeryLong = R"(
        {"geometry":{"rings":[[[40426489.331430912,3001752.0858958033],[40426225.692211367,3001750.0779145896],
        [40426202.957955509,3001594.0301330695],[40426290.959128119,3001559.0584689],[40426390.723533183,3001537.0470353812],
        [40426464.66273,3001645.3116807956],[40426489.331430912,3001752.0858958033]]],"spatialReference":{"wkid":2364}},"succeed":true,"msg":""}
    )";
    vsjson::Json longJson = vsjson::parse(veryVeryLong);
    std::cout << longJson.dump(std::fixed, std::setprecision(10)) << std::endl;
    std::cout << longJson << std::endl;
    return 0;
}