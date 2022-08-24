#include <tools/file_utils.h>
#include <tools/platform.h>

#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace tools;

std::string tools::dump_file(const std::string& _path) {

	std::ifstream f(_path);
	if(!f) {
		throw std::runtime_error(std::string{"dump_file failed, could not open "}+_path);
	}

	std::stringstream ss;
	ss<<f.rdbuf();
	return ss.str();
}
