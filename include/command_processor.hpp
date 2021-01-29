#ifndef MINIDB_COMMAND_PROCESSOR_INCLUDED
#define MINIDB_COMMAND_PROCESSOR_INCLUDED

#include "command_parser.hpp"
#include "row_filter.hpp"
#include "value.hpp"
#include <array>
#include <functional>
#include <iosfwd>
#include <map>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <exception>

namespace minidb {

class database;

class command_processor {
	database& db;
	bool exit = false;
	using commandCallback = void (command_processor::*)(const std::vector<command_parser::argument_type>&,
	                                                    std::ostream&);
	std::unordered_map<std::string, commandCallback> callback_structure;

	static void execute_help(std::ostream& output);
	void execute_create_table(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_drop_table(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_append_row(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_update_rows(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_update_cell(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_query_table(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_query_column_histogram(const std::vector<command_parser::argument_type>& arguments,
	                                    std::ostream& output);
	void execute_erase_row(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);
	void execute_erase_rows(const std::vector<command_parser::argument_type>& arguments, std::ostream& output);

	template <typename T, typename... Arg>
	T get_from_argument(const std::variant<Arg...>& arg) const {
		try {
			return std::get<T>(arg);
			// Mainly for more expressive exceptions
		} catch(const std::exception&) {
			throw std::invalid_argument("Invalid argument supplied!");
		}
	}

	template <typename... Arg>
	value get_value_from_argument(const std::variant<Arg...>& arg) const {
		return std::visit([](const auto& a) { return to_value(a); }, arg);
	}

public:
	command_processor(database& db);
	void execute(std::string command_line, std::ostream& output);

	bool should_exit() const noexcept {
		return exit;
	}
};

} // namespace minidb

#endif // MINIDB_COMMAND_PROCESSOR_INCLUDED
