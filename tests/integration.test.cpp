#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include "test_helpers.hpp"
#include <catch2/catch.hpp>
#include <command_processor.hpp>
#include <database.hpp>
#include <sstream>

namespace {
struct test_fixture {
	minidb::database db;
	minidb::command_processor cmd_proc;
	std::stringstream output;
	test_fixture() : cmd_proc(db) {}
};
using namespace test;
} // namespace

TEST_CASE_METHOD(test_fixture, "The create_table command can be successfully used to create a table.",
				 "[integration][table]") {
	cmd_proc.execute("create_table creation-test { foo = integer, bar=decimal, baz=string}", output);
	const auto& tab = db.lookup_table("creation-test");
	CHECK(tab.name() == "creation-test");
	CHECK(tab.columns().size() == 3);
	CHECK(tab.columns().at(0).name() == "foo");
	CHECK(tab.columns().at(0).type() == minidb::value_type::integer);
	CHECK(tab.columns().at(1).name() == "bar");
	CHECK(tab.columns().at(1).type() == minidb::value_type::decimal);
	CHECK(tab.columns().at(2).name() == "baz");
	CHECK(tab.columns().at(2).type() == minidb::value_type::string);
}

TEST_CASE_METHOD(test_fixture, "The drop_table command can be successfully used to delete a table.",
				 "[integration][table]") {
	db.create_table("drop-test",
					minidb::schema{{{"A", minidb::value_type::integer}, {"B", minidb::value_type::string}}});
	cmd_proc.execute("drop_table  drop-test ", output);
	CHECK_THROWS(db.lookup_table("drop-test"));
}

TEST_CASE_METHOD(test_fixture, "The append_row command can be successfully used to append a row to an existing table.",
				 "[integration][append]") {
	db.create_table("append-test", minidb::schema{{{"A", minidb::value_type::integer},
												   {"B", minidb::value_type::string},
												   {"C", minidb::value_type::decimal}}});
	cmd_proc.execute("append_row  append-test  [ 42 , \"Hello World\" , -123.456 ] ", output);
	const auto& tab = db.lookup_table("append-test");
	CHECK(tab.rows().size() == 1);
	CHECK(tab.rows().at(0).get_cell_value<long long>(0) == 42);
	CHECK(tab.rows().at(0).get_cell_value<std::string>(1) == "Hello World");
	CHECK(tab.rows().at(0).get_cell_value<double>(2) == Approx(-123.456));
}

TEST_CASE_METHOD(test_fixture,
				 "The update_rows command can be successfully used to change cell values in an existing table.",
				 "[integration][update]") {
	db.create_table("update-test", minidb::schema{{{"X", minidb::value_type::integer},
												   {"Y", minidb::value_type::integer},
												   {"A", minidb::value_type::integer},
												   {"B", minidb::value_type::string},
												   {"C", minidb::value_type::decimal}}});
	db.append_row("update-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("update-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("update-test", {1LL, 3LL, 23LL, "This", 345.67});
	db.append_row("update-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("update-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("update-test", {2LL, 3LL, 56LL, "Test", 678.9});
	db.append_row("update-test", {3LL, 1LL, 78LL, "abc", 789.01});
	db.append_row("update-test", {3LL, 2LL, 89LL, "def", 890.12});
	db.append_row("update-test", {3LL, 3LL, 90LL, "ghi", 901.23});

	std::vector<std::vector<minidb::value>> expected1 = {{1LL, 1LL, 42LL, "Hello", 123.45},
														 {1LL, 2LL, 12LL, "World", 234.56},
														 {1LL, 3LL, 23LL, "This", 345.67},
														 {2LL, 1LL, 34LL, "is", 456.78},
														 {2LL, 2LL, 12345LL, "Test, Test, Test!", 42.42},
														 {2LL, 3LL, 56LL, "Test", 678.9},
														 {3LL, 1LL, 78LL, "abc", 789.01},
														 {3LL, 2LL, 89LL, "def", 890.12},
														 {3LL, 3LL, 90LL, "ghi", 901.23}};
	cmd_proc.execute("update_rows update-test { X = 2 , Y = 2 } { A = 12345 , B = \"Test, Test, Test!\" , C = 42.42 }",
					 output);
	check_approx_table(db.lookup_table("update-test"), expected1);

	std::vector<std::vector<minidb::value>> expected2 = {{1LL, 1LL, 42LL, "Hello", 123.45},
														 {1LL, 2LL, 12LL, "World", 234.56},
														 {1LL, 3LL, 9876LL, "ABCDEF", 345.67},
														 {2LL, 1LL, 34LL, "is", 456.78},
														 {2LL, 2LL, 12345LL, "Test, Test, Test!", 42.42},
														 {2LL, 3LL, 9876LL, "ABCDEF", 678.9},
														 {3LL, 1LL, 78LL, "abc", 789.01},
														 {3LL, 2LL, 89LL, "def", 890.12},
														 {3LL, 3LL, 9876LL, "ABCDEF", 901.23}};
	cmd_proc.execute("update_rows update-test {Y=3} {A=9876,B=ABCDEF}", output);
	check_approx_table(db.lookup_table("update-test"), expected2);

	std::vector<std::vector<minidb::value>> expected3 = {{1LL, 1LL, 42LL, "Hello", 123.45},
														 {1LL, 2LL, 12LL, "World", 234.56},
														 {1LL, 3LL, 9876LL, "ABCDEF", 345.67},
														 {4LL, 1LL, 34LL, "is", 456.78},
														 {4LL, 2LL, 12345LL, "Test, Test, Test!", 42.42},
														 {4LL, 3LL, 9876LL, "ABCDEF", 678.9},
														 {3LL, 1LL, 78LL, "abc", 789.01},
														 {3LL, 2LL, 89LL, "def", 890.12},
														 {3LL, 3LL, 9876LL, "ABCDEF", 901.23}};
	cmd_proc.execute("update_rows update-test {X=2} {X=4}", output);
	check_approx_table(db.lookup_table("update-test"), expected3);
}

TEST_CASE_METHOD(test_fixture,
				 "The erase_rows command can be successfully used to remove rows matching a filter from a table.",
				 "[integration][erase]") {
	db.create_table("erase-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("erase-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("erase-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("erase-test", {1LL, 3LL, 23LL, "This", 345.67});
	db.append_row("erase-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("erase-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("erase-test", {2LL, 3LL, 56LL, "Test", 678.9});
	db.append_row("erase-test", {3LL, 1LL, 78LL, "abc", 789.01});
	db.append_row("erase-test", {3LL, 2LL, 89LL, "def", 890.12});
	db.append_row("erase-test", {3LL, 3LL, 90LL, "ghi", 901.23});

	std::vector<std::vector<minidb::value>> expected1 = {
			{1LL, 1LL, 42LL, "Hello", 123.45}, {1LL, 2LL, 12LL, "World", 234.56}, {1LL, 3LL, 23LL, "This", 345.67},
			{2LL, 1LL, 34LL, "is", 456.78},	   {2LL, 3LL, 56LL, "Test", 678.9},	  {3LL, 1LL, 78LL, "abc", 789.01},
			{3LL, 2LL, 89LL, "def", 890.12},   {3LL, 3LL, 90LL, "ghi", 901.23}};
	cmd_proc.execute("erase_rows erase-test {Y=2,X=2}", output);
	check_approx_table(db.lookup_table("erase-test"), expected1);
	std::vector<std::vector<minidb::value>> expected2 = {{1LL, 1LL, 42LL, "Hello", 123.45},
														 {1LL, 2LL, 12LL, "World", 234.56},
														 {2LL, 1LL, 34LL, "is", 456.78},
														 {3LL, 1LL, 78LL, "abc", 789.01},
														 {3LL, 2LL, 89LL, "def", 890.12}};
	cmd_proc.execute("erase_rows erase-test {Y=3}", output);
	check_approx_table(db.lookup_table("erase-test"), expected2);
}

TEST_CASE_METHOD(test_fixture, "The erase_row command can be successfully used to remove a rows from a table by index.",
				 "[integration][erase]") {
	db.create_table("erase-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("erase-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("erase-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("erase-test", {1LL, 3LL, 23LL, "This", 345.67});
	db.append_row("erase-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("erase-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("erase-test", {2LL, 3LL, 56LL, "Test", 678.9});

	std::vector<std::vector<minidb::value>> expected = {{1LL, 1LL, 42LL, "Hello", 123.45},
														{1LL, 2LL, 12LL, "World", 234.56},
														{1LL, 3LL, 23LL, "This", 345.67},
														{2LL, 1LL, 34LL, "is", 456.78},
														{2LL, 3LL, 56LL, "Test", 678.9}};
	cmd_proc.execute("erase_row erase-test 4", output);
	check_approx_table(db.lookup_table("erase-test"), expected);
}
TEST_CASE_METHOD(test_fixture, "The update_cell command can be successfully used to update a single value in a row.",
				 "[integration][update]") {
	db.create_table("update-test", minidb::schema{{{"X", minidb::value_type::integer},
												   {"Y", minidb::value_type::integer},
												   {"A", minidb::value_type::integer},
												   {"B", minidb::value_type::string},
												   {"C", minidb::value_type::decimal}}});
	db.append_row("update-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("update-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("update-test", {1LL, 3LL, 23LL, "This", 345.67});
	db.append_row("update-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("update-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("update-test", {2LL, 3LL, 56LL, "Test", 678.9});

	std::vector<std::vector<minidb::value>> expected = {{1LL, 1LL, 42LL, "Hello", 123.45},
														{1LL, 2LL, 424242LL, "World", 234.56},
														{1LL, 3LL, 23LL, "Test Test Test!", 345.67},
														{2LL, 1LL, 34LL, "is", 456.78},
														{2LL, 2LL, 45LL, "a", 567.89},
														{2LL, 3LL, 56LL, "Test", 123456.7}};
	cmd_proc.execute("update_cell update-test  1  2  424242  ", output);
	cmd_proc.execute("update_cell update-test  2  3  \"Test Test Test!\"  ", output);
	cmd_proc.execute("update_cell update-test  5  4  123456.7  ", output);
	check_approx_table(db.lookup_table("update-test"), expected);
}

TEST_CASE_METHOD(test_fixture, "The query_table command can be successfully used to display all rows in a table.",
				 "[integration][query]") {
	db.create_table("query-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("query-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("query-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("query-test", {1LL, 3LL, 23LL, "This", 345.67});
	db.append_row("query-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("query-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("query-test", {2LL, 3LL, 56LL, "Test", 678.9});
	db.append_row("query-test", {3LL, 1LL, 78LL, "abc", 789.01});
	db.append_row("query-test", {3LL, 2LL, 89LL, "def", 890.12});
	db.append_row("query-test", {3LL, 3LL, 90LL, "ghi", 901.23});

	// clang-format off
	std::vector<std::string> expected = {
		"1", "1", "42", "Hello", "123.45",
		"1", "2", "12", "World", "234.56",
		"1", "3", "23", "This" , "345.67",
		"2", "1", "34", "is"   , "456.78",
		"2", "2", "45", "a"    , "567.89",
		"2", "3", "56", "Test" , "678.9" ,
		"3", "1", "78", "abc"  , "789.01",
		"3", "2", "89", "def"  , "890.12",
		"3", "3", "90", "ghi"  , "901.23"
	};
	// clang-format on

	cmd_proc.execute("query_table query-test  ", output);
	check_approx_output(output.str(), expected);
}

TEST_CASE_METHOD(test_fixture,
				 "The query_table command can be successfully used to display a subset of the rows in a table, "
				 "specified by a row filter.",
				 "[integration][query]") {
	db.create_table("query-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("query-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("query-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("query-test", {1LL, 3LL, 23LL, "ABCD", 345.67});
	db.append_row("query-test", {2LL, 1LL, 34LL, "is", 456.78});
	db.append_row("query-test", {2LL, 2LL, 45LL, "a", 567.89});
	db.append_row("query-test", {2LL, 3LL, 56LL, "ABCD", 678.9});
	db.append_row("query-test", {3LL, 1LL, 78LL, "abc", 789.01});
	db.append_row("query-test", {3LL, 2LL, 89LL, "def", 890.12});
	db.append_row("query-test", {3LL, 3LL, 90LL, "ABCD", 901.23});
	db.append_row("query-test", {3LL, 3LL, 90LL, "Hello World", 901.23});
	db.append_row("query-test", {4LL, 1LL, 90LL, "ABCD", 901.23});

	// clang-format off
	std::vector<std::string> expected = {
		"1", "3", "23", "ABCD", "345.67",
		"2", "3", "56", "ABCD", "678.9" ,
		"3", "3", "90", "ABCD", "901.23"
	};
	// clang-format on

	cmd_proc.execute("query_table query-test { B = ABCD, Y=3} ", output);
	check_approx_output(output.str(), expected);
}

TEST_CASE_METHOD(test_fixture,
				 "The query_column_histogram command can be successfully used to generate a histogram of the values "
				 "of a column of a table.",
				 "[integration][query][histogram]") {
	db.create_table("query-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("query-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("query-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("query-test", {1LL, 3LL, 23LL, "ABCD", 345.67});
	db.append_row("query-test", {2LL, 1LL, 34LL, "Test", 456.78});
	db.append_row("query-test", {2LL, 2LL, 45LL, "Hello", 567.89});
	db.append_row("query-test", {2LL, 3LL, 56LL, "ABCD", 678.9});
	db.append_row("query-test", {3LL, 1LL, 78LL, "test", 789.01});
	db.append_row("query-test", {3LL, 2LL, 89LL, "World", 890.12});
	db.append_row("query-test", {3LL, 3LL, 90LL, "ABCD", 901.23});
	db.append_row("query-test", {3LL, 3LL, 90LL, "Hello", 901.23});
	db.append_row("query-test", {4LL, 1LL, 90LL, "ABCD", 901.23});

	// clang-format off
	std::vector<std::string> expected = {
		"ABCD" ,"4",
		"Hello","3",
		"Test" ,"1",
		"World","2",
		"test" ,"1"
	};
	// clang-format on

	cmd_proc.execute("query_column_histogram query-test B", output);
	check_approx_output(output.str(), expected);
}

TEST_CASE_METHOD(test_fixture,
				 "The query_column_histogram command can be successfully used to generate a histogram of the values of "
				 "a column in a subset of the filtered row of a table.",
				 "[integration][query][histogram]") {
	db.create_table("query-test", minidb::schema{{{"X", minidb::value_type::integer},
												  {"Y", minidb::value_type::integer},
												  {"A", minidb::value_type::integer},
												  {"B", minidb::value_type::string},
												  {"C", minidb::value_type::decimal}}});
	db.append_row("query-test", {1LL, 1LL, 42LL, "Hello", 123.45});
	db.append_row("query-test", {1LL, 2LL, 12LL, "World", 234.56});
	db.append_row("query-test", {1LL, 3LL, 23LL, "ABCD", 345.67});
	db.append_row("query-test", {2LL, 1LL, 34LL, "Test", 456.78});
	db.append_row("query-test", {2LL, 2LL, 45LL, "Hello", 567.89});
	db.append_row("query-test", {2LL, 3LL, 56LL, "ABCD", 678.9});
	db.append_row("query-test", {3LL, 1LL, 78LL, "ABCD", 789.01});
	db.append_row("query-test", {3LL, 2LL, 89LL, "World", 890.12});
	db.append_row("query-test", {3LL, 3LL, 90LL, "ABCD", 901.23});
	db.append_row("query-test", {3LL, 3LL, 90LL, "ABCD", 901.23});
	db.append_row("query-test", {3LL, 3LL, 90LL, "Hello", 901.23});
	db.append_row("query-test", {4LL, 1LL, 90LL, "ABCD", 901.23});

	SECTION("filtered by string, histogram over integers") {
		// clang-format off
		std::vector<std::string> expected = {
			"1", "2",
			"3", "4"
		};
		// clang-format on

		cmd_proc.execute("query_column_histogram query-test Y {B=ABCD}", output);
		check_approx_output(output.str(), expected);
	}
	SECTION("filtered by integer, histogram over strings") {
		// clang-format off
		std::vector<std::string> expected = {
			"ABCD", "3",
			"Hello", "1",
			"World", "1"
		};
		// clang-format on

		cmd_proc.execute("query_column_histogram query-test B {X=3}", output);
		check_approx_output(output.str(), expected);
	}
}
