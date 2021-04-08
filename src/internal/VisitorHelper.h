#ifndef __JSON_UTILS_VISITOR_HELPER_H__
#define __JSON_UTILS_VISITOR_HELPER_H__
namespace vsjson {

template <typename R>
struct Return {
    using ReturnType = R;
};

struct NoReturn: public Return<void> {};

} // vsjson
#endif