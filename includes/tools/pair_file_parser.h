#pragma once

#include "string_utils.h"

#include <map>
#include <exception>

namespace tools{

//!Wrapper for key-value based data, read from a file.

//!Implemented in terms of the text_reader class, so supports comments. The
//!format expects N lines consisting on a single pair of key and value separated
//!by a delimiter character. Lines can also be empty or begin with a comment
//!character. The delimiter and comment character are to be supplied by the 
//!client code.

class pair_file_parser
{
	public:
	//!Class constructor with path, delimiter and comment key. Will load
	//!the file contents into the internal storage. Will throw if the
	//!file cannot be found or has the wrong format.
						pair_file_parser(const std::string&, char, char);

	//!Saves the internal storage data to the file. Will not preserve the
	//!original order of entries not comments or blank lines.
	void					save();

	//!Syncs this instance with the values of a second.
	void					sync(const pair_file_parser& f);

	//!Returns the configuration value with the given key. Throws with 
	//!the corresponding std::map exception if the item cannot be found.
	const std::string&			operator[](const std::string key) const {return data.at(key);}


	//TODO: How do we check if an entry exists?
	//!Returns the configuration value with the given key. Does not do any
	//!Boundary checking, so will fail if the item cannot be found.
	std::string&				operator[](const std::string key) {return data[key];}

	private:

	//!Loads the data into the internal storage 
	void 					load();

	std::string				filename;	//!< Path to the currently loaded file.
	char					delimiter,	//!< Delimiter that separates keys from values.
						comment;	//!< Character that starts a comment line.

	std::map<std::string, std::string>	data;		//!< Internal data storage.
};

}
