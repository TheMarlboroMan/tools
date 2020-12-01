#include <tools/text_reader.h>
#include <tools/string_utils.h>


#include <iostream>
#include <stdexcept>

using namespace tools;

text_reader::text_reader()
:
	line_number{0},
	comment{'#'},
	flags{none} {

}

text_reader::text_reader(
	const std::string& path,
	const char c,
	int _flags)
:
	line_number(0),
	comment(c),
	flags{_flags} {

	open_file(path);
}

std::string text_reader::read_line() {

	if(file.eof()) {

		return std::string{};
	}

	while(true){

		++line_number;
		std::string current_line;
		std::getline(file, current_line);

		if(file.eof()) {

			return std::string();
		}

		if(flags & text_reader::ltrim ) {

			tools::ltrim(current_line);
		}

		if(flags & text_reader::rtrim) {

			tools::rtrim(current_line);
		}

		if(!current_line.size()) {

			continue;
		}

		if(is_comment(current_line)) {
			continue;
		}

		return current_line;
	}
}

bool text_reader::is_comment(const std::string& _line) {

	if(flags & ignorewscomment && !(flags & ltrim)) {

		return str_ltrim(_line)[0]==comment;
	}

	return _line[0]==comment;
}

bool text_reader::open_file(const std::string& path) {

	if(file) {
		file.close();
	}

	line_number=0;
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
