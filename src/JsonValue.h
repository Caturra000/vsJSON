#ifndef __JSON_JSON_VALUE_H__
#define __JSON_JSON_VALUE_H__
#include <bits/stdc++.h>
#include "utils/TypeCompat.h"
#include "utils/Variant.h"

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
#endif