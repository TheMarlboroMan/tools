#ifndef TEXT_READER_TOOLS_H
#define TEXT_READER_TOOLS_H

#include <string>
#include <fstream>
#include <vector>

namespace tools
{

//!Line-buffered reader for text files, supporting simple comment syntax (lines beginning with a character, by default #).
class text_reader {
	public:

	//!Default constructor for an empty reader. Useless if open_file is not invoked.
				text_reader();

	//!Constructs a file reader for the given file_path and comment char,
				text_reader(const std::string&, const char);

	//!Reads the next line. If the next line is empty or is a comment will try to read the next one. A blank string is returned when no more lines are available.
	std::string 		read_line();

	//!Opens a new file. Returns true if the file could be opened.
	bool 			open_file(const std::string&);

	//!Returns true if the reader has an assigned file. Useful to evaluate after "open_file."
	/*explicit*/ operator bool() const {return (bool)file;}

	//!Returns the current line number (comments and blank lines are considered).
	unsigned int 		get_line_number() const {return line_number;}

	//!Returns the current comment char.
	char 			get_comment() const {return comment;}

	//!Returns the current line. read_line() will advance the line pointer.
	const std::string& 	get_current_line() {return current_line;}

	//!Returns true if the file has been completely read.
	bool 			is_eof() const {return file.eof();}

	//!Sets a new commenting character.
	void 			set_comment(const char v) {comment=v;}

	//!Sets the file pointer back at the first line.
	void 			rewind(){file.seekg(0, file.beg);}

	private:

	unsigned int 		line_number;		//!< Current line number.
	char 			comment;		//!< Current comment char.
	std::string 		current_line;		//!< Current line.
	std::ifstream 		file;			//!< File data.
};

std::vector<std::string> explode_lines_from_file(const std::string&);
}

#endif
