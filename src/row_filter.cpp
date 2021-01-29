#include <algorithm>
#include <iterator>
#include <row_filter.hpp>
#include <util.hpp>

namespace minidb {

void row_filter::bind_to_table(const table& tab) {

	for(const auto& pair : filters) {
		const auto& name = pair.first;
		int index = -1;
		const auto& it =
				std::find_if(tab.columns().begin(), tab.columns().end(),
				             [&name, &index](const column& column)
				             {
					             ++index;
					             return column.name() == name;
				             });
		if(it == tab.columns().end()) throw
				std::invalid_argument("Table doesn't contain the row: " + std::string(name));
		column_indexed_filters[index] = pair.second;
	}
}

bool row_filter::operator()(const row& r) {

	for(const auto& pair : column_indexed_filters) {
		if( r.get_cell_value_if(pair.first) != pair.second) {
			return false;
		}
	}
	// Placeholder to avoid warnings (remove when implemented):
	return true;
}

} // namespace minidb
