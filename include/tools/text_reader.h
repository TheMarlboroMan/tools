#pragma once

#include <string>
#include <fstream>
#include <vector>

namespace tools{

//!Line-buffered reader for text files, supporting simple comment syntax
//!(lines beginning with a character, by default #).
class text_reader {
	public:

	enum		flags{
		none=0,
		ltrim=1, //left trim each line
		rtrim=2, //right trim each line
		ignorewscomment=4 //ignores whitespace when looking for the comment character. Has no effect it ltrim is active.
	};

	//!Default constructor for an empty reader. Useless if open_file is not invoked.
				text_reader();

	//!Constructs a file reader for the given file_path, comment char and flags
				text_reader(const std::string&, const char='#', int=none);

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

	//!Returns true if the file has been completely read.
	bool 			is_eof() const {return file.eof();}

	//!Sets a new commenting character.
	void 			set_comment(const char v) {comment=v;}

	//!Sets the flags.
	void			set_flags(int _v) {flags=_v;}

	//!Sets the file pointer back at the first line.
	void 			rewind(){file.seekg(0, file.beg);}

	//TODO: ADD A FOREACH FUNCTION THAT GETS THE CALLBACK AS A PARAMETER.

	private:

	//!Returns true if the line is a comment. Assumes that the line has length.
	bool            is_comment(const std::string&);

	unsigned int    line_number;    //!< Current line number.
	char            comment;        //!< Current comment char.
	std::ifstream   file;           //!< File data.
	int				flags;
};

//!Returns a vector of strings resulting of extracting the contents of a file separated by newlines.
std::vector<std::string> explode_lines_from_file(const std::string&);
}
