#include <tools/pair_file_parser.h>
#include <tools/text_reader.h>
#include <tools/compatibility_patches.h>

#include <fstream>

using namespace tools;

pair_file_parser::pair_file_parser(const std::string& nf, char sep, char com):
	filename(nf), delimiter(sep), comment(com) {

	try {
		load();
	}
	catch(std::exception& e) {
		throw std::runtime_error("Unable to parse pair file "+filename+" : "+e.what());
	}
}

void pair_file_parser::save() {

	std::ofstream fichero(filename.c_str());
	
	for(const auto& par : data) {
		fichero<<par.first<<delimiter<<par.second<<std::endl;
	}
}

void pair_file_parser::sync(const pair_file_parser& f) {

	for(const auto& par : f.data) {
		if(!data.count(par.first)) {
			data[par.first]=f.data.at(par.first);
		}
	}
}

void pair_file_parser::load() {

	//TODO: This is preposterous: the text-reader could implement a foreach.
	text_reader L(filename.c_str(), comment);

	if(!L) {
		throw std::runtime_error("tools::pair_file_parser::load could not open "+filename);
	}

	std::string line;

	while(true) {
		line=L.read_line();

		if(L.is_eof()) {
			break;
		}
		//Localizar delimiter... 
		else if(line.find(delimiter)==line.npos) {
			throw std::runtime_error("tools::map_pair malformed line "+compat::to_string(L.get_line_number())+" '"+line+"' in "+filename);
		}

		std::vector<std::string> values=explode(line, delimiter, 2);
		data[values[0]]=values[1];
	}
}

