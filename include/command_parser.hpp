#ifndef MINIDB_COMMAND_PARSER_INCLUDED
#define MINIDB_COMMAND_PARSER_INCLUDED

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace minidb {

class syntax_error : public std::runtime_error {
public:
	syntax_error(std::string msg, std::string_view context)
			: runtime_error(std::move(msg) + " Context: " + std::string(context)) {}
	syntax_error(std::string msg) : runtime_error(std::move(msg)) {}
};

class command_parser {
public:
	using integer_argument_type = long long;
	using decimal_argument_type = double;
	using string_argument_type = std::string;
	using primitive_argument_type = std::variant<integer_argument_type, decimal_argument_type, string_argument_type>;
	using list_argument_type = std::vector<primitive_argument_type>;
	using key_value_list_argument_type = std::vector<std::pair<string_argument_type, primitive_argument_type>>;
	using argument_type = std::variant<
			// Primitives:
			integer_argument_type, decimal_argument_type, string_argument_type,
			// Lists:
			list_argument_type,
			// Key-Value-List:
			key_value_list_argument_type>;
	static std::pair<std::string, std::vector<argument_type>> parse_command(std::string&& cmd);

private:
	static void consume_expected(std::string_view& input, const char& expected);
	static void consume_whitespace(std::string_view& input);
	static std::string_view extract_command(std::string_view& input);
	static std::string_view extract_text(std::string_view& input, const std::string& end_delimiters = " \t");
	static std::variant<std::monostate, long long, double> parse_number(std::string_view& text);
	static primitive_argument_type extract_primitive(std::string_view& input, const std::string& end_delimiters = " \t");
	static std::vector<primitive_argument_type> extract_list(std::string_view& input);
	static std::vector<std::pair<std::string, primitive_argument_type>> extract_key_value_list(std::string_view& input);
};

} // namespace minidb

#endif // MINIDB_COMMAND_PARSER_INCLUDED
