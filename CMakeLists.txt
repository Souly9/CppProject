cmake_minimum_required (VERSION 3.16)

project(MiniDB)

include(./StrictCompilerSettings.cmake)
include(./Catch2_submodule.cmake)

add_library(minidb_lib
	include/table.hpp
	include/schema.hpp
	include/database.hpp
	include/command_processor.hpp
	include/value.hpp
	include/util.hpp
	include/row_filter.hpp
	include/command_parser.hpp
	src/table.cpp
	src/database.cpp
	src/command_processor.cpp
	src/value.cpp
	src/row_filter.cpp
	src/command_parser.cpp
)
target_compile_features(minidb_lib PUBLIC cxx_std_20)
enable_strict_compiler_settings(minidb_lib)
target_include_directories(minidb_lib PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)


add_executable(minidb cli/main.cpp)
target_link_libraries(minidb PRIVATE minidb_lib)
enable_strict_compiler_settings(minidb)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT minidb)

add_executable(minidb_tests
	tests/main.test.cpp
	tests/database.test.cpp
	tests/command_parsing.test.cpp
	tests/integration.test.cpp
	tests/test_helpers.cpp
	tests/test_helpers.hpp
)
target_link_libraries(minidb_tests PRIVATE
	minidb_lib
	Catch2::Catch2
)
enable_strict_compiler_settings(minidb_tests)
catch_discover_tests(minidb_tests)
