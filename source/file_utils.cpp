#include "file_utils.h"

using namespace tools;

bool tools::file_exists(const std::string& c) {

	std::ifstream f(c);
	return f.is_open();
}

std::string tools::dump_file(const std::string& c) {

	std::string res, lin;
	std::ifstream f(c);
	
	while(true) {
		if(f.eof()) {
			break;
		}

		std::getline(f, lin);

		res+=lin+"\n";
	}

	//Remove last newline.
	res.pop_back();
	return res;
}
