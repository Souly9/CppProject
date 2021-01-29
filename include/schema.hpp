#ifndef MINIDB_SCHEMA_INCLUDED
#define MINIDB_SCHEMA_INCLUDED

#include "value.hpp"
#include <string>
#include <vector>

namespace minidb {

class column {
	std::string name_;
	value_type type_;

public:
	column(std::string name, value_type type) : name_(std::move(name)), type_(type) {}
	const std::string& name() const noexcept {
		return name_;
	}
	value_type type() const noexcept {
		return type_;
	}
};

class schema {
	std::vector<column> columns_;

public:
	schema(std::vector<column> columns) : columns_(std::move(columns)) {}
	const std::vector<column>& columns() const noexcept {
		return columns_;
	}
};

} // namespace minidb

#endif // MINIDB_SCHEMA_INCLUDED
