#ifndef TEST_HELPERS_INCLUDED
#define TEST_HELPERS_INCLUDED

#include <table.hpp>
#include <value.hpp>
#include <vector>

namespace test {

void check_approx_value(const minidb::value& actual, const minidb::value& expected);
void check_approx_row(const minidb::row& actual, const std::vector<minidb::value>& expected_values);
void check_approx_table(const minidb::table& actual, const std::vector<std::vector<minidb::value>>& expected_data);
void check_approx_output(const std::string& output, const std::vector<std::string> expected_contents);

} // namespace test

#endif // TEST_HELPERS_INCLUDED
