#include <tools/text_reader.h>
#include <iostream>
#include <stdexcept>

using namespace tools;

text_reader::text_reader() 
	:line_number(0), comment('#') {

}

text_reader::text_reader(const std::string& path, const char c)
	:line_number(0), comment(c) {
	open_file(path);
}

std::string text_reader::read_line() {

	if(file.eof()) 	{
		current_line="";
	}
	else {
		++line_number;
		std::getline(file, current_line);
		if(!current_line.size() || (current_line.size() && current_line[0]==comment)) {
			return read_line();
		}
	}

	return current_line;
}

bool text_reader::open_file(const std::string& path) {

	if(file) file.close();
	line_number=0;
	current_line="";
	file.open(path.c_str());
	return (bool)file;
}

std::vector<std::string> tools::explode_lines_from_file(const std::string& path) {

	text_reader L(path, '#');
	std::vector<std::string> result;

	if(!L) 	{
		throw std::runtime_error("Unable to explode lines from "+path);
	}

	while(true) {
		std::string linea=L.read_line();
		if(!L) break;
		result.push_back(linea);
	}

	return result;
}
