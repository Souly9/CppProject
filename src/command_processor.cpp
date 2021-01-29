#include "database.hpp"
#include "util.hpp"
#include <algorithm>
#include <command_processor.hpp>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

namespace minidb {

command_processor::command_processor(database& db)
	: db(db) {
	using args_t = std::vector<command_parser::argument_type>;
	using namespace std::literals;

	callback_structure.emplace("create_table"s, &command_processor::execute_create_table);
	callback_structure.emplace("drop_table"s, &command_processor::execute_drop_table);
	callback_structure.emplace("append_row"s, &command_processor::execute_append_row);
	callback_structure.emplace("update_rows"s, &command_processor::execute_update_rows);
	callback_structure.emplace("update_cell"s, &command_processor::execute_update_cell);
	callback_structure.emplace("query_table"s, &command_processor::execute_query_table);
	callback_structure.emplace("query_column_histogram"s, &command_processor::execute_query_column_histogram);
	callback_structure.emplace("erase_row"s, &command_processor::execute_erase_row);
	callback_structure.emplace("erase_rows"s, &command_processor::execute_erase_rows);
}

void command_processor::execute_help(std::ostream& output) {
	output <<
			R"(Available commands:
help: Display this help text.
exit: Exit the CLI.
create_table <table name> {<column name 0>=<column type 0>,<column name 1>=<column type 1>,...}:
		Create a table with the given schema and name.
		The valid column types are: integer, decimal, string
drop_table <table name>: Drop the named table.
append_row <table name> [<row value for column 0>,<row value for column 1>,...]:
		Append a row with the given values to the named table.
update_rows <table name> {<filter column name 0>=<filter column value 0>,<filter column name 1>=<filter column value 1>,...} {<update column name 0>=<update column value 0>,<update column name 1>=<update column value 1>,...}:
		Update all rows in the named table that match the given filter to set the values for the given update columns to their corresponding new value.
		The filter and the update component name columns and corresponding values.
		A row matches the filter if the cell for each column named in the filter has a value equal to the value given in the filter.
		The columns named in the update component are set to the value given for them there for all matching rows.
		Example: update_row tab {foo=test,bar=42} {foo="test test"}
			This updates all rows with "test" in their "foo" column cell and 42 in their "bar" column cell to have "test test" in their "foo" column cell.
update_cell <table name> <row index> <column index> <new value>:
		Update the cell for the column with the given index (starting at 0) in the row with the given index (starting at 0) of the named table to the given value.
query_table <table name> {<filter column name 0>=<filter column value 0>,<filter column name 1>=<filter column value 1>,...}:
		Display all rows in the named table that match the given row filter.
		The filter is optional. If it is ommitted, all rows are displayed.
		A row matches the filter if the cell for each column named in the filter has a value equal to the value given in the filter.
query_column_histogram <table name> <column name> {<filter column name 0>=<filter column value 0>,<filter column name 1>=<filter column value 1>,...}:
		Generate and display a histogram (number of occurrences for each distinct value) for the values in named column of the row in the named table that match the given row filter.
		The filter is optional. If it is ommitted, all rows are included in the histogram.
		A row matches the filter if the cell for each column named in the filter has a value equal to the value given in the filter.
erase_row <table name> <row index>: Delete the row with the given index (starting at 0) from the named table.
erase_rows <table name> {<filter column name 0>=<filter column value 0>,<filter column name 1>=<filter column value 1>,...}:
		Delete all rows from the named table that match the given filter.
		The filter names columns and corresponding values.
		A row matches the filter if the cell for each column named in the filter has a value equal to the value given in the filter.
)";
}

void command_processor::execute(std::string command_line, std::ostream& output) {
	using namespace std::literals;
	auto [command, arguments] = command_parser::parse_command(std::move(command_line));

	if(command == "help") {
		execute_help(output);
	} else if(command == "exit") {
		this->exit = true;
	} else {
		(this->*callback_structure[command])(arguments, output);
	}
}


void command_processor::execute_create_table(const std::vector<command_parser::argument_type>& arguments,
                                             std::ostream& output) {
	if(arguments.size() == 2) {
		const auto& argVec = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(1));

		std::vector<column> columns;
		for(const std::pair<command_parser::string_argument_type, command_parser::primitive_argument_type>& pair :
		    argVec) {

			const auto& v = get_from_argument<std::string>(pair.second);
			value_type secArgs;
			if(v == "integer") secArgs = value_type_index<command_parser::integer_argument_type>::value;
			else if(v == "decimal") secArgs = value_type_index<command_parser::decimal_argument_type>::value;
			else if(v == "string") secArgs = value_type_index<command_parser::string_argument_type>::value;
			else throw std::invalid_argument("Type not valid");
			columns.emplace_back(column{pair.first, secArgs});
		}

		db.create_table(get_from_argument<std::string>(arguments.at(0)), schema{columns});
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
	MAYBE_UNUSED(output);
}

void command_processor::execute_drop_table(const std::vector<command_parser::argument_type>& arguments,
                                           std::ostream& output) {
	if(arguments.size() == 1) {
		const auto& name = get_from_argument<std::string>(arguments.at(0));
		db.drop_table(name);
		output << "Table names " << name << "dropped";
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}

}

void command_processor::execute_append_row(const std::vector<command_parser::argument_type>& arguments,
                                           std::ostream& output) {
	if(arguments.size() == 2) {
		db.append_row(
				get_from_argument<std::string>(arguments.at(0)),
				get_from_argument<std::vector<value>>(arguments.at(1))
				);
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
	MAYBE_UNUSED(output);
}

void command_processor::execute_update_rows(const std::vector<command_parser::argument_type>& arguments,
                                            std::ostream& output) {
	if(arguments.size() == 3) {
		const auto& filterPairs = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(1));
		const auto& changes = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(2));
		db.update_rows
				(
						get_from_argument<std::string>(arguments.at(0)),
						row_filter{filterPairs.begin(), filterPairs.end()},
						std::unordered_map{changes.begin(), changes.end()}
						);
		output << "Updated rows";
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
}

void command_processor::execute_update_cell(const std::vector<command_parser::argument_type>& arguments,
                                            std::ostream& output) {
	if(arguments.size() == 4) {
		db.update_cell
				(
						get_from_argument<std::string>(arguments.at(0)),
						get_from_argument<long long>(arguments.at(1)),
						get_from_argument<long long>(arguments.at(2)),
						get_value_from_argument(arguments.at(3))
						);
		output << "Updated cell";
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
}

void command_processor::execute_query_table(const std::vector<command_parser::argument_type>& arguments,
                                            std::ostream& output) {
	if(arguments.size() == 1 || arguments.size() == 2) {
		const auto& callback = [&output](const row& row)
		{
			output << row << '\n';
		};
		if(arguments.size() == 2) {
			const auto& filterPairs = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(1));
			db.query_table
					(
							get_from_argument<std::string>(arguments.at(0)),
							row_filter{filterPairs.begin(), filterPairs.end()},
							callback
							);
			return;
		}
		db.query_table
				(
						get_from_argument<std::string>(arguments.at(0)),
						callback
						);

	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
}

void command_processor::execute_query_column_histogram(
		const std::vector<command_parser::argument_type>& arguments,
		std::ostream& output) {
	if(arguments.size() == 2 || arguments.size() == 3) {
		std::map<value, size_t> rslt;
		if(arguments.size() == 3) {
			const auto& filterPairs = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(2));
			rslt = db.query_column_histogram
					(
							get_from_argument<std::string>(arguments.at(0)),
							get_from_argument<std::string>(arguments.at(1)),
							row_filter{filterPairs.begin(), filterPairs.end()}
							);
		} else {
			rslt = db.query_column_histogram
					(
							get_from_argument<std::string>(arguments.at(0)),
							get_from_argument<std::string>(arguments.at(1))
							);
		}
		for(const auto& p : rslt) output << p.first << " " << p.second << '\n';
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
}

void command_processor::execute_erase_row(const std::vector<command_parser::argument_type>& arguments,
                                          std::ostream& output) {
	if(arguments.size() == 2) {
		db.erase_row(
				get_from_argument<std::string>(arguments.at(0)),
				get_from_argument<long long>(arguments.at(1))
				);
		output << "Erased row";
	} else {
		throw std::invalid_argument("Expected 2 arguments got " + std::to_string(arguments.size()) + " arguments");
	}
}

void command_processor::execute_erase_rows(const std::vector<command_parser::argument_type>& arguments,
                                           std::ostream& output) {
	if(arguments.size() == 2) {
		const auto& filterPairs = get_from_argument<command_parser::key_value_list_argument_type>(arguments.at(1));
		db.erase_rows
				(
						get_from_argument<std::string>(arguments.at(0)),
						row_filter{filterPairs.begin(), filterPairs.end()}
						);
		output << "Erased rows";
	} else {
		throw std::invalid_argument("Arguments don't match specified pattern");
	}
}
} // namespace minidb
