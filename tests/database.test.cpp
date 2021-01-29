#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include "test_helpers.hpp"
#include <catch2/catch.hpp>
#include <database.hpp>
#include <map>
#include <variant>
#include <vector>

namespace {
using namespace std::literals;

void create_shop_db(minidb::database& db) {
	db.create_table("article"sv, minidb::schema{{{"article_number", minidb::value_type::integer},
												 {"name", minidb::value_type::string},
												 {"price", minidb::value_type::decimal}}});
	db.create_table("customer"sv, minidb::schema{{{"customer_number", minidb::value_type::integer},
												  {"first_name", minidb::value_type::string},
												  {"last_name", minidb::value_type::string},
												  {"address", minidb::value_type::string}}});
	db.create_table("order"sv, minidb::schema{{{"order_number", minidb::value_type::integer},
											   {"customer_number", minidb::value_type::integer},
											   {"date", minidb::value_type::string}}});
	db.create_table("order_item"sv, minidb::schema{{{"order_number", minidb::value_type::integer},
													{"article_number", minidb::value_type::integer},
													{"count", minidb::value_type::integer},
													{"unit_price", minidb::value_type::decimal}}});
}
void fill_shop_db(minidb::database& db) {
	db.append_row("article"sv, {1LL, "widget"s, 42.12});
	db.append_row("article"sv, {2LL, "gizmo"s, 123.45});

	db.append_row("customer"sv, {10LL, "John"s, "Doe"s, "Fake Street 123, Fake City"s});
	db.append_row("customer"sv, {11LL, "Jane"s, "Smith"s, "Main Street 234, Somewhere"s});

	db.append_row("order"sv, {100LL, 10LL, "2020-10-20"s});
	db.append_row("order_item"sv, {100LL, 1LL, 5LL, 42.12});
	db.append_row("order_item"sv, {100LL, 2LL, 1LL, 123.45});

	db.append_row("order"sv, {101LL, 11LL, "2020-10-23"});
	db.append_row("order_item"sv, {101LL, 1LL, 1LL, 42.12});
	db.append_row("order_item"sv, {101LL, 2LL, 1LL, 123.45});

	db.append_row("order"sv, {102LL, 11LL, "2020-10-29"});
	db.append_row("order_item"sv, {102LL, 1LL, 10LL, 42.12});

	db.append_row("order"sv, {103LL, 11LL, "2020-10-30"});
	db.append_row("order_item"sv, {103LL, 2LL, 10LL, 123.45});

	db.append_row("order"sv, {104LL, 10LL, "2020-10-30"});
	db.append_row("order_item"sv, {104LL, 1LL, 4LL, 42.12});
	db.append_row("order_item"sv, {104LL, 2LL, 3LL, 123.45});

	db.append_row("order"sv, {105LL, 11LL, "2020-11-04"});
	db.append_row("order_item"sv, {105LL, 1LL, 5LL, 42.12});
	db.append_row("order_item"sv, {105LL, 2LL, 8LL, 123.45});

	db.append_row("order"sv, {106LL, 11LL, "2020-11-10"});
	db.append_row("order_item"sv, {106LL, 1LL, 4LL, 42.12});

	db.append_row("order"sv, {107LL, 10LL, "2020-11-15"});
	db.append_row("order_item"sv, {107LL, 1LL, 5LL, 42.12});
	db.append_row("order_item"sv, {107LL, 2LL, 5LL, 123.45});

	db.append_row("order"sv, {108LL, 11LL, "2020-11-19"});
	db.append_row("order_item"sv, {108LL, 1LL, 15LL, 42.12});
}

struct test_fixture {
	minidb::database db;
	test_fixture() {
		create_shop_db(db);
		fill_shop_db(db);
	}
};

using namespace test;

} // namespace

TEST_CASE("The database class can create some sample tables and the structure is correctly reflected in the created "
		  "table objects.",
		  "[database][table][schema]") {
	minidb::database db;
	create_shop_db(db);
	auto& article_tab = db.lookup_table("article");
	CHECK(article_tab.name() == "article");
	CHECK(article_tab.get_column_name(0) == "article_number");
	CHECK(article_tab.get_column_type(0) == minidb::value_type::integer);
	CHECK(article_tab.get_column_name(1) == "name");
	CHECK(article_tab.get_column_type(1) == minidb::value_type::string);
	CHECK(article_tab.get_column_name(2) == "price");
	CHECK(article_tab.get_column_type(2) == minidb::value_type::decimal);

	auto& customer_tab = db.lookup_table("customer");
	CHECK(customer_tab.name() == "customer");
	CHECK(customer_tab.get_column_name(0) == "customer_number");
	CHECK(customer_tab.get_column_type(0) == minidb::value_type::integer);
	CHECK(customer_tab.get_column_name(1) == "first_name");
	CHECK(customer_tab.get_column_type(1) == minidb::value_type::string);
	CHECK(customer_tab.get_column_name(2) == "last_name");
	CHECK(customer_tab.get_column_type(2) == minidb::value_type::string);
	CHECK(customer_tab.get_column_name(3) == "address");
	CHECK(customer_tab.get_column_type(3) == minidb::value_type::string);

	auto& order_tab = db.lookup_table("order");
	CHECK(order_tab.name() == "order");
	CHECK(order_tab.get_column_name(0) == "order_number");
	CHECK(order_tab.get_column_type(0) == minidb::value_type::integer);
	CHECK(order_tab.get_column_name(1) == "customer_number");
	CHECK(order_tab.get_column_type(1) == minidb::value_type::integer);
	CHECK(order_tab.get_column_name(2) == "date");
	CHECK(order_tab.get_column_type(2) == minidb::value_type::string);

	auto& order_item_tab = db.lookup_table("order_item");
	CHECK(order_item_tab.name() == "order_item");
	CHECK(order_item_tab.get_column_name(0) == "order_number");
	CHECK(order_item_tab.get_column_type(0) == minidb::value_type::integer);
	CHECK(order_item_tab.get_column_name(1) == "article_number");
	CHECK(order_item_tab.get_column_type(1) == minidb::value_type::integer);
	CHECK(order_item_tab.get_column_name(2) == "count");
	CHECK(order_item_tab.get_column_type(2) == minidb::value_type::integer);
	CHECK(order_item_tab.get_column_name(3) == "unit_price");
	CHECK(order_item_tab.get_column_type(3) == minidb::value_type::decimal);
}

TEST_CASE("Attempting to create a table with a name that is already used by an existing table results in an "
		  "exception "
		  "and preserves the previous table.",
		  "[database][table]") {
	minidb::database db;
	db.create_table("test", {{{"Test_String", minidb::value_type::string}}});
	CHECK_THROWS(db.create_table("test", {{}}));
	auto& test_tab = db.lookup_table("test");
	CHECK(test_tab.columns().size() == 1);
	CHECK(test_tab.get_column_name(0) == "Test_String");
	CHECK(test_tab.get_column_type(0) == minidb::value_type::string);
}

TEST_CASE_METHOD(test_fixture,
				 "The test database is correctly filled by the append_row calls in the test fixture code.",
				 "[database][row][value]") {
	std::vector<std::vector<minidb::value>> expected_articles = {{1LL, "widget"s, 42.12}, {2LL, "gizmo"s, 123.45}};
	std::vector<std::vector<minidb::value>> expected_customers = {
			{10LL, "John"s, "Doe"s, "Fake Street 123, Fake City"s},
			{11LL, "Jane"s, "Smith"s, "Main Street 234, Somewhere"s}};
	std::vector<std::vector<minidb::value>> expected_orders = {
			{100LL, 10LL, "2020-10-20"s}, {101LL, 11LL, "2020-10-23"}, {102LL, 11LL, "2020-10-29"},
			{103LL, 11LL, "2020-10-30"},  {104LL, 10LL, "2020-10-30"}, {105LL, 11LL, "2020-11-04"},
			{106LL, 11LL, "2020-11-10"},  {107LL, 10LL, "2020-11-15"}, {108LL, 11LL, "2020-11-19"}};
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 1LL, 5LL, 42.12},  {100LL, 2LL, 1LL, 123.45},  {101LL, 1LL, 1LL, 42.12}, {101LL, 2LL, 1LL, 123.45},
			{102LL, 1LL, 10LL, 42.12}, {103LL, 2LL, 10LL, 123.45}, {104LL, 1LL, 4LL, 42.12}, {104LL, 2LL, 3LL, 123.45},
			{105LL, 1LL, 5LL, 42.12},  {105LL, 2LL, 8LL, 123.45},  {106LL, 1LL, 4LL, 42.12}, {107LL, 1LL, 5LL, 42.12},
			{107LL, 2LL, 5LL, 123.45}, {108LL, 1LL, 15LL, 42.12}};

	auto& articles_tab = db.lookup_table("article");
	auto& customers_tab = db.lookup_table("customer");
	auto& orders_tab = db.lookup_table("order");
	auto& order_items_tab = db.lookup_table("order_item");
	check_approx_table(articles_tab, expected_articles);
	check_approx_table(customers_tab, expected_customers);
	check_approx_table(orders_tab, expected_orders);
	check_approx_table(order_items_tab, expected_order_items);
}
TEST_CASE_METHOD(test_fixture, "A table can be querried unfiltered using database::query_table.", "[database][query]") {
	std::vector<std::vector<minidb::value>> expected_customers = {
			{10LL, "John"s, "Doe"s, "Fake Street 123, Fake City"s},
			{11LL, "Jane"s, "Smith"s, "Main Street 234, Somewhere"s}};
	std::size_t index = 0;
	db.query_table("customer"sv, [&index, &expected_customers](const minidb::row& row) {
		CHECK(index < expected_customers.size());
		check_approx_row(row, expected_customers.at(index++));
	});
	CHECK(index == expected_customers.size());
}
TEST_CASE_METHOD(test_fixture, "A table can be querried with a single filter using database::query_table.",
				 "[database][query]") {
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 1LL, 5LL, 42.12}, {101LL, 1LL, 1LL, 42.12}, {102LL, 1LL, 10LL, 42.12}, {104LL, 1LL, 4LL, 42.12},
			{105LL, 1LL, 5LL, 42.12}, {106LL, 1LL, 4LL, 42.12}, {107LL, 1LL, 5LL, 42.12},  {108LL, 1LL, 15LL, 42.12}};
	std::size_t index = 0;
	db.query_table("order_item"sv, {{"article_number"s, 1LL}}, [&index, &expected_order_items](const minidb::row& row) {
		CHECK(index < expected_order_items.size());
		check_approx_row(row, expected_order_items.at(index++));
	});
	CHECK(index == expected_order_items.size());
}
TEST_CASE_METHOD(test_fixture, "A table can be querried with multiple filters using database::query_table.",
				 "[database][query]") {
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 1LL, 5LL, 42.12}, {105LL, 1LL, 5LL, 42.12}, {107LL, 1LL, 5LL, 42.12}};
	std::size_t index = 0;
	db.query_table("order_item"sv, {{"article_number"s, 1LL}, {"count", 5LL}},
				   [&index, &expected_order_items](const minidb::row& row) {
					   CHECK(index < expected_order_items.size());
					   check_approx_row(row, expected_order_items.at(index++));
				   });
	CHECK(index == expected_order_items.size());
}

TEST_CASE_METHOD(
		test_fixture,
		"A table can be querried to obtain a histogram of order items by order number (unfiltered histogram query).",
		"[database][query]") {
	std::map<minidb::value, std::size_t> expected_histogram = {
			{100LL, 2}, {101LL, 2}, {102LL, 1}, {103LL, 1}, {104LL, 2}, {105LL, 2}, {106LL, 1}, {107LL, 2}, {108LL, 1}};
	auto histogram = db.query_column_histogram("order_item"sv, "order_number"sv);
	CHECK(histogram == expected_histogram);
}

TEST_CASE_METHOD(
		test_fixture,
		"A table can be querried to obtain a histogram of order items by article number (unfiltered histogram query).",
		"[database][query]") {
	std::map<minidb::value, std::size_t> expected_histogram = {{1LL, 8}, {2LL, 6}};
	auto histogram = db.query_column_histogram("order_item"sv, "article_number"sv);
	CHECK(histogram == expected_histogram);
}

TEST_CASE_METHOD(test_fixture,
				 "A table can be querried to obtain a histogram of order quantities for a specific article number "
				 "(filtered histogram query).",
				 "[database][query]") {
	std::map<minidb::value, std::size_t> expected_histogram = {{1LL, 1}, {4LL, 2}, {5LL, 3}, {10LL, 1}, {15LL, 1}};
	auto histogram = db.query_column_histogram("order_item"sv, "count"sv, {{"article_number"s, 1LL}});
	CHECK(histogram == expected_histogram);
}

TEST_CASE_METHOD(test_fixture, "Rows in a table can be correctly updated using a row filter.", "[database][update]") {
	std::vector<std::vector<minidb::value>> expected_articles = {{1LL, "widget premium"s, 49.95},
																 {2LL, "gizmo"s, 123.45}};
	std::vector<std::vector<minidb::value>> expected_customers = {
			{10LL, "John"s, "Doe"s, "Example Boulevard 5, Example City"s},
			{11LL, "Jane"s, "Smith"s, "Main Street 234, Somewhere"s}};
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 1LL, 6LL, 40.00},  {100LL, 2LL, 1LL, 123.45},  {101LL, 1LL, 1LL, 42.12}, {101LL, 2LL, 1LL, 123.45},
			{102LL, 1LL, 10LL, 42.12}, {103LL, 2LL, 10LL, 123.45}, {104LL, 1LL, 4LL, 42.12}, {104LL, 2LL, 3LL, 123.45},
			{105LL, 1LL, 6LL, 40.00},  {105LL, 2LL, 8LL, 123.45},  {106LL, 1LL, 4LL, 42.12}, {107LL, 1LL, 6LL, 40.00},
			{107LL, 2LL, 5LL, 123.45}, {108LL, 1LL, 15LL, 42.12}};
	db.update_rows("customer"sv, {{"customer_number", 10LL}}, {{"address", "Example Boulevard 5, Example City"}});
	db.update_rows("article"sv, {{"article_number", 1LL}}, {{"name", "widget premium"}, {"price", 49.95}});
	db.update_rows("order_item"sv, {{"article_number", 1LL}, {"count", 5LL}}, {{"count", 6LL}, {"unit_price", 40.00}});

	auto& articles_tab = db.lookup_table("article");
	auto& customers_tab = db.lookup_table("customer");
	auto& order_items_tab = db.lookup_table("order_item");
	check_approx_table(articles_tab, expected_articles);
	check_approx_table(customers_tab, expected_customers);
	check_approx_table(order_items_tab, expected_order_items);
}
TEST_CASE_METHOD(test_fixture, "A table can be correctly deleted by name.", "[database][table][drop]") {
	db.drop_table("order"sv);
	db.drop_table("article"sv);
	CHECK_THROWS(db.lookup_table("article"sv));
	CHECK_THROWS(db.lookup_table("order"sv));
	// Check the other tables remain in place:
	db.lookup_table("customer");
	db.lookup_table("order_item");
}
TEST_CASE_METHOD(test_fixture, "A row can be correctly erased from a table by index.", "[database][erase]") {
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 1LL, 5LL, 42.12},  {100LL, 2LL, 1LL, 123.45}, {101LL, 1LL, 1LL, 42.12},	 {101LL, 2LL, 1LL, 123.45},
			{102LL, 1LL, 10LL, 42.12}, {104LL, 1LL, 4LL, 42.12},  {104LL, 2LL, 3LL, 123.45}, {105LL, 1LL, 5LL, 42.12},
			{105LL, 2LL, 8LL, 123.45}, {106LL, 1LL, 4LL, 42.12},  {107LL, 1LL, 5LL, 42.12},	 {107LL, 2LL, 5LL, 123.45},
			{108LL, 1LL, 15LL, 42.12}};
	db.erase_row("order_item"sv, 5);
	auto& order_items_tab = db.lookup_table("order_item");
	check_approx_table(order_items_tab, expected_order_items);
}
TEST_CASE_METHOD(test_fixture, "Rows in a table can be correctly erased using a row filter.", "[database][erase]") {
	std::vector<std::vector<minidb::value>> expected_order_items = {
			{100LL, 2LL, 1LL, 123.45},	{101LL, 1LL, 1LL, 42.12},  {101LL, 2LL, 1LL, 123.45}, {102LL, 1LL, 10LL, 42.12},
			{103LL, 2LL, 10LL, 123.45}, {104LL, 1LL, 4LL, 42.12},  {104LL, 2LL, 3LL, 123.45}, {105LL, 2LL, 8LL, 123.45},
			{106LL, 1LL, 4LL, 42.12},	{107LL, 2LL, 5LL, 123.45}, {108LL, 1LL, 15LL, 42.12}};
	db.erase_rows("order_item"sv, {{"article_number", 1LL}, {"count", 5LL}});
	auto& order_items_tab = db.lookup_table("order_item");
	check_approx_table(order_items_tab, expected_order_items);
}
TEST_CASE_METHOD(test_fixture, "Attempting to use a non-existent table throws an exception.", "[database][errors]") {
	SECTION("in a query") {
		bool callback_called = false;
		CHECK_THROWS(db.query_table("test"sv, [&](const auto&) { callback_called = true; }));
		CHECK(!callback_called);
	}
	SECTION("in an update") {
		CHECK_THROWS(db.update_rows("test"sv, {{"dummy", 123LL}}, {{"x", 123.4}}));
	}
	SECTION("in an erase_rows") {
		CHECK_THROWS(db.erase_rows("test"sv, {{"dummy", 123LL}}));
	}
	SECTION("in an erase_row") {
		CHECK_THROWS(db.erase_row("test"sv, 42));
	}
	SECTION("in a drop_table") {
		CHECK_THROWS(db.drop_table("test"sv));
	}
}
TEST_CASE_METHOD(test_fixture, "Attempting to use a non-existent column name in a row filter throws an exception.",
				 "[database][errors]") {
	SECTION("in a query") {
		bool callback_called = false;
		CHECK_THROWS(db.query_table("order_item"sv, {{"dummy", 42LL}}, [&](const auto&) { callback_called = true; }));
		CHECK(!callback_called);
	}
	SECTION("in an update") {
		CHECK_THROWS(db.update_rows("order_item"sv, {{"dummy", 123LL}}, {{"x", 123.4}}));
	}
	SECTION("in an erase_rows") {
		CHECK_THROWS(db.erase_rows("order_item"sv, {{"dummy", 123LL}}));
	}
}
TEST_CASE_METHOD(test_fixture,
				 "Attempting to use a non-existent column name in a get_column_index_by_name throws an exception.",
				 "[database][table][errors]") {
	auto& tab = db.lookup_table("order_item"sv);
	CHECK_THROWS(tab.get_column_index_by_name("dummy"sv));
}
TEST_CASE_METHOD(test_fixture, "Attempting to use an invalid column index throws an exception.",
				 "[database][table][errors]") {
	auto& tab = db.lookup_table("order_item"sv);
	SECTION("in an get_column_name") {
		CHECK_THROWS(tab.get_column_name(4));
	}
	SECTION("in an get_column_type") {
		CHECK_THROWS(tab.get_column_type(4));
	}
	auto& r = tab.rows().at(3);
	SECTION("in an get_column_name") {
		CHECK_THROWS(r.get_cell_value<std::string>(4));
	}
	SECTION("in an get_column_name") {
		CHECK_THROWS(r.set_cell_value(4, "Test"s));
	}
}
TEST_CASE_METHOD(test_fixture, "Attempting to use an invalid row index in erase_row throws an exception.",
				 "[database][errors]") {
	CHECK_THROWS(db.erase_row("order_item"sv, 14));
}
TEST_CASE_METHOD(test_fixture, "Attempting to set a cell to a value of the incorrect type throws an exception.",
				 "[database][table][errors]") {
	auto& tab = db.lookup_table("article"sv);
	auto& r = tab.rows().at(0);
	SECTION("floating point to integer") {
		CHECK_THROWS(r.set_cell_value(0, 123.45));
	}
	SECTION("string to integer") {
		CHECK_THROWS(r.set_cell_value(0, "Test"s));
	}
	SECTION("integer to string") {
		CHECK_THROWS(r.set_cell_value(1, 42LL));
	}
	SECTION("floating point to string") {
		CHECK_THROWS(r.set_cell_value(1, 123.45));
	}
	SECTION("integer to floating point") {
		CHECK_THROWS(r.set_cell_value(2, 42LL));
	}
	SECTION("string to floating point") {
		CHECK_THROWS(r.set_cell_value(2, "Test"s));
	}
}
TEST_CASE_METHOD(test_fixture, "Attempting to read a cell value using the incorrect type throws an exception.",
				 "[database][table][errors]") {
	auto& tab = db.lookup_table("article"sv);
	auto& r = tab.rows().at(0);
	SECTION("floating point from integer") {
		CHECK_THROWS(r.get_cell_value<double>(0));
	}
	SECTION("string from integer") {
		CHECK_THROWS(r.get_cell_value<std::string>(0));
	}
	SECTION("integer from string") {
		CHECK_THROWS(r.get_cell_value<long long>(1));
	}
	SECTION("floating point from string") {
		CHECK_THROWS(r.get_cell_value<double>(1));
	}
	SECTION("integer from floating point") {
		CHECK_THROWS(r.get_cell_value<long long>(2));
	}
	SECTION("string from floating point") {
		CHECK_THROWS(r.get_cell_value<std::string>(2));
	}
}
