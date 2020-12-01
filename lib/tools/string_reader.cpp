#include <tools/string_reader.h>
#include <tools/string_utils.h>


#include <iostream>
#include <stdexcept>

using namespace tools;

string_reader::string_reader()
:
	line_number{0},
	comment{'#'},
	flags{none} {

}

string_reader::string_reader(
	const std::string& _str,
	const char c,
	int _flags)
:
	line_number(0),
	comment(c),
	flags{_flags} {

	set(_str);
}

//TODO: We could actually have some underlying implementation with
//this to be shared with text_reader too, since it's basically the same
//code.

std::string string_reader::read_line() {

	if(stream.eof()) {

		return std::string{};
	}

	while(true){

		++line_number;
		std::string current_line;
		std::getline(stream, current_line);

		if(stream.eof()) {

			return std::string();
		}

		if(flags & string_reader::ltrim ) {

			tools::ltrim(current_line);
		}

		if(flags & string_reader::rtrim) {

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

bool string_reader::is_comment(const std::string& _line) {

	if(flags & ignorewscomment && !(flags & ltrim)) {

		return str_ltrim(_line)[0]==comment;
	}

	return _line[0]==comment;
}

bool string_reader::set(const std::string& _str) {

	line_number=0;
	stream.str(_str);
	return (bool)stream;
}
