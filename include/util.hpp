#ifndef MINIDB_UTIL_INCLUDED
#define MINIDB_UTIL_INCLUDED

#include <string_view>
#include <variant>

#define MAYBE_UNUSED(e) static_cast<void>(e)

namespace minidb {

template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace minidb

#endif // MINIDB_UTIL_INCLUDED
