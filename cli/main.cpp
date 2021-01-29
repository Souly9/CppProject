#include <command_processor.hpp>
#include <database.hpp>
#include <iostream>
#include <string>

int main() {
	minidb::database db;
	minidb::command_processor cmd_proc(db);
	std::string line;
	std::cout << "minidb> ";
	while(!cmd_proc.should_exit() && getline(std::cin, line)) {
		if(line.empty()) continue;
		try {
			cmd_proc.execute(line, std::cout);
		} catch(const std::exception& ex) {
			std::cerr << "Error: " << ex.what() << "\n";
		}
		std::cout << "minidb> ";
	}
}
