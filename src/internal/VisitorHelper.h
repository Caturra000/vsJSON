#ifndef __JSON_UTILS_VISITOR_HELPER_H__
#define __JSON_UTILS_VISITOR_HELPER_H__
namespace vsjson {
namespace detail {
namespace visitor {

template <typename R>
struct Return {
    using ReturnType = R;
};

using NoReturn = Return<void>;

} // visitor
} // detail
} // vsjson
#endif