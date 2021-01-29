#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include <catch2/catch.hpp>
#include <command_parser.hpp>

TEST_CASE("The command parser correctly parses an argument-less command line.", "[parsing]") {
	SECTION("without whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command");
		CHECK(cmd == "test_command");
		CHECK(args.empty());
	}
	SECTION("with added whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t  test_command \t  ");
		CHECK(cmd == "test_command");
		CHECK(args.empty());
	}
}

TEST_CASE("The command parser correctly parses a command line containing a command with a single integer argument.",
		  "[parsing]") {
	using elem_type = minidb::command_parser::integer_argument_type;
	SECTION("positive, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command 12345");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == 12345);
	}
	SECTION("negative, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command -12345");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == -12345);
	}
	SECTION("positive, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t  test_command    12345  \t  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == 12345);
	}
	SECTION("negative, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t\t test_command  \t   -12345  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == -12345);
	}
}

TEST_CASE("The command parser correctly parses a command line containing a command with a single decimal argument.",
		  "[parsing]") {
	using elem_type = minidb::command_parser::decimal_argument_type;
	SECTION("positive, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command 123.45");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == Approx(123.45));
	}
	SECTION("negative, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command -123.45");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == Approx(-123.45));
	}
	SECTION("positive, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t  test_command    123.45  \t  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == Approx(123.45));
	}
	SECTION("negative, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t\t test_command  \t   -123.45  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<elem_type>(args.at(0)));
		CHECK(std::get<elem_type>(args.at(0)) == Approx(-123.45));
	}
}

TEST_CASE("The command parser correctly parses a command line containing a command with a single string argument.",
		  "[parsing]") {
	using expected_type = minidb::command_parser::string_argument_type;
	SECTION("unquoted, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command Hello");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == "Hello");
	}
	SECTION("quoted, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command \"Hello World\"");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == "Hello World");
	}
	SECTION("unquoted, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t  test_command    Hello  \t  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == "Hello");
	}
	SECTION("quoted, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("  \t\t test_command  \t   \"Hello World\"  ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == "Hello World");
	}
}

TEST_CASE("The command parser correctly parses a command line containing a command with a single list argument.",
		  "[parsing]") {
	using expected_type = minidb::command_parser::list_argument_type;
	SECTION("homogeneous, integers, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command [1,-2,3,-4,5,-6]");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == expected_type({1LL, -2LL, 3LL, -4LL, 5LL, -6LL}));
	}
	SECTION("homogeneous, decimal, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command [12.34,-234.56,3456.78,-45678.901]");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using elem_type = minidb::command_parser::decimal_argument_type;
		CHECK(std::get<elem_type>(lst.at(0)) == Approx(12.34));
		CHECK(std::get<elem_type>(lst.at(1)) == Approx(-234.56));
		CHECK(std::get<elem_type>(lst.at(2)) == Approx(3456.78));
		CHECK(std::get<elem_type>(lst.at(3)) == Approx(-45678.901));
	}
	SECTION("homogeneous, strings, without additional whitespace") {
		auto [cmd, args] =
				minidb::command_parser::parse_command("test_command [Hello,World,\"This is a test!\",\"Hello World\"]");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using elem_type = minidb::command_parser::string_argument_type;
		CHECK(std::get<elem_type>(lst.at(0)) == "Hello");
		CHECK(std::get<elem_type>(lst.at(1)) == "World");
		CHECK(std::get<elem_type>(lst.at(2)) == "This is a test!");
		CHECK(std::get<elem_type>(lst.at(3)) == "Hello World");
	}
	SECTION("heterogeneous, without additional whitespace") {
		auto [cmd, args] =
				minidb::command_parser::parse_command("test_command [Hello,123456,\"This is a test!\",1234.5678]");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using int_type = minidb::command_parser::integer_argument_type;
		using dec_type = minidb::command_parser::decimal_argument_type;
		using str_type = minidb::command_parser::string_argument_type;
		CHECK(std::get<str_type>(lst.at(0)) == "Hello");
		CHECK(std::get<int_type>(lst.at(1)) == 123456);
		CHECK(std::get<str_type>(lst.at(2)) == "This is a test!");
		CHECK(std::get<dec_type>(lst.at(3)) == Approx(1234.5678));
	}
	SECTION("heterogeneous, with added whitespace") {
		auto [cmd, args] =
				minidb::command_parser::parse_command("  \t test_command  \t   [ \t  Hello   ,\t\t123456\t  \t, \t   "
													  "\"This is a test!\"   , \t\t 1234.5678 \t\t\t]  \t ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using int_type = minidb::command_parser::integer_argument_type;
		using dec_type = minidb::command_parser::decimal_argument_type;
		using str_type = minidb::command_parser::string_argument_type;
		CHECK(std::get<str_type>(lst.at(0)) == "Hello");
		CHECK(std::get<int_type>(lst.at(1)) == 123456);
		CHECK(std::get<str_type>(lst.at(2)) == "This is a test!");
		CHECK(std::get<dec_type>(lst.at(3)) == Approx(1234.5678));
	}
}

TEST_CASE("The command parser correctly parses a command line containing a command with a single key-value-list "
		  "argument.",
		  "[parsing]") {
	using expected_type = minidb::command_parser::key_value_list_argument_type;
	SECTION("homogeneous, integer values, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command("test_command {foo=1,bar=-2,\"baz\"=3}");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) == expected_type({{"foo", 1LL}, {"bar", -2LL}, {"baz", 3LL}}));
	}
	SECTION("homogeneous, decimal values, without additional whitespace") {
		auto [cmd, args] =
				minidb::command_parser::parse_command("test_command {foo=123.456,\"bar\"=-2345.67,baz=34.5678}");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 3);
		using val_type = minidb::command_parser::decimal_argument_type;
		CHECK(lst.at(0).first == "foo");
		CHECK(std::get<val_type>(lst.at(0).second) == Approx(123.456));
		CHECK(lst.at(1).first == "bar");
		CHECK(std::get<val_type>(lst.at(1).second) == Approx(-2345.67));
		CHECK(lst.at(2).first == "baz");
		CHECK(std::get<val_type>(lst.at(2).second) == Approx(34.5678));
	}
	SECTION("homogeneous, string values, without additional whitespace") {
		auto [cmd, args] =
				minidb::command_parser::parse_command("test_command {foo=Test,\"bar\"=\"Hello World\",baz=\"12345\"}");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		CHECK(std::get<expected_type>(args.at(0)) ==
			  expected_type({{"foo", "Test"}, {"bar", "Hello World"}, {"baz", "12345"}}));
	}
	SECTION("heterogeneous, without additional whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command(
				"test_command {test=Hello,\"test test\"=123456,foo=\"This is a test!\",bar=1234.5678}");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using int_type = minidb::command_parser::integer_argument_type;
		using dec_type = minidb::command_parser::decimal_argument_type;
		using str_type = minidb::command_parser::string_argument_type;
		CHECK(lst.at(0).first == "test");
		CHECK(std::get<str_type>(lst.at(0).second) == "Hello");
		CHECK(lst.at(1).first == "test test");
		CHECK(std::get<int_type>(lst.at(1).second) == 123456);
		CHECK(lst.at(2).first == "foo");
		CHECK(std::get<str_type>(lst.at(2).second) == "This is a test!");
		CHECK(lst.at(3).first == "bar");
		CHECK(std::get<dec_type>(lst.at(3).second) == Approx(1234.5678));
	}
	SECTION("heterogeneous, with added whitespace") {
		auto [cmd, args] = minidb::command_parser::parse_command(
				"  \t test_command\t    { \t  test=Hello\t\t ,\t  \"test,test\"   = \t 123456   \t ,  \t \tfoo   =   "
				"\"This is a test!\"\t\t\t,    bar \t   =   1234.5678   }   ");
		CHECK(cmd == "test_command");
		CHECK(args.size() == 1);
		CHECK(std::holds_alternative<expected_type>(args.at(0)));
		const auto& lst = std::get<expected_type>(args.at(0));
		CHECK(lst.size() == 4);
		using int_type = minidb::command_parser::integer_argument_type;
		using dec_type = minidb::command_parser::decimal_argument_type;
		using str_type = minidb::command_parser::string_argument_type;
		CHECK(lst.at(0).first == "test");
		CHECK(std::get<str_type>(lst.at(0).second) == "Hello");
		CHECK(lst.at(1).first == "test,test");
		CHECK(std::get<int_type>(lst.at(1).second) == 123456);
		CHECK(lst.at(2).first == "foo");
		CHECK(std::get<str_type>(lst.at(2).second) == "This is a test!");
		CHECK(lst.at(3).first == "bar");
		CHECK(std::get<dec_type>(lst.at(3).second) == Approx(1234.5678));
	}
}

TEST_CASE("The command parser correctly parses a command line containing multiple arguments.", "[parsing]") {
	std::string cmd_line;
	SECTION("without additional whitespace") {
		cmd_line = "test_command -123 987.654 abc \"Test Test Test!\" [12345,123.456,xyz,\"Hello World\"] "
				   "{test=Hello,\"test,test\"=123456,foo=\"This is a test!\",bar=1234.5678}";
	}
	SECTION("with added whitespace") {
		cmd_line = "  \t test_command\t\t   -123 \t  \t 987.654  \t   abc     \"Test Test Test!\"  \t\t  [ \t 12345    "
				   ",\t\t123.456   , \t \t xyz   ,  \t \"Hello World\"      ]  \t   \t\t {    test   = \t\t  Hello   , "
				   "  \"test,test\" \t\t =   123456  ,  foo  \t =\t\t\"This is a test!\"   ,   bar \t  =   1234.5678   "
				   " }\t   ";
	}
	auto [cmd, args] = minidb::command_parser::parse_command(std::move(cmd_line));
	CHECK(cmd == "test_command");
	CHECK(args.size() == 6);
	auto* arg0 = std::get_if<minidb::command_parser::integer_argument_type>(&args.at(0));
	REQUIRE(arg0 != nullptr);
	CHECK(*arg0 == -123);
	auto* arg1 = std::get_if<minidb::command_parser::decimal_argument_type>(&args.at(1));
	REQUIRE(arg1 != nullptr);
	CHECK(*arg1 == Approx(987.654));
	auto* arg2 = std::get_if<minidb::command_parser::string_argument_type>(&args.at(2));
	REQUIRE(arg2 != nullptr);
	CHECK(*arg2 == "abc");
	auto* arg3 = std::get_if<minidb::command_parser::string_argument_type>(&args.at(3));
	REQUIRE(arg3 != nullptr);
	CHECK(*arg3 == "Test Test Test!");

	auto* arg4 = std::get_if<minidb::command_parser::list_argument_type>(&args.at(4));
	REQUIRE(arg4 != nullptr);
	CHECK(arg4->size() == 4);
	CHECK(std::get<minidb::command_parser::integer_argument_type>(arg4->at(0)) == 12345);
	CHECK(std::get<minidb::command_parser::decimal_argument_type>(arg4->at(1)) == Approx(123.456));
	CHECK(std::get<minidb::command_parser::string_argument_type>(arg4->at(2)) == "xyz");
	CHECK(std::get<minidb::command_parser::string_argument_type>(arg4->at(3)) == "Hello World");

	auto* arg5 = std::get_if<minidb::command_parser::key_value_list_argument_type>(&args.at(5));
	REQUIRE(arg5 != nullptr);
	CHECK(arg5->size() == 4);
	CHECK(arg5->at(0).first == "test");
	CHECK(std::get<minidb::command_parser::string_argument_type>(arg5->at(0).second) == "Hello");
	CHECK(arg5->at(1).first == "test,test");
	CHECK(std::get<minidb::command_parser::integer_argument_type>(arg5->at(1).second) == 123456);
	CHECK(arg5->at(2).first == "foo");
	CHECK(std::get<minidb::command_parser::string_argument_type>(arg5->at(2).second) == "This is a test!");
	CHECK(arg5->at(3).first == "bar");
	CHECK(std::get<minidb::command_parser::decimal_argument_type>(arg5->at(3).second) == Approx(1234.5678));
}
