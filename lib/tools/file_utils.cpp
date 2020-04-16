#include <tools/file_utils.h>

#include <stdexcept>
#include <iostream>

using namespace tools;

bool tools::file_exists(const std::string& c) {

	std::ifstream f(c);
	return f.is_open();
}

std::string tools::dump_file(const std::string& c) {

	std::ifstream f(c);
	if(!f) {
		throw std::runtime_error("dump_file failed, not a file");
	}

	std::string res, lin;
	while(std::getline(f, lin)) {
		res+=lin+"\n";
	}

	if(res.size()) {
		res.pop_back();
	}

	return res;
}
