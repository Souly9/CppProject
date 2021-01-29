#include <database.hpp>
#include <stdexcept>
#include <util.hpp>
#include <utility>
#include <numeric>

namespace minidb {

table& database::lookup_table(std::string_view name) {
	return tables_.at(name.data());
}

const table& database::lookup_table(std::string_view name) const {
	return tables_.at(name.data());
}

void database::create_table(std::string_view name, schema table_schema) {
	if(tables_.find(name) != tables_.end()) {
		throw std::invalid_argument("The table already exists in the database");
	}
	// Check if it moves out of parent contet without std::move
	tables_.emplace(name, table{std::string(name), std::move(table_schema)});
}

void database::drop_table(std::string_view name) {
	if(std::erase_if(tables_, [name](const auto& elem) { return elem.first == name; }) == 0) throw
			std::invalid_argument("Table name doesn't exist");
}

void database::append_row(std::string_view table_name, std::vector<value> cell_values) {
	auto& table = lookup_table(table_name);
	table.append_row(std::move(cell_values));
}

void database::erase_row(std::string_view table_name, std::size_t row_index) {
	auto& table = lookup_table(table_name);
	table.erase_row(row_index);
}

table& database::bind_filter_if(std::string_view table_name, row_filter& filter) {
	auto& table = lookup_table(table_name);
	filter.bind_to_table(table);
	return table;
}

void database::update_cell(std::string_view table_name, const std::size_t& row_index, const std::size_t& column_index,
                           const value& new_value) {
	auto& table = lookup_table(table_name);
	table.update_cell(row_index, column_index, new_value);
}

void database::erase_rows(std::string_view table_name, row_filter row_filter) {
	auto& table = bind_filter_if(table_name, row_filter);
	std::erase_if(table.rows(),
	              [&row_filter](const auto& row) -> bool { return row_filter(row); });
}

void database::update_rows(std::string_view table_name, row_filter row_filter,
                           std::unordered_map<std::string, value> changes) {

	auto& table = bind_filter_if(table_name, row_filter);
	for(auto& row : table.rows()) {
		if(row_filter(row)) {
			for(const auto& p : changes) {
				row.set_cell_value(table.get_column_index_by_name(p.first), p.second);
			}
		}
	}
}

std::map<value, std::size_t> database::query_column_histogram(std::string_view table_name, std::string_view column_name,
                                                              row_filter row_filter) const {

	const auto& table = lookup_table(table_name);
	row_filter.bind_to_table(table);
	std::map<value, std::size_t> rslt;
	const auto& index = table.get_column_index_by_name(column_name);
	for(const auto& row : table.rows()) {
		if(row_filter(row)) {
			rslt[row.get_cell_value(index)]++;
		}
	}
	return rslt;
}

std::map<value, std::size_t> database::query_column_histogram(std::string_view table_name,
                                                              std::string_view column_name) const {

	const auto& table = lookup_table(table_name);
	std::map<value, std::size_t> rslt;
	const auto& index = table.get_column_index_by_name(column_name);
	for(const auto& row : table.rows()) {
		rslt[row.get_cell_value(index)]++;
	}

	return rslt;
}

} // namespace minidb
