#ifndef MINIDB_ROW_FILTER_INCLUDED
#define MINIDB_ROW_FILTER_INCLUDED

#include "table.hpp"
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

namespace minidb {

class row_filter {
public:
	using filter_value_type = std::variant<long long, double, std::string>;

	row_filter(std::unordered_map<std::string_view, filter_value_type>&& filter)
		: filters{std::move(filter)} {
	}

	row_filter(const std::initializer_list<std::pair<std::string_view const, filter_value_type>>& filter)
		: filters(filter) {
	}

	template <typename BeginIt, typename EndIt>
	row_filter(BeginIt beginIt, EndIt endIt)
		: filters{beginIt, endIt} {
	}

	void bind_to_table(const table& tab);
	bool operator()(const row& r);

private:
	std::unordered_map<std::string_view, filter_value_type>
	filters;
	std::unordered_map<int, filter_value_type>
	column_indexed_filters;
};

} // namespace minidb

#endif // MINIDB_ROW_FILTER_INCLUDED
