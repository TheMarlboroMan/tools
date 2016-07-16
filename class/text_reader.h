#ifndef TEXT_READER_TOOLS_H
#define TEXT_READER_TOOLS_H

#include <string>
#include <fstream>
#include <vector>

namespace tools
{

class text_reader
{
	public:

				text_reader();
				text_reader(const std::string&, const char);

	std::string 		read_line();
	void 			open_file(const std::string&);

	/*explicit*/ operator bool() const {return file;}

	unsigned int 		get_line_number() const {return line_number;}
	char 			get_comment() const {return comment;}
	const std::string& 	get_current_line() {return current_line;}
	bool 			is_eof() const {return file.eof();}

	void 			set_comment(const char v) {comment=v;}
	void 			rewind(){file.seekg(0, file.beg);}

	private:

	unsigned int 		line_number;
	char 			comment;
	std::string 		current_line;
	std::ifstream 		file;
};

std::vector<std::string> explode_lines_from_file(const std::string&);
}

#endif
