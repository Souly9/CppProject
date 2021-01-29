#ifndef MINIDB_DATABASE_INCLUDED
#define MINIDB_DATABASE_INCLUDED

#include "row_filter.hpp"
#include "table.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <util.hpp>

namespace minidb {

class database {
	std::map<std::string, table, std::less<>> tables_;

public:
	using rowCallBack = std::function<void(const row&)>;

	const auto& tables() const noexcept {
		return tables_;
	}

	table& lookup_table(std::string_view name);
	const table& lookup_table(std::string_view name) const;
	void create_table(std::string_view name, schema table_schema);
	void drop_table(std::string_view name);
	void append_row(std::string_view table_name, std::vector<value> cell_values);
	void erase_row(std::string_view table_name, std::size_t row_index);
	void erase_rows(std::string_view table_name, row_filter filter);
	void update_rows(std::string_view table_name, row_filter filter, std::unordered_map<std::string, value> changes);
	table& bind_filter_if(std::string_view table_name, row_filter& filter);
	void update_cell(std::string_view table_name, const std::size_t& row_index, const std::size_t& column_index,
	                 const value& new_value);

	void query_table(std::string_view table_name, const rowCallBack& row_callback
			) const {
		const auto& table = lookup_table(table_name);
		for(const auto& row : table.rows()) row_callback(row);
	}

	void query_table(std::string_view table_name, row_filter filter,
	                 const rowCallBack& row_callback) const {
		const auto& table = lookup_table(table_name);
		filter.bind_to_table(table);
		for(const auto& row : table.rows()) {
			if(filter(row)) row_callback(row);
		}
	}

	auto query_column_histogram(std::string_view table_name, std::string_view column_name,
	                            row_filter filter) const -> std::map<value, std::size_t>;
	auto query_column_histogram(std::string_view table_name,
	                            std::string_view column_name) const -> std::map<value, std::size_t>;
};

} // namespace minidb

#endif // MINIDB_DATABASE_INCLUDED
