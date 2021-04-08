#ifndef __JSON_JSON_VALUE_H__
#define __JSON_JSON_VALUE_H__
#include <bits/stdc++.h>
#include "internal/TypeCompat.h"
#include "internal/Variant.h"
namespace vsjson {

class Json;
using NullImpl = Null;
using ObjectImpl = std::map<std::string, Json>;
using ArrayImpl = std::vector<Json>;
using StringImpl = std::string;
using IntegerImpl = int;
using DecimalImpl = double;
using BooleanImpl = Boolean;

using JsonValue =
        Variant<NullImpl,
                ObjectImpl, 
                ArrayImpl, 
                StringImpl, 
                IntegerImpl, 
                DecimalImpl, 
                BooleanImpl>;

} // vsjson
#endif