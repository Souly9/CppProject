#ifndef MINIDB_TABLE_INCLUDED
#define MINIDB_TABLE_INCLUDED

#include "schema.hpp"
#include "value.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <util.hpp>
#include <vector>

namespace minidb {

class row {
	std::vector<value> cells_;
	schema* table_schema_;

	friend class table;
	row(std::vector<value> cells, schema* table_schema);

public:
	friend std::ostream& operator<<(std::ostream& stream, const row& row);
	const value& get_cell_value(std::size_t column_index) const {
		return cells_.at(column_index);
	}

	const value& get_cell_value_if(std::size_t column_index) const {
		if(column_index >= cells_.size()) {
			throw std::out_of_range("Cell index out of range");
		}
		return get_cell_value(column_index);
	}

	const value_type get_value_type_at_cell(std::size_t column_index) const {
		return table_schema_->columns().at(column_index).type();
	}

	const value_type get_value_type_at_cell_if(std::size_t column_index) const {
		if(column_index >= cells_.size()) {
			throw std::out_of_range("Column index out of range");
		}
		return get_value_type_at_cell(column_index);
	}

	template <typename T>
	const T& get_cell_value(std::size_t column_index) const {
		return std::get<T>(get_cell_value_if(column_index));
	}

	// Allows supplying string_views for cells where a string is expected.
	void set_cell_value(std::size_t column_index, std::string_view val) {
		set_cell_value(column_index, std::string(val));
	}

	template <typename T>
	value_type getVal(T) const {
		throw std::invalid_argument("Invalid");
	}

	value_type getVal(value& v) const {
		return std::visit([]<typename T0>(T0)
		{
			return value_type_index<T0>::value;
		}, v);
	}


	template <typename T>
	void set_cell_value(std::size_t column_index, T val) {
		if(getVal(val) == get_value_type_at_cell_if(column_index)) {
			cells_.at(column_index) = val;
		} else {
			throw std::invalid_argument("Invalid type at the given index when setting cell");
		}
	}
};

class table {
	std::string name_;
	std::unique_ptr<schema> schema_;
	std::vector<row> rows_;

public:
	table(std::string name, schema table_schema);

	const std::vector<column>& columns() const noexcept {
		return schema_->columns();
	}

	const std::string& name() const noexcept {
		return name_;
	}

	const std::vector<row>& rows() const noexcept {
		return rows_;
	}

	std::vector<row>& rows() noexcept {
		return rows_;
	}

	std::size_t get_column_index_by_name(std::string_view name) const;
	value_type get_column_type(std::size_t column_index) const;
	const std::string& get_column_name(std::size_t column_index) const;
	void append_row(std::vector<value> cell_values);
	void erase_row(std::size_t row_index);
	void update_cell(const std::size_t row_index, const std::size_t column_index, const value& value);
};

} // namespace minidb

#endif // MINIDB_TABLE_INCLUDED
