#include <tools/file_utils.h>
#include <tools/platform.h>

#include <stdexcept>
#include <iostream>

using namespace tools;


//TODO: Use filesystem!!
bool tools::file_exists(const std::string& _path) {

	std::ifstream f(_path);
	return f.is_open();
}

std::string tools::dump_file(const std::string& _path) {

	std::ifstream f(_path);
	if(!f) {
		throw std::runtime_error(std::string{"dump_file failed, could not open "}+_path);
	}

	std::string res, lin;
	while(std::getline(f, lin)) {
		res+=lin+tools::newline;
	}

	return res;
}
