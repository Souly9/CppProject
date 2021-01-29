#include <ostream>
#include <value.hpp>

namespace minidb {

std::ostream& operator<<(std::ostream& ostr, const minidb::value& val) {
	visit([&ostr](const auto& v) { ostr << v; }, val);
	return ostr;
}

} // namespace minidb