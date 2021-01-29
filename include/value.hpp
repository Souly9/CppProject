#ifndef MINIDB_VALUE_INCLUDED
#define MINIDB_VALUE_INCLUDED

#include <iosfwd>
#include <string>
#include <variant>

namespace minidb {

using value = std::variant<long long, double, std::string>;
enum class value_type { integer, decimal, string };

template <typename T>
struct value_type_index {};
template <>
struct value_type_index<long long> {
	static constexpr auto value = value_type::integer;
};
template <>
struct value_type_index<double> {
	static constexpr auto value = value_type::decimal;
};
template <>
struct value_type_index<std::string> {
	static constexpr auto value = value_type::string;
};

template<typename T>
inline value to_value(T val) {
	throw std::exception("Not valid value");
}
inline value to_value(long long val) {
	return val;
}
inline value to_value(double val) {
	return val;
}
inline value to_value(std::string val) {
	return value(std::move(val));
}
inline value to_value(std::string_view val) {
	return std::string(val);
}

std::ostream& operator<<(std::ostream& ostr, const minidb::value& val);

} // namespace minidb

#endif // MINIDB_VALUE_INCLUDED
