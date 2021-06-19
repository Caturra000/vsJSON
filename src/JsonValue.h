#ifndef __JSON_JSON_VALUE_H__
#define __JSON_JSON_VALUE_H__
#include <bits/stdc++.h>
#include "internal/TypeCompat.h"
#include "internal/Variant.h"
namespace vsjson {

class Json;
using NullImpl = detail::Null;
using ObjectImpl = std::map<std::string, Json>;
using ArrayImpl = std::vector<Json>;
using StringImpl = detail::FormatString; //std::string;
using IntegerImpl = int;
using DecimalImpl = double;
using BooleanImpl = detail::Boolean;

using JsonValue =
detail::Variant<NullImpl,
                ObjectImpl,
                ArrayImpl,
                StringImpl,
                IntegerImpl,
                DecimalImpl,
                BooleanImpl>;

} // vsjson
#endif