#include "test_helpers.hpp"
#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>
#include <util.hpp>

namespace test {

void check_approx_value(const minidb::value& actual, const minidb::value& expected) {
	using namespace minidb;
	std::visit(overloaded{[](double a, double b) { CHECK(a == Approx(b)); },
						  // Adding these would allow type differences between numeric types:
						  //[](double a, long long b) { CHECK(a == Approx(b)); },
						  //[](long long a, double b) { CHECK(Approx(a) == b); },
						  []<typename T>(const T& a, const T& b) { CHECK(a == b); },
						  [](const auto&, const auto&) {
							  bool correct_type = false;
							  CHECK(correct_type); // Use variable name to indicate what went wrong.
						  }},
			   actual, expected);
}

void check_approx_row(const minidb::row& actual, const std::vector<minidb::value>& expected_values) {
	for(std::size_t column_index = 0; column_index < expected_values.size(); ++column_index) {
		CAPTURE(column_index);
		check_approx_value(actual.get_cell_value(column_index), expected_values.at(column_index));
	}
}

void check_approx_table(const minidb::table& actual, const std::vector<std::vector<minidb::value>>& expected_data) {
	CHECK(actual.rows().size() == expected_data.size());
	for(std::size_t row_index = 0; row_index < expected_data.size(); ++row_index) {
		CAPTURE(row_index);
		check_approx_row(actual.rows().at(row_index), expected_data.at(row_index));
	}
}

void check_approx_output(const std::string& output, const std::vector<std::string> expected_contents) {
	CAPTURE(output);
	auto expected_it = expected_contents.begin();
	std::size_t scan_pos = 0;
	auto alnum_pred = [](unsigned char c) -> bool { return std::isalnum(c); };
	while(expected_it != expected_contents.end()) {
		CAPTURE(*expected_it, scan_pos);
		auto pos = output.find(*expected_it, scan_pos);
		bool found = pos != std::string::npos;
		CHECK(found);
		// Check if there is actual data (letters and numbers) in the space that we skipped between two expected
		// outputs. This assumes that other characters (e.g. whitespace, '-', '|', '+', '=') might be ther for
		// formatting purposes and can legitimately be skipped. Alpha-numberic characters between expected outputs
		// indicates unexpected outputs from the code under test.
		CAPTURE(pos);
		bool skipped_actual_data =
				std::any_of(output.begin() + scan_pos, output.begin() + std::min(pos, output.size()), alnum_pred);
		CHECK(!skipped_actual_data);
		scan_pos = pos + expected_it->size();
		++expected_it;
	}
	bool skipped_actual_data = std::any_of(output.begin() + scan_pos, output.end(), alnum_pred);
	CHECK(!skipped_actual_data);
}

} // namespace test
