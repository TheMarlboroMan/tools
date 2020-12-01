#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace tools{

/**
* Line-buffered reader for strings in the style of text_reader.
*/
class string_reader {
	public:

	enum		flags{
		none=0,
		ltrim=1, //left trim each line
		rtrim=2, //right trim each line
		ignorewscomment=4 //ignores whitespace when looking for the comment character. Has no effect it ltrim is active.
	};

	//!Default constructor for an empty reader.
	                    string_reader();

	//!Constructs a file reader for the given string, comment char and flags
	                    string_reader(const std::string&, const char='#', int=none);

	//!Reads the next line. If the next line is empty or is a comment will try to read the next one. A blank string is returned when no more lines are available.
	std::string         read_line();

	//!Sets the contents of the reader.
	bool                set(const std::string&);

	//!Returns stream state.
	/*explicit*/ operator bool() const {return (bool)stream;}

	//!Returns the current line number (comments and blank lines are considered).
	unsigned int        get_line_number() const {return line_number;}

	//!Returns the current comment char.
	char                get_comment() const {return comment;}

	//!Returns true if the file has been completely read.
	bool                is_eof() const {return stream.eof();}

	//!Sets a new commenting character.
	void                set_comment(const char v) {comment=v;}

	//!Sets the flags.
	void                set_flags(int _v) {flags=_v;}

	//!Sets the pointer back at the first line.
	void                rewind(){stream.seekg(0, stream.beg);}

	private:

	//!Returns true if the line is a comment. Assumes that the line has length.
	bool                is_comment(const std::string&);

	unsigned int        line_number;    //!< Current line number.
	char                comment;        //!< Current comment char.
	std::stringstream   stream;         //!< File data.
	int                 flags;
};

}
