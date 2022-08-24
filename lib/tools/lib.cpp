#include <tools/lib.h>

#include <sstream>

using namespace tools;

std::string tools::get_lib_version() {

	std::stringstream ss;
	ss<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION;
	return ss.str();
}
