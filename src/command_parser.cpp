#include <algorithm>
#include <command_parser.hpp>

namespace minidb {

using namespace std::literals;
// Moving the string in as no caller uses the passed in string again
std::pair<std::string, std::vector<command_parser::argument_type>>
command_parser::parse_command(std::string&& cmd_lin) {
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
	std::vector<argument_type> arguments;
	auto cmd_line = std::string_view{cmd_lin};
	consume_whitespace(cmd_line);
	const auto command = extract_command(cmd_line);
	consume_whitespace(cmd_line);
	while(!cmd_line.empty()) {
		switch(cmd_line.front()) {
		case '[': arguments.emplace_back(extract_list(cmd_line));
			break;
		case '{': arguments.emplace_back(extract_key_value_list(cmd_line));
			break;
		default: std::visit([&](auto&& val) { arguments.emplace_back(val); }, extract_primitive(cmd_line));
			break;
		}
		consume_whitespace(cmd_line);
	}
	return {std::string{command}, arguments};
}

void command_parser::consume_expected(std::string_view& input, const char& expected) {
	if(input.empty()) {
		throw syntax_error("No remaining input when expecting character '"s + expected + "'."s);
	}
	if(input.front() != expected) {
		throw syntax_error("Found unexpected character where '"s + expected + "' was expected."s, input);
	}
	input = input.substr(1);
}

void command_parser::consume_whitespace(std::string_view& input) {
	input = input.substr(std::min(input.find_first_not_of(" \t"sv), input.size()));
}

std::string_view command_parser::extract_command(std::string_view& input) {
	const auto separator_pos = input.find_first_of(" \t"sv);
	auto cmd = input.substr(0, separator_pos);
	input = input.substr(std::min(separator_pos, input.size()));
	return cmd;
}

std::string_view command_parser::extract_text(std::string_view& input, const std::string& end_delimiters) {
	if(input.front() == '"') {
		const auto end_pos = input.find('"', 1);
		if(end_pos == std::string_view::npos) {
			throw syntax_error("Input ends before string."s, input);
		}
		auto result = input.substr(1, end_pos - 1);
		input = input.substr(end_pos + 1);
		consume_whitespace(input);
		return result;
	}
	const auto end_pos = input.find_first_of(end_delimiters);
	const auto element_text = input.substr(0, end_pos);
	input = input.substr(std::min(end_pos, input.size()));
	return element_text.substr(0, element_text.find_last_not_of(" \t") + 1);
}

std::variant<std::monostate, long long, double> command_parser::parse_number(std::string_view& text) {
	using namespace std::literals;
	if(text.empty()) {
		return {};
	}
	bool negative = false;
	if(text.front() == '-') {
		negative = true;
		text = text.substr(1);
		if(text.empty()) {
			return {};
		}
	}
	std::size_t index = 0;
	long long integral_part = 0;
	if(text.front() == '0') {
		index = 1;
	} else {
		for(; index < text.size(); ++index) {
			if('0' <= text[index] && text[index] <= '9') {
				integral_part = integral_part * 10 + (text[index] - '0');
			} else {
				break;
			}
		}
	}
	if(index < text.size() && text[index] == '.') {
		++index;
		double fractional_part = 0.0;
		double weight = 1.0 / 10.0;
		for(; index < text.size(); ++index) {
			if('0' <= text[index] && text[index] <= '9') {
				fractional_part += weight * (text[index] - '0');
				weight /= 10.0;
			} else {
				break;
			}
		}
		if(index < text.size() && (text[index] == 'e' || text[index] == 'E')) {
			return {};
		}
		text = text.substr(index);
		return (negative ? -1.0 : 1.0) * (integral_part + fractional_part);
	}
	text = text.substr(index);
	return (negative ? -integral_part : integral_part);
}

command_parser::primitive_argument_type command_parser::extract_primitive(std::string_view& input,
                                                                          const std::string& end_delimiters) {
	if(input.front() == '"') {
		return std::string{extract_text(input, end_delimiters)};
	}
	const auto end_pos = input.find_first_of(end_delimiters);
	const auto element_text = input.substr(0, end_pos);
	auto number_element_text = element_text;
	const auto number_result = parse_number(number_element_text);
	input = input.substr(std::min(end_pos, input.size()));
	if(std::holds_alternative<std::monostate>(number_result) ||
	   number_element_text.find_first_not_of(" \t") != std::string_view::npos) {
		// Couldn't parse element_text completely as number. Pass it as text instead.
		return std::string{element_text.substr(0, element_text.find_last_not_of(" \t") + 1)};
	}
	if(std::holds_alternative<long long>(number_result)) {
		return std::get<long long>(number_result);
	}
	return std::get<double>(number_result);
}

std::vector<command_parser::primitive_argument_type> command_parser::extract_list(std::string_view& input) {
	std::vector<primitive_argument_type> result;
	consume_expected(input, '[');
	consume_whitespace(input);
	bool first = true;
	while(!input.empty() && input.front() != ']' && (first || input.front() == ',')) {
		if(!first) {
			consume_expected(input, ',');
			consume_whitespace(input);
		} else {
			first = false;
		}
		result.push_back(extract_primitive(input, ",]"));
		consume_whitespace(input);
	}
	consume_expected(input, ']');
	return result;
}

std::vector<std::pair<std::string, command_parser::primitive_argument_type>>
command_parser::extract_key_value_list(std::string_view& input) {
	std::vector<std::pair<std::string, primitive_argument_type>> result;
	consume_expected(input, '{');
	consume_whitespace(input);
	bool first = true;
	while(!input.empty() && input.front() != '}' && (first || input.front() == ',')) {
		if(!first) {
			consume_expected(input, ',');
			consume_whitespace(input);
		} else {
			first = false;
		}
		auto key = extract_text(input, "=");
		consume_expected(input, '=');
		consume_whitespace(input);
		auto value = extract_primitive(input, ",}");
		result.emplace_back(key, value);
		consume_whitespace(input);
	}
	consume_expected(input, '}');
	return result;
}

} // namespace minidb
