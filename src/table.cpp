#include <algorithm>
#include <stdexcept>
#include <table.hpp>
#include <utility>

namespace minidb {

row::row(std::vector<value> cells, schema* table_schema)
	: table_schema_(table_schema) {
	// TODO: Check for errors such as element number of cells not matching the number of columns in the table_schema, or
	//       the types of the supplied cell values not matching the types specified for the corrsponding columns in the
	//       table_schema. If such an error occurs, throw an appropriate exception, otherwise take on the content of
	//       cells as the content of the cells_ member variable.

	if(cells.size() != table_schema_->columns().size()) throw std::invalid_argument(
			"More columns in the scheme than cells");
	const bool isSame = std::equal(cells.begin(), cells.end(),
	                         table_schema_->columns().begin(), table_schema_->columns().end(),
	                         [](const value& cell, const column& column) -> bool
	                         {
		                        const auto val = std::visit([&]<typename T0>(T0 const&)
		                         {
		                         	return value_type_index<T0>::value;
		                         }, cell);
		                         return column.type() == val;
	                         });
	if(isSame) cells_ = std::move(cells);
}

table::table(std::string name, schema table_schema)
	: name_(std::move(name)), schema_(std::make_unique<schema>(std::move(table_schema))) {
}

std::size_t table::get_column_index_by_name(std::string_view name) const {
	// TODO: Return the index of the column with the given name in this table's schema, or throw an appropriate
	//       exception if no such column exists.
	for(std::size_t index = 0; index != schema_->columns().size(); ++index) {
		if(schema_->columns().at(index).name() == name) return index;
	}
	throw std::invalid_argument("No column with such name");
}

value_type table::get_column_type(std::size_t column_index) const {
	return schema_->columns().at(column_index).type();
}

const std::string& table::get_column_name(std::size_t column_index) const {
	return schema_->columns().at(column_index).name();
}

void table::append_row(std::vector<value> cell_values) {
	// TODO: Append a row with the supplied cell values to this table.
	rows_.emplace_back(row{std::move(cell_values), schema_.get()});
}

void table::erase_row(std::size_t row_index) {
	// TODO: Erase the row with the given index from this table, or throw an appropriate exception if no such row exists
	//       (i.e. if the index is out-of-bounds).
	if(row_index >= rows_.size()) throw std::out_of_range("Row index out of range");
	rows_.erase(rows_.begin() + row_index);
}

void table::update_cell(const std::size_t row_index, const std::size_t column_index, const value& value) {
	if(row_index >= rows_.size()) throw std::out_of_range("Row index out of range");
	rows_.at(row_index).set_cell_value(column_index, value);
}

std::ostream& operator<<(std::ostream& stream, const row& row) {
	for(const auto& cell : row.cells_) {
		stream << cell << " ";
	}
	return stream;
}
} // namespace minidb
